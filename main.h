#ifndef  __MAIN_H
#define  __MAIN_H

#define UART_BUFFER_SIZE 128


// --- Rasberry Pi 4 Model B ---
// serial0 = ttyAMA1
// Ez van kivezetve a GPIO 14 (TX) es GPIO 15 (RX) labakra
// Ezert /dev/ttyAMA1 a hasznalhato UART a GPIO-n keresztal

// USB to Serial adapter on Ubuntu 20.04
// #define UART_NAME "/dev/ttyUSB0"

// GPIO 14/15
#define UART_NAME "/dev/serial0"

#define SERVICE_NAME "KEIRO250803"
#define ERROR_CODE_COM_PORT_CANT_OPEN -2 

#define LOG_PATH "log.txt"
#define LOG_MSG_SIZE 128

int Task_Init(void);
int Task_Run(void);
void Task_Close(void);

#endif