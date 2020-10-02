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
#include <sndfile.h>

typedef struct {
  SPFLOAT dur;
} sporth_smpldur_d;

int sporth_smpldur(sporth_stack *stack, void *ud)
{
    plumber_data *pd = ud;
    sporth_smpldur_d *smpldur;

    char *samplepath;

    switch (pd->mode) {
    case PLUMBER_CREATE:
#ifdef DEBUG_MODE
        plumber_print(pd, "smpldur: creating\n");
#endif
        smpldur = malloc(sizeof(sporth_smpldur_d));
        if (sporth_check_args(stack, "s") != SPORTH_OK) {
            plumber_print(pd,"Not enough arguments for smpldur\n");
            stack->error++;
            return PLUMBER_NOTOK;
        }

        samplepath = sporth_stack_pop_string(stack);
        SF_INFO info;
        memset(&info, 0, sizeof(SF_INFO));
        info.format = 0;
        SNDFILE *in = sf_open(samplepath, SFM_READ, &info);
        if (sf_error(in) != 0) {
          plumber_print(pd, "smpldur: Sndfile error reading <%s> with errorcode <%d>\n", samplepath, sf_error(in));
          stack->error++;
          return PLUMBER_NOTOK;
        } else {
          smpldur->dur = (
            (SPFLOAT) info.frames /
            (SPFLOAT) info.samplerate
          );
          sf_close(in);
        }

        plumber_add_ugen(pd, SPORTH_SMPLDUR, smpldur);
        sporth_stack_push_float(stack, smpldur->dur);
        break;
    case PLUMBER_INIT:
        smpldur = pd->last->ud;
        sporth_stack_pop_string(stack);
        sporth_stack_push_float(stack, smpldur->dur);
        break;
    case PLUMBER_COMPUTE:
        smpldur = pd->last->ud;
        sporth_stack_push_float(stack, smpldur->dur);
        break;
    case PLUMBER_DESTROY:
        break;
    default:
        plumber_print(pd, "Error: smpldur Unknown mode!\n");
        break;
    }
    return PLUMBER_OK;
}
