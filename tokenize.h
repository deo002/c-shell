#ifndef TOKENIZE_H
#define TOKENIZE_H

#include "commands.h"

char *trim_string(char *str);

commands_t *tokenize_commands(char *cmds);

#endif // TOKENIZE