#include "headers.h"
#include "signal_handlers.h"
#include "prompt.h"

int main(int argc, char **argv)
{
    if (getcwd(PATH_TO_HISTORY_FILE, BUFFER_SIZE) == NULL)
    {
        perror("getcwd() error");
        return EXIT_FAILURE;
    }

    strcat(PATH_TO_HISTORY_FILE, "/history.txt");

    signal(SIGCHLD, child_termination_handler);

    while (1)
    {
        if (prompt() == EXIT_FAILURE)
        {
            fprintf(stderr, "Error starting shell\n");
            return EXIT_FAILURE;
        }

        readline();
    }

    return 0;
}

// https://stackoverflow.com/questions/448944/c-non-blocking-keyboard-input
// https://www.thegeekstuff.com/2014/01/linux-interrupts/