#ifndef __COMP3400_utils_h__
#define __COMP3400_utils_h__

#define BUFFER_SIZE 1000

int setup_server (const char *);
char *get_connection (int, int *);
char *build_response (char *, char *, char **);

#endif
