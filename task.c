#include <stdio.h>
#include <stdbool.h>    
#include <string.h>     // strcmp
#include <unistd.h>     // usleep()
#include <syslog.h>
#include <stdlib.h>     // system()

#include "uart.h"
#include "main.h"
#include "tools.h"


long timestamp;
int downcounter;
int huart;
char UartRxBuffer[UART_BUFFER_SIZE];
char UartTxBuffer[UART_BUFFER_SIZE];
char UartName[50];
char LogMsg[LOG_MSG_SIZE];
int stateIndex;

enum State_e
{
  ST_OPC_QUERY,
  ST_REQ_OFF_QUERY,
  ST_WAIT_FOR_SHUTDOWN
};



int Task_Init(void)
{
  strcpy(UartName, UART_NAME);
  syslog(LOG_INFO, "%s Task_Init Begin.", SERVICE_NAME);

  timestamp = HAL_GetTick();
  downcounter = 5;
  huart = UART_Open(UartName, B115200 /*B9600*/);

  stateIndex = ST_OPC_QUERY;

  if(huart == -1)
  {
    syslog(LOG_ERR,  "%s Serial port cant open... UART name: %s", SERVICE_NAME, UartName);
    return ERROR_CODE_COM_PORT_CANT_OPEN;
  }

  syslog(LOG_INFO, "%s Task_Init Completed", SERVICE_NAME);

  return 0;
}



int Task_Run(bool debug)
{

  if(HAL_GetTick() -  timestamp > 1000 )
  {
    timestamp = HAL_GetTick();

    if(debug)
    {
      downcounter--;
      if(downcounter < 0)
        return 0;
    }
    
    switch(stateIndex)
    {
      case ST_OPC_QUERY:
      {
        
        strcpy(UartTxBuffer, "*OPC?\n");
        UART_Write(huart, UartTxBuffer);
        UART_Read(huart, UartRxBuffer, sizeof(UartRxBuffer), '\n', 1000);
        if(strcmp("*OPC\n", UartRxBuffer) == 0)
        {
          syslog(LOG_DEBUG, "%s Req: %s -> Res: %s", SERVICE_NAME, trim(UartTxBuffer), trim(UartRxBuffer));
          stateIndex = ST_REQ_OFF_QUERY;
        }
        else 
        {
          syslog(LOG_ERR, "%s Req: %s Unknown Response: %s", SERVICE_NAME, trim(UartTxBuffer), trim(UartRxBuffer));
          stateIndex = ST_OPC_QUERY;
        }
        break;
      }

      case ST_REQ_OFF_QUERY:
      {
        strcpy(UartTxBuffer, "RPI:REQ:OFF?\n");
        UART_Write(huart, UartTxBuffer);
        UART_Read(huart, UartRxBuffer, sizeof(UartRxBuffer), '\n', 1000);
        if(strcmp("YES\n", UartRxBuffer) == 0)
        {
          syslog(LOG_DEBUG, "%s Req: %s -> Res: %s", SERVICE_NAME, trim(UartTxBuffer), trim(UartRxBuffer));

          system("systemctl poweroff");
          stateIndex = ST_WAIT_FOR_SHUTDOWN;
        }
        else if(strcmp("NO\n", UartRxBuffer) == 0)
        {
          syslog(LOG_DEBUG, "%s Req: %s -> Res: %s", SERVICE_NAME, trim(UartTxBuffer), trim(UartRxBuffer));
          stateIndex = ST_OPC_QUERY;
        }
        else 
        {
          syslog(LOG_ERR, "%s Req: %s -> Unknown Response: %s", SERVICE_NAME, trim(UartTxBuffer), trim(UartRxBuffer));
          stateIndex = ST_OPC_QUERY;
        }

        break;
      }

      case ST_WAIT_FOR_SHUTDOWN: 
      {
        
        break;
      }

      default: 
      {
        stateIndex = ST_OPC_QUERY;
      }
    
    }
  }

  return true;
}


void Task_Close(void)
{
   close(huart);
}