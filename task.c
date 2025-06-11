#include <stdio.h>
#include <stdbool.h>    
#include <string.h>     // strcmp
#include <unistd.h>     // usleep()
#include <syslog.h>

#include "uart.h"
#include "main.h"
#include "tools.h"


long timestamp;
int downcounter;
int huart;
char UartRxBuffer[UART_BUFFER_SIZE];
char UartName[50];
char LogMsg[LOG_MSG_SIZE];


int Task_Init(void)
{
  strcpy(UartName, UART_NAME);
  syslog(LOG_INFO, "%s Task_Init Begin.", SERVICE_NAME);

  timestamp = HAL_GetTick();
  downcounter = 5;
  huart = UART_Open(UartName, B115200);

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
    syslog(LOG_INFO, "%s Running until: %ds", SERVICE_NAME, downcounter--);

    UART_Write(huart, "*OPC?\n");
    UART_Read(huart, UartRxBuffer, sizeof(UartRxBuffer), '\n', 1000);
    if(strcmp("*OPC\n", UartRxBuffer) == 0)
    {
      syslog(LOG_INFO, "%s Ok Response: %s", SERVICE_NAME, UartRxBuffer);
    }
    else
    {
      syslog(LOG_INFO, "%s Unknown Response: %s", SERVICE_NAME, UartRxBuffer);
    }
  }

  return downcounter;
}