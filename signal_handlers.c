#include "signal_handlers.h"
#include "headers.h"

void child_termination_handler()
{
    int wstatus;
    int pid = waitpid(-1, &wstatus, WNOHANG);
    if (pid > 0)
    {
        if (WIFEXITED(wstatus))
        {
            int status = WEXITSTATUS(wstatus);

            if (status == EXIT_FAILURE)
            {
                fprintf(stderr, "\nProcess with pid %d ended abnormally\n", pid);
            }
            else
            {
                fprintf(stderr, "\nProcess with pid %d ended normally\n", pid);
            }
        }
    }
}
