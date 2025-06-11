#include <stdio.h>
#include <time.h>

// Felhasználó saját könyvtárába: fopen(getenv("HOME") "/myapp.log", "a");
// /tmp mappába ideiglenesen: fopen("/tmp/myapp.log", "a");
//  Rendszerszintű naplózásra (haladó): syslog()
// Log fájl a program mellé:
// FILE* logf = fopen("main.log", "a"); // vagy "w" ha mindig új fájlt akarsz
// fprintf(logf, "Program elindult\n");
// fclose(logf);


// sample: Log_Write(LOG_PATH, "Task_Init Completed\n");


void TimestampIntoFile(FILE *hfile) 
{
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    char buffer[64];

    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", t);
    fprintf(hfile, "[%s]:", buffer);
}



int Log_Write(const char *path, const char *message)
{
  FILE *hfile;

  hfile = fopen (path, "a+"); // open appending
  if(hfile == NULL)
  {
    printf("Error opening the file\n");
    return -1;
  }

  TimestampIntoFile(hfile);
  fprintf(hfile, "%s", message);
  fclose(hfile);
}   