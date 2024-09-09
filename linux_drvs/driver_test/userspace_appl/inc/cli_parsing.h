#if 0
#ifndef CLI_PARSING_H
#define CLI_PARSING_H

#include <stdint.h>
#include <stdlib.h>

enum application_role
{
    NOT_SET,
    ROLE_REVERSE_PROXY,
    ROLE_FORWARD_PROXY,
    ROLE_ECHO_SERVER,
    ROLE_ECHO_CLIENT,
};

enum time_measure_target
{
    NO_TARGET,
    HANDSHAKE_MEASURE,
    MODELPARSE_MEASURE,
    MMS_MEASURE,
};


/* Parse the provided argv array and store the information in the provided config variables. 
 * 
 * Returns 0 on success, +1 in case the help was printed and -1 on failure (error is printed
 * on console).
 */
int parse_cli_arguments(bool* verbose, char** device, int argc, char **argv);


#endif // CLI_PARSING_H
#endif