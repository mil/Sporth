#include <stdio.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include "plumber.h"

#define PORT    57121
#define MAXLINE 1024

typedef struct {
    int sockfd;
} sporth_udp_d;

int sporth_udpr(sporth_stack *stack, void *ud)
{
    plumber_data *pd = ud;

    int n, len;
    char buffer[MAXLINE];
    char pad[MAXLINE];
    char state[MAXLINE];
    char *hello = "Hello from server";
    struct sockaddr_in servaddr, cliaddr;

    sporth_udp_d *udpr;

    SPFLOAT out;
    switch (pd->mode) {


    case PLUMBER_CREATE:
        udpr = malloc(sizeof(sporth_udp_d));
        if ((udpr->sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
            fprintf(stderr, "socket creation failed\n");
            exit(EXIT_FAILURE);
        }
        fcntl(
            udpr->sockfd,
            F_SETFL,
            fcntl(udpr->sockfd, F_GETFL) | O_NONBLOCK
        );

        memset(&servaddr, 0, sizeof(servaddr));
        memset(&cliaddr, 0, sizeof(cliaddr));
        servaddr.sin_family    = AF_INET;
        servaddr.sin_addr.s_addr = INADDR_ANY;
        servaddr.sin_port = htons(PORT);
        if (setsockopt(udpr->sockfd, SOL_SOCKET, SO_REUSEPORT, &(int) {
        1
    }, sizeof(int)) < 0) {
            fprintf(stderr, "setsockopt(SO_REUSEADDR) failed\n");
        }
        if (bind(udpr->sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0 ) {
            fprintf(stderr, "bind failed\n");
            exit(EXIT_FAILURE);
        }

        plumber_add_ugen(pd, SPORTH_UDPR, udpr);
        sporth_stack_push_float(stack, 0);
        break;


    case PLUMBER_INIT:
        udpr = pd->last->ud;
        /*udpr-> ldu = sporth_stack_pop_string(stack);*/
        sporth_stack_push_float(stack, 0);
        break;


    case PLUMBER_COMPUTE:
        udpr = pd->last->ud;
        n = recvfrom(
                udpr->sockfd,
                (char *)buffer,
                MAXLINE,
                MSG_WAITALL,
                (struct sockaddr *) &cliaddr,
                &len
            );
        /*buffer[n] = '\0';*/

        if (n == -1) {
            pd->p[15] = 0;
            /*fprintf(stderr, "Read Error: %d - %s\n", errno, strerror(errno)); */
        } else if (n > 0) {
            /* fprintf(stderr, "Buffer %d [%s]\n", n, buffer );*/
            if (strncmp("/pad", buffer, 4) == 0) {
                int i;
                for (i = 0; i < n; i++) {
                    if (buffer[i] == '\0') {
                        buffer[i] =  ' ';
                    }
                }

                char holder[50];
                sscanf(buffer, "/pad    ,s %s", &holder);
                pd->p[15] = oscPadActionToDigit(holder);
            }
        } else {
            pd->p[15] = 0;
        }
        break;

    case PLUMBER_DESTROY:
        udpr = pd->last->ud;
        free(udpr);
        break;


    default:
        plumber_print(pd, "Error: Unknown mode!");
        break;
    }

    return PLUMBER_OK;
}

