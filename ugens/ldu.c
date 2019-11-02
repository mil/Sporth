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

typedef struct {
    int numdown;
    int numup;
    char * str;
    char * strdown;
    char * strup;
} sporth_ldu_d;

int oscPadActionToDigit(char *s)
{
    char row = ' ';
    int col;
    int bank = 1;
    char state[10];

    if (strchr(s, '@') == NULL) {
        sscanf(s, "%c%d,%s", &row, &col, state);
    } else {
        sscanf(s, "%c%d@%d,%s", &row, &col, &bank, state);
    }

    int row_i;
    if (row == 'a') {
        row_i = 1;
    }
    if (row == 'b') {
        row_i = 2;
    }
    if (row == 'c') {
        row_i = 3;
    }
    if (row == 'd') {
        row_i = 4;
    }

    int state_down = strcmp(state, "down") == 0 ? 2 : 1;

    int dig = (
                  (col        * 1) +
                  (row_i      * 10) +
                  (bank       * 100) +
                  (state_down * 10000)
              );

    return dig;
}

int sporth_ldu(sporth_stack *stack, void *ud)
{
    plumber_data *pd = ud;
    SPFLOAT out;

    sporth_ldu_d *ldu;

    switch (pd->mode) {
    case PLUMBER_CREATE:
        ldu = malloc(sizeof(sporth_ldu_d));
        ldu->str = malloc(10);
        ldu->strdown = malloc(10);
        ldu->strup = malloc(10);

        ldu->str = sporth_stack_pop_string(stack);
        strcpy(ldu->strdown, ldu->str);
        ldu->strdown = strcat(ldu->strdown, ",down");
        strcpy(ldu->strup, ldu->str);
        ldu->strup = strcat(ldu->strup, ",up");
        ldu->numdown = oscPadActionToDigit(ldu->strdown);
        ldu->numup = oscPadActionToDigit(ldu->strup);

        plumber_add_ugen(pd, SPORTH_LDU, ldu);
        sporth_stack_push_float(stack, 0);
        break;
    case PLUMBER_INIT:
        ldu = pd->last->ud;
        ldu->str = sporth_stack_pop_string(stack);
        sporth_stack_push_float(stack, 0);
        break;
    case PLUMBER_COMPUTE:
        ldu = pd->last->ud;
        if ((int) pd->p[15] == ldu->numdown) {
            fprintf(stderr, "(DOWN!) Computing %s and %f\n", ldu->str, pd->p[15]);
            out = 2;
        } else if ((int) pd->p[15] == ldu->numup) {
            fprintf(stderr, "(UP!) Computing %s and %f\n", ldu->str, pd->p[15]);
            out = 1;
        } else {
            out = 0;
        }
        sporth_stack_push_float(stack, out);
        break;
    case PLUMBER_DESTROY:
        ldu = pd->last->ud;
        free(ldu);
        break;
    default:
        plumber_print(pd, "Error: Unknown mode!");
        break;
    }
    return PLUMBER_OK;
}
