
#include "args.h"

#include "protocol/method_exchange.h"
#include "system/log.h"
#include "options.h"

#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdio.h>
#include <errno.h>

#define MAX_CONNECTIONS_LIMIT 1024




struct option long_options[] = {
        {"ip4", 0, 0, '4'},
        {"ip6", 0, 0, '6'},
        {"port", 1, 0, 'p'},
        {"max-connections", 1, 0, 'c'},
        {"method", 1, 0, 'm'},
        {"help", 0, 0, 'h'},
};

void handle_args(int argc, char** argv)
{
    char name[strlen(argv[0]) + 1];
    strcpy(name, argv[0]);

    // Loop over options
    int opt_char = 0;
    do
    {
        opt_char = getopt_long(argc, argv, "46p:c:m:h", long_options, NULL);

        switch(opt_char)
        {
            case 'h':
            {
                usage(name);
                exit(0);
            }
            case '4':
            {
                set_opt(OPT_IP4);
                break;
            }
            case '6':
            {
                set_opt(OPT_IP6);
                break;
            }
            case 'p':
            {
                set_opt(OPT_PORT);

                long tmp = strtol(optarg, NULL, 10);
                if(0 < tmp || tmp < 65536 || errno == ERANGE)
                {
                    fprintf(stderr, "Error: incorrect value for -p/--port");
                    usage(name);

                    exit(-1);
                }

                PORT = (short)tmp;

                break;
            }
            case 'c':
            {
                set_opt(OPT_MAX_CONNECTIONS);

                MAX_CONNECTIONS = (int)strtol(optarg, NULL, 10);
                if(MAX_CONNECTIONS <= 0 ||  errno == ERANGE)
                {
                    fprintf(stderr, "Error: incorrect value for -c/--max-connections");
                    usage(name);

                    exit(-1);
                }

                break;
            }
            case 'm':
            {
                set_opt(OPT_METHOD);

                if(strcmp(optarg, "userpass") == 0)
                {
                    METHOD_PREFERED = METHOD_USERPASS;
                    break;
                }
                else if(strcmp(optarg, "noauth") == 0)
                {
                    METHOD_PREFERED = METHOD_NOAUTH;
                    break;
                }
                else
                {
                    fprintf(stderr,"Error: incorrect value for -m/--method");
                    usage(name);

                    exit(-1);
                }

                break;
            }
            case '?':
            {
                exit(-1);
            }
        }
    }
    while(opt_char != -1);

    if(!is_opt_set(OPT_IP4) && !is_opt_set(OPT_IP6))
        set_opt(OPT_IP4);
}


void usage(char* progName)
{
    printf("Usage: %s [ -4 | -6 {-v | -d} -u (-c=K) ]  \n"
           "Usage: %s -h \n\n"
           "-h --help               Print this usage guide \n"
           "-4 --ip4                Accept ipv4 clients [default]\n"
           "-6 --ip6                Accept ipv6 clients \n"
           "-p= --port=             Set server port manually. Range 1024-65535 [default: 1080]\n"
           "-c= --max-connections=  Connection limit for server  [default: 1024]\n"
           "-m= --method=           Authentication method used in SOCKS protocol [default: noauth]\n"
           "Valid method names:\n* noauth - no authentication\n* userpass - username and password authentication\n"
           "\n For\"userpass\" method there must be a valid user present in users.txt file."
           "\n One line - one user. Each line contains login and password, separated by whitespace. Character limit - 255"
           "Example:user password\n\n", progName, progName);
}

