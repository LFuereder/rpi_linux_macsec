#if 0
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>

#include "../inc/cli_parsing.h"

static const struct option cli_options[] =
{
    { "verbose",                no_argument, 0, 'a' },
    { "help",                   no_argument, 0, 'h' },

    { "device",                 required_argument, 0, 'd' },

    {NULL, 0, NULL, 0}
};

static void print_help(char const *name);

/* Parse the provided argv array and store the information in the provided config variables.
 *
 * Returns 0 on success, +1 in case the help was printed and  -1 on failure (error is printed
 * on console).
 */
int parse_cli_arguments(bool* verbose, char** device, int argc, char **argv)
{
    /* Set default values */
    *verbose = "false";

    char* defaultDevice = "/dev/xdma0_h2c_0";
    memcpy(device, defaultDevice, sizeof(defaultDevice));



    /* Parse arguments */
    int index = 0;

    while (true)
    {
        int result = getopt_long(argc, argv, "vhd:", cli_options, &index);

        if (result == -1)
            break; /* end of list */

        switch (result)
        {
        
        case 'v':
            *verbose = true;
            break;

        case 'd':
            *device = optarg;
            break;

        case 'h':
            print_help(argv[0]);
            return 1;
            break;

        default:
            printf("unknown option: %c", result);
            print_help( argv[0]);
            return 1;
        }
    }

    return 0;
}

static void print_help(char const *name)
{
    printf("Usage: %s [OPTIONS]", name);
    
    printf("\nConnection configuration:\n");
    printf("  --network_interface name         name of the interface used for the GOOSE communication\n");
    printf("  --network_port value             network port used to establish connections to the sever (default = 102)\n");
    printf("  --host_IP value                  specifies the IP-address of the destination IED server\n");
    printf("  --destination_MAC                specifies the MAC-address for the incoming GOOSE messages\n");

    printf("\nOptions:\n");
    printf("  --verbose                        runtime option to provide additional details during execution\n");
    printf("  --mms_enable                     enables the publication of MMS messages in the IED\n");
    printf("  --mms_interval value             specifies the intervals in which the IED shall publish MMS messages\n");
    
    printf("\nEncryption Options:\n");
    printf("  --tls_enable                     enables TLS encryption for the MMS communication (the arguments blow have to be passed to the application)\n");
    printf("  --tls_client_root_cert value     specifies the path to the root certificat of the substation client implementation\n");
    printf("  --tls_client_cert value          specifies the path to the substation client certificat\n");
    printf("  --tls_client_key value           specifies the path th the substation client key\n");

    printf("\nMeasurement Options (both with and without TLS):\n");
    printf("  --measure_handshake_t            runtime option to measure time duration of the IEC handshake\n"); 
    printf("  --measure_modelparser_t          runtime option to measure time duration of the inital modelparsing\n"); 
    printf("  --measure_mms_t                  runtime option to measure time duration of a single MMS message\n"); 
    
    printf("\n");
    printf("  -h, --help                       display this help and exit\n");
}

#endif