#include <stdio.h>
#include <fcntl.h>      // open
#include <unistd.h>     // read, write, close
#include <termios.h>    // termios, tcgetattr, tcsetattr
#include <string.h>     // memset
#include <sys/time.h>   // struct timeval tv;
#include <stdint.h>
#include <syslog.h>

// --- Soros portok listázása --- 
// dmesg | grep tty
// Ez megmutatja, hogy mely soros eszközök jelentek meg, pl: ttyUSB0

// --- Permission denied ---
// Ez Linuxon gyakran azért van, mert a soros port eszköz fájl az dialout csoporthoz van rendelve, 
// és a felhasználód nincs ebben a csoportban
// ls -l /dev/ttyUSB0
// crw-rw---- 1 root dialout 188, 0 Jun  4 10:00 /dev/ttyUSB0
// Add hozzá a felhasználódat a dialout csoporthoz:
// sudo usermod -a -G dialout marrob



// canonical módban:
// read() csak akkor tér vissza, ha a sor teljes, azaz záró karaktert (pl. Enter-\n) kapott.
// portname:"/dev/ttyS0"
// baudrate:B9600

int UART_Open(const char *portname, speed_t baudrate)
{
    int huart = open(portname, O_RDWR | O_NOCTTY | O_SYNC);
    if (huart < 0)
    {
        perror("UART_Open: Serial Port can't open");
        syslog(LOG_ERR,"UART_Open: Serial Port cant'open. Port name: %s", portname);
        return -1;
    }

    // Soros port beállítása
    struct termios tty;
    memset(&tty, 0, sizeof tty);

    if (tcgetattr(huart, &tty) != 0)
    {
        syslog(LOG_ERR,"UART_Open: tcgetattr error");
        perror("tcgetattr error");
        close(huart);
        return -1;
    }

    // Baudrate beállítás (pl. 9600 baud)
     cfsetospeed(&tty, baudrate);
     cfsetispeed(&tty, baudrate);

    // 8N1 beállítás: 8 adatbit, nincs paritás, 1 stopbit
    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8 adatbit
    tty.c_cflag &= ~PARENB;                         // paritás kikapcsolva
    tty.c_cflag &= ~CSTOPB;                         // 1 stopbit
    tty.c_cflag &= ~CRTSCTS;                        // RTS/CTS kikapcsolva (hardveres flow control)
    tty.c_cflag |= CREAD | CLOCAL;                  // bekapcsoljuk a vevőt, és helyi kapcsolat

    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);  // nyers mód (canonical mód kikapcsolása)

    //tty.c_lflag |= ICANON ;                         // canonical mód, a read()read Enter \n-ig olvas!!!!
    //tty.c_lflag &= ~(ECHO | ECHOE | ISIG);          // canonical mód, a read()read Enter \n-ig olvas!!!!
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);           // szoftveres flow control kikapcsolása
    tty.c_oflag &= ~OPOST;                            // kikapcsolja a kimeneti posztprocesszlast, tehat nem fogja peldaul a '\n' karaktert automatikusan '\r\n'-re cserelni.

    // VMIN és VTIME beállítása (olvasási timeout)
    tty.c_cc[VMIN] = 0;      // minimum 0 karakter várakozás
    tty.c_cc[VTIME] = 1;     // 0.1sec resolution Timeout, csak non-canonical módban

    if (tcsetattr(huart, TCSANOW, &tty) != 0)
    {
        syslog(LOG_ERR,"UART_Open: tcsetattr errror");
        perror("tcsetattr error");
        close(huart);
        return -1;
    }

    syslog(LOG_ERR, "UART_Open: Succesfully");

    return huart;
}

int UART_Write(int huart, char *string)
{
    int len = strlen(string);
    int wlen = write(huart, string, len);
    if (wlen != len)
    {
        perror("UART_Write: error wlen != len");
	syslog(LOG_ERR,"UART_Write: error wlen != len");
        close(huart);
        return -1;
    }
}

static long get_timestamp_ms(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    long timestamp_ms =  (int64_t)tv.tv_sec * 1000 + tv.tv_usec / 1000;
    return timestamp_ms;
}
/*
- termination: '\n' in Linux syststems
- timeout_ms: eg 1000ms
- hiába küld az eszköz \r zárókaraktert valami kicseréli \n-re és ez jelenik meg a bufferben...
*/
int UART_Read(int huart, char *buffer, size_t size, char termination, int timeout_ms)
{

    char ch;
    size_t i = 0;
    long start = get_timestamp_ms();

    while(1)
    {
        int rdlen = read(huart, &ch, 1);
        if(rdlen > 0)
        {
            buffer[i]=ch;
            i++;
            if(ch == termination)
            {
                buffer[i] = '\0'; // close the string
                return i;
            }
        }

        if (i >= size - 1)
        {
            buffer[i] = '\0';
            perror("UART_Read: Buffer overrun error.");
            syslog(LOG_ERR,"UART_Read: Buffer overrun error.");
            return -2; // buffer overrun
        }

        if(get_timestamp_ms() - start >  timeout_ms)
        {
            buffer[i] = '\0'; // close the string
            perror("UART_Read: Timeout error.");
            syslog(LOG_ERR,"UART_Read: Timeout error.");
            return -3;
        }
        syslog(LOG_DEBUG, "UART_Read: ts: %ld, start:%ld, ts-start:%ld", get_timestamp_ms(), start, get_timestamp_ms()-start);
    }
}

//portname: 
void UART_Test(const char *portname)
{
    int huart = UART_Open(portname, B9600);
    char rxString[128] = {0};
    char txString[] = "Hello, World!\n"; //cannonicalban ha nics Enter-\n akkor read() vár...
    printf("UART_Test started...\n");

    for(int i =  0; i < 3; i++)
    {
        UART_Write(huart, txString);
        usleep(100);
        UART_Read(huart, rxString, sizeof(rxString), '\n', 1000 );

        if(strcmp(txString,rxString) == 0)
        {
            printf("Passed\n");
        }else{
            printf("Failed\n");
        }
    }
    close(huart);
}


