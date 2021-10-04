#include "headers.h"

int piping_and_redirection(char **args, int arg_count)
{
    char *infile = (char *)malloc(BUFFER_SIZE);

    if (infile == NULL)
    {
        fprintf(stderr, "Error allocating memory\n");
        return EXIT_FAILURE;
    }

    char *outfile = (char *)malloc(BUFFER_SIZE);

    if (outfile == NULL)
    {
        fprintf(stderr, "Error allocating memory\n");
        free(infile);
        return EXIT_FAILURE;
    }

    strcpy(infile, "");
    strcpy(outfile, "");

    int saved_stdout = dup(STDOUT_FILENO);

    if (saved_stdout == -1)
    {
        perror("dup() error");
        free(infile);
        free(outfile);
        return EXIT_FAILURE;
    }

    int saved_stdin = dup(STDIN_FILENO);

    if (saved_stdin == -1)
    {
        perror("dup() error");
        free(infile);
        free(outfile);
        close(saved_stdout);
        return EXIT_FAILURE;
    }

    int is_append = 0, has_input_redirection = 0, has_output_redirection = 0;

    for (int i = 0; i < arg_count;)
    {
        if (strcmp(args[i], "<") == 0)
        {
            ++i;
            has_input_redirection = 1;
            while (i < arg_count && strcmp(args[i], ">") != 0 &&
                   strcmp(args[i], ">>") != 0 && strcmp(args[i], "|") != 0)
            {
                strcpy(infile, args[i++]);
            }
        }
        else if (strcmp(args[i], ">") == 0)
        {
            ++i;
            has_output_redirection = 0;
            while (i < arg_count && strcmp(args[i], "<") != 0 &&
                   strcmp(args[i], ">>") != 0 && strcmp(args[i], "|") != 0)
            {
                strcpy(outfile, args[i++]);
            }
        }
        else if (strcmp(args[i], ">>") == 0)
        {
            is_append = 1;
            has_output_redirection = 0;
            ++i;
            while (i < arg_count && strcmp(args[i], "<") != 0 &&
                   strcmp(args[i], ">") != 0 && strcmp(args[i], "|") != 0)
            {
                strcpy(outfile, args[i++]);
            }
        }
        else
        {
            struct simple_commands *node = create_new_command();
            int j = 0;
            if (strcmp(args[i], "|") == 0)
            {
                ++i;
            }
            while (i < arg_count && strcmp(args[i], "|") != 0 && strcmp(args[i], "<") != 0 &&
                   strcmp(args[i], ">") != 0 && strcmp(args[i], ">>") != 0)
            {
                node->cmd[j] = (char *)malloc(BUFFER_SIZE);

                if (node->cmd[j] == NULL)
                {
                    fprintf(stderr, "Error allocating memory\n");
                    clear_commands();
                    close(saved_stdout);
                    close(saved_stdin);

                    for (int k = 0; k < node->cnt_args; ++k)
                    {
                        free(node->cmd[i]);
                    }
                    free(node);
                    return EXIT_FAILURE;
                }
                strcpy(node->cmd[j++], args[i++]);
                node->cnt_args = j;
            }
            node->cmd[j] = NULL;
            add_new_command(node);
        }
    }

    struct stat sb;

    if (has_input_redirection == 1 && strlen(infile) == 0)
    {
        fprintf(stderr, "No input file detected\n");
        close(saved_stdout);
        close(saved_stdin);
        free(infile);
        free(outfile);
        clear_commands();
        return EXIT_FAILURE;
    }

    if (strlen(infile) != 0 && stat(infile, &sb) != -1 && S_ISDIR(sb.st_mode))
    {
        fprintf(stderr, "Input file is a directory\n");
        close(saved_stdout);
        close(saved_stdin);
        free(infile);
        free(outfile);
        clear_commands();
        return EXIT_FAILURE;
    }

    if (has_output_redirection == 1 && strlen(outfile) == 0)
    {
        fprintf(stderr, "No output file detected\n");
        close(saved_stdout);
        close(saved_stdin);
        free(infile);
        free(outfile);
        clear_commands();
        return EXIT_FAILURE;
    }

    if (strlen(outfile) != 0 && stat(outfile, &sb) != -1 && S_ISDIR(sb.st_mode))
    {
        fprintf(stderr, "Output file is a directory\n");
        close(saved_stdout);
        close(saved_stdin);
        free(infile);
        free(outfile);
        clear_commands();
        return EXIT_FAILURE;
    }

    int fdin, fdout;

    if (strlen(infile) == 0)
    {
        fdin = dup(saved_stdin);
    }
    else
    {
        fdin = open(infile, O_RDONLY);
    }

    if (fdin == -1)
    {
        perror("dup() or open() error");
        clear_commands();
        close(saved_stdout);
        close(saved_stdin);
        free(infile);
        free(outfile);
        return EXIT_FAILURE;
    }

    while (head != NULL)
    {
        int rc = dup2(fdin, STDIN_FILENO);

        if (rc == -1)
        {
            perror("dup2() error");
            clear_commands();
            close(saved_stdout);
            close(saved_stdin);
            free(infile);
            free(outfile);
            return EXIT_FAILURE;
        }

        close(fdin);

        if (head->next == NULL)
        {
            if (strlen(outfile) == 0)
            {
                fdout = dup(saved_stdout);
            }
            else
            {
                if (is_append == 1)
                {

                    fdout = open(outfile, O_WRONLY | O_CREAT | O_APPEND, 0644);
                }
                else
                {
                    fdout = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                }
            }

            if (fdout == -1)
            {
                perror("dup2() or open() error");
                clear_commands();
                close(saved_stdout);
                close(saved_stdin);
                free(infile);
                free(outfile);
                return EXIT_FAILURE;
            }
        }
        else
        {
            int pipe_fd[2];
            pipe(pipe_fd);
            fdin = pipe_fd[0];
            fdout = pipe_fd[1];
        }

        rc = dup2(fdout, STDOUT_FILENO);

        if (rc == -1)
        {
            perror("dup2() error");
            clear_commands();
            close(saved_stdout);
            close(saved_stdin);
            free(infile);
            free(outfile);
            return EXIT_FAILURE;
        }

        close(fdout);

        int pid = fork();

        if (pid < 0)
        {
            perror("fork() error");
            clear_commands();
            close(saved_stdout);
            close(saved_stdin);
            free(infile);
            free(outfile);
            return EXIT_FAILURE;
        }
        else if (pid == 0)
        {
            setpgid(0, 0);
            int res = execvp(head->cmd[0], head->cmd);

            if (res == -1)
            {
                perror("execvp() error");
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            wait(NULL);
        }

        pop_command();
    }

    int rc = dup2(saved_stdin, STDIN_FILENO);

    if (rc == -1)
    {
        perror("dup2() error");
        clear_commands();
        close(saved_stdout);
        close(saved_stdin);
        free(infile);
        free(outfile);
        return EXIT_FAILURE;
    }

    rc = dup2(saved_stdout, STDOUT_FILENO);

    if (rc == -1)
    {
        perror("dup2() error");
        clear_commands();
        close(saved_stdout);
        close(saved_stdin);
        free(infile);
        free(outfile);
        return EXIT_FAILURE;
    }

    close(saved_stdout);
    close(saved_stdin);
    free(infile);
    free(outfile);

    return EXIT_SUCCESS;
}