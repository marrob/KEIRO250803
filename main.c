#include <stdio.h>
#include <stdbool.h>    
#include <string.h>     // strcmp
#include <unistd.h>     // usleep()

#include "uart.h"
#include "main.h"
#include "tools.h"

// így theted írásvédetté: chmod 444 .vscode/tasks.json, hogy a VSCode ne basztassa

char UartRxBuffer[UART_BUFFER_SIZE];

int main(void)
{
   
    UART_Test(UART_NAME);

    long timestamp = HAL_GetTick();
    int downcounter = 5;
    int huart = UART_Open(UART_NAME, B115200);
    do
    {
        UART_Write(huart, "*OPC?\n");
        UART_Read(huart, UartRxBuffer, sizeof(UartRxBuffer), '\n', 1000);
        if(strcmp("*OPC\n", UartRxBuffer)==0){
            printf("Live\n");
        }
        usleep(100);
        if( HAL_GetTick() -  timestamp > 1000 )
        {
             timestamp = HAL_GetTick();

             printf("Running until: %ds \r\n", downcounter--);
        }
    }while (downcounter);

   return 0;
}
