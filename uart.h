
#ifndef __UART_H
#define __UART_H

#include <termios.h>    // termios, tcgetattr, tcsetattr


int UART_Open(const char *portname, speed_t baudrate);
int UART_Write(int huart, char *string);
int UART_Read(int huart, char *buffer, size_t size, char termination, int timeout_ms);
void UART_Test(const char *portname);

#endif