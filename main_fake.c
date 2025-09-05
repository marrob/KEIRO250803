
#include "main.h"
#include <syslog.h>
#include <stdbool.h>

// így theted írásvédetté: chmod 444 .vscode/tasks.json, hogy a VSCode ne basztassa


int keep_running = 1;

int main(void)
{

  openlog(SERVICE_NAME, LOG_PID | LOG_CONS, LOG_DAEMON);
  syslog(LOG_INFO, "Fake service debug started...");

  if(Task_Init() == -1)
    keep_running = 0;

  while (keep_running)
  {
    if(Task_Run(true) == 0)
      break;
  }

  syslog(LOG_INFO, "Fake service stopped...");
  closelog();
  Task_Close();
  
  return 0;
}
