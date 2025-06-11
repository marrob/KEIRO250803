#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <syslog.h>
#include <signal.h> // Hozzáadva a jelkezeléshez
#include "main.h"



// Globális változó a leállítási jel fogadására
volatile sig_atomic_t keep_running = 1;

// Jelkezelő függvény
void sigterm_handler(int signum) 
{
    keep_running = 0;
    syslog(LOG_INFO, "%s SIGTERM is received, the service is stoping", SERVICE_NAME);
}


int main() {
    pid_t pid = fork(); //Elengedi a szülőfolyamatot, így a démon a háttérbe kerül.

    if (pid < 0) exit(EXIT_FAILURE); // hiba
    if (pid > 0) exit(EXIT_SUCCESS); // szülő kilép

    // 1. új session
    if (setsid() < 0) exit(EXIT_FAILURE);

    // 2. második fork
    pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS);

    // 3. jogosultságok és könyvtár
    umask(0); // Beállítja a fájl létrehozási jogosultságokat, hogy a démon által létrehozott fájlok teljes jogosultsággal rendelkezzenek.
    chdir("/"); //Megváltoztatja a démon munkakönyvtárát a gyökérkönyvtárra, elkerülve a problémákat a csatlakoztatott fájlrendszerek lecsatolásakor.

    //printf("mydaemon vagyok\n");

    // 4. fájlleírók zárása
    //Ez a kulcsfontosságú rész, ami a kérdésedhez kapcsolódik. Lezárja a standard bemeneti, kimeneti és hibakimeneti fájlleírókat.
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    // Kezdjük el a syslog használatát
    openlog(SERVICE_NAME, LOG_PID | LOG_CONS, LOG_DAEMON);
    syslog(LOG_INFO,  "%s I'm a service and this is a system log.", SERVICE_NAME);
    
    // Regisztráljuk a jelkezelőt a SIGTERM (leállítási) jelre
    signal(SIGTERM, sigterm_handler);

    Task_Init();

    // --- A fő ciklus, ami fenntartja a démont ---
    while (keep_running)
    {
        //syslog(LOG_INFO, "mydaemon fut, az aktuális idő: %ld", (long)time(NULL));
        //sleep(5); // A démon 5 másodpercenként dolgozik/naplóz

        if(Task_Run() == 0)
            break;
    }

    // A ciklusból való kilépés után
    syslog(LOG_INFO, "%s service is stopped...", SERVICE_NAME);
    closelog(); // Ne felejtsd el lezárni a syslog kapcsolatot

    return EXIT_SUCCESS;
}