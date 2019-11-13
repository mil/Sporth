#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "plumber.h"

int sporth_add2(sporth_stack *stack, void *ud)
{
    if(stack->error > 0) return PLUMBER_NOTOK;

    plumber_data *pd = ud;
    SPFLOAT l1, r1, l2, r2;
    switch(pd->mode){
        case PLUMBER_CREATE:
            plumber_add_ugen(pd, SPORTH_ADD2, NULL);
            if(sporth_check_args(stack, "ffff") != SPORTH_OK) {
                stack->error++;
                return PLUMBER_NOTOK;
            }
            l1 = sporth_stack_pop_float(stack);
            r1 = sporth_stack_pop_float(stack);
            l2 = sporth_stack_pop_float(stack);
            r2 = sporth_stack_pop_float(stack);
            sporth_stack_push_float(stack, 0);
            sporth_stack_push_float(stack, 0);
            break;
        case PLUMBER_INIT:
            l1 = sporth_stack_pop_float(stack);
            r1 = sporth_stack_pop_float(stack);
            l2 = sporth_stack_pop_float(stack);
            r2 = sporth_stack_pop_float(stack);
            sporth_stack_push_float(stack, r1 + r2);
            sporth_stack_push_float(stack, l1 + l2);
            break;
        case PLUMBER_COMPUTE:
            l1 = sporth_stack_pop_float(stack);
            r1 = sporth_stack_pop_float(stack);
            l2 = sporth_stack_pop_float(stack);
            r2 = sporth_stack_pop_float(stack);
            sporth_stack_push_float(stack, r1 + r2);
            sporth_stack_push_float(stack, l1 + l2);
            break;
        case PLUMBER_DESTROY:
            break;
        default:
          plumber_print(pd,"Error: Unknown mode!");
           stack->error++;
           return PLUMBER_NOTOK;
           break;
    }
    return PLUMBER_OK;
}
