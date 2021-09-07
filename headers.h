#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <errno.h>
#include <ctype.h>
#include <sys/wait.h>
#include <signal.h>
#include <limits.h>
#include <time.h>
#include <sys/select.h>
#include <termios.h>

#define BUFFER_SIZE 150

char PATH_TO_HISTORY_FILE[BUFFER_SIZE];