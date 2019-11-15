#include <math.h>
#include "plumber.h"

int sporth_mutetrigger(sporth_stack *stack, void *ud)
{
    plumber_data *pd = ud;
    int p_register;
    float signal;

    switch(pd->mode) {
        case PLUMBER_CREATE:

#ifdef DEBUG_MODE
            plumber_print(pd, "mutetrigger: creating\n");
#endif
            plumber_add_ugen(pd, SPORTH_MUTETRIGGER, NULL);

            if (sporth_check_args(stack, "ff") != SPORTH_OK) {
                plumber_print(pd,"Not enough arguments for mutetrigger\n");
                stack->error++;
                return PLUMBER_NOTOK;
            }

            p_register = (int) sporth_stack_pop_float(stack);
            signal = sporth_stack_pop_float(stack);
            pd->p[p_register] = 0;
            sporth_stack_push_float(stack, 0);
            break;
        case PLUMBER_INIT:
        case PLUMBER_COMPUTE:
            p_register = (int) sporth_stack_pop_float(stack);
            signal = sporth_stack_pop_float(stack);
            pd->p[p_register] = signal;
            sporth_stack_push_float(stack, signal);
            break;
        case PLUMBER_DESTROY:
            break;
        default:
            plumber_print(pd, "mutetrigger: unknown mode!\n");
            break;
    }
    return PLUMBER_OK;
}

int sporth_muteable(sporth_stack *stack, void *ud)
{
    plumber_data *pd = ud;
    int p_register;
    float signal;

    switch(pd->mode) {
        case PLUMBER_CREATE:

#ifdef DEBUG_MODE
            plumber_print(pd, "muteable: creating\n");
#endif
            plumber_add_ugen(pd, SPORTH_MUTEABLE, NULL);

            if (sporth_check_args(stack, "ff") != SPORTH_OK) {
                plumber_print(pd,"Not enough arguments for muteable\n");
                stack->error++;
                return PLUMBER_NOTOK;
            }
            p_register = (int) sporth_stack_pop_float(stack);
            signal = sporth_stack_pop_float(stack);
            sporth_stack_push_float(stack, signal);
            break;
        case PLUMBER_INIT:
        case PLUMBER_COMPUTE:
            p_register = (int) sporth_stack_pop_float(stack);
            signal = sporth_stack_pop_float(stack);
            sporth_stack_push_float(stack, pd->p[p_register] == 0 ? signal : 0);
            break;
        case PLUMBER_DESTROY:
            break;
        default:
            plumber_print(pd, "muteable: unknown mode!\n");
            break;
    }
    return PLUMBER_OK;
}