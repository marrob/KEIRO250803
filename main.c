
#include "main.h"
#include <syslog.h>

// így theted írásvédetté: chmod 444 .vscode/tasks.json, hogy a VSCode ne basztassa


int keep_running = 1;

int main(void)
{

  openlog(SERVICE_NAME, LOG_PID | LOG_CONS, LOG_DAEMON);
  syslog(LOG_INFO, "%s fake service debug started...", SERVICE_NAME);

  if(Task_Init() == -1)
    keep_running = 0;

  while (keep_running)
  {
    if(Task_Run() == 0)
      break;
  }

  syslog(LOG_INFO, "%s fake service stopped...", SERVICE_NAME);
  closelog();
  return 0;
}
