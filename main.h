#ifndef  __MAIN_H
#define  __MAIN_H

#define UART_BUFFER_SIZE 128
#define UART_NAME "/dev/ttyUSB0"

#define SERVICE_NAME "MRPI250610"
#define ERROR_CODE_COM_PORT_CANT_OPEN -2 

#define LOG_PATH "log.txt"
#define LOG_MSG_SIZE 128

int Task_Init(void);
int Task_Run(void);

#endif