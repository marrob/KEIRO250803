#include <stdio.h>
#include <stdbool.h>    
#include <string.h>     // strcmp
#include <unistd.h>     // usleep()
#include <syslog.h>

#include <ctype.h> //isspace

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
int cmdIndex;


char* trim(char* str) {
    char* end;

    // Bal oldali szok�z�k kihagy�sa
    while (isspace((unsigned char)*str)) str++;

    if (*str == 0)  // Ha csak sz�k�z volt
        return str;

    // Jobb oldali sz�k�z�k elt�vol�t�sa
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;

    // Null termin�tor elhelyez�se
    *(end + 1) = '\0';

    return str;
}

int Task_Init(void)
{
  strcpy(UartName, UART_NAME);
  syslog(LOG_INFO, "%s Task_Init Begin.", SERVICE_NAME);

  timestamp = HAL_GetTick();
  downcounter = 5;
  huart = UART_Open(UartName, B115200 /*B9600*/);
  cmdIndex = 0;

  if(huart == -1)
  {
    syslog(LOG_ERR,  "%s Serial port cant open... UART name: %s", SERVICE_NAME, UartName);
    return ERROR_CODE_COM_PORT_CANT_OPEN;
  }

  syslog(LOG_INFO, "%s Task_Init Completed", SERVICE_NAME);

  return 0;
}



int Task_Run(void)
{

  if(HAL_GetTick() -  timestamp > 1000 )
  {
    timestamp = HAL_GetTick();
    //syslog(LOG_DEBUG, "%s Running until: %ds", SERVICE_NAME, downcounter++);


    switch(cmdIndex)
    {
      case 0:
      {
        
        strcpy(UartTxBuffer, "*OPC?\n");
        UART_Write(huart, UartTxBuffer);
        UART_Read(huart, UartRxBuffer, sizeof(UartRxBuffer), '\n', 1000);
        if(strcmp("*OPC\n", UartRxBuffer) == 0){
          syslog(LOG_DEBUG, "%s Req: %s -> Res: %s", SERVICE_NAME, trim(UartTxBuffer), trim(UartRxBuffer));
        }
        else {
          syslog(LOG_ERR, "%s Req: %s Unknown Response: %s", SERVICE_NAME, trim(UartTxBuffer), trim(UartRxBuffer));
        }

        cmdIndex ++;
        break;
      }

      case 1:
      {
        strcpy(UartTxBuffer, "RPI:REQ:OFF?\n");
        UART_Write(huart, UartTxBuffer);
        UART_Read(huart, UartRxBuffer, sizeof(UartRxBuffer), '\n', 1000);
        if(strcmp("YES\n", UartRxBuffer) == 0){
          syslog(LOG_DEBUG, "%s Req: %s -> Res: %s", SERVICE_NAME, trim(UartTxBuffer), trim(UartRxBuffer));
        }
        else if(strcmp("NO\n", UartRxBuffer) == 0){
          syslog(LOG_DEBUG, "%s Req: %s -> Res: %s", SERVICE_NAME, trim(UartTxBuffer), trim(UartRxBuffer));
        }
        else {
          syslog(LOG_ERR, "%s Req: %s -> Unknown Response: %s", SERVICE_NAME, trim(UartTxBuffer), trim(UartRxBuffer));
        }
        cmdIndex = 0;
        break;
      }

      default: 
      {
        cmdIndex = 0;
      }
    
    }
  }

  return downcounter;
}


void Task_Close(void)
{
   close(huart);
}