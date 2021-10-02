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

            int rc = remove_background_jobs(pid);
        }
    }
}

void ctrl_z_handler()
{
    if (foreground_pid != -1)
    {
        int rc = kill(foreground_pid, SIGTSTP);

        if (rc == -1)
        {
            perror("kill() error");
        }

        int res = add_background_jobs(cmd_name, foreground_pid);
    }
}

void ctrl_c_handler()
{
    if (foreground_pid != -1)
    {
        int rc = kill(foreground_pid, SIGKILL);

        if (rc == -1)
        {
            perror("kill() error");
        }
    }
}
