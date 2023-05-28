#include "thirdpart.h"

int run_php(const char *script_path, char **output) {
    char *command = "php";
    char *script_arg = script_path;
    char *pipe_cmd = "/bin/bash -c 'popen(\"%s %s\",\"r\")'";

    FILE *pipe = popen(pipe_cmd, "r");
    if (!pipe) {
        perror("popen");
        return 1;
    }

    char *input = command;
    while (*script_arg) {
        if (*script_arg == '\"') {
            script_arg++;
            fputs(input, pipe);
            input = script_arg;
        } else {
            fputc(*script_arg, pipe);
            script_arg++;
        }
    }
    fputs(input, pipe);

    int status;
    if (pclose(pipe) == -1) {
        perror("pclose");
        return 1;
    }

    *output = NULL;
    if (output) {
        while (fgets(*output, 4096, pipe) != NULL) {
        }
    }

    return 0;
}




// #include <stdio.h>
// #include <stdlib.h>
// #include <fcgi_stdio.h>

// int fastcgi_handler(struct FastCGI_Handler *handler) {
//     char *query;
//     int query_len;
//     char *content;

//     while (FCGI_Accept(handler) >= 0) {
//         query = FCGI_GetParameter(handler, "QUERY_STRING", 0, &query_len);
//         content = "Hello, World!";

//         FCGI_SetHeader(handler, "Content-type", "text/html");
//         FCGI_Output(handler, (char *)content, strlen(content));
//         FCGI_SetHeader(handler, "Content-Length", query_len + strlen(content));
//     }

//     return 0;
// }