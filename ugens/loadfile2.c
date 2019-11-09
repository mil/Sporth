#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "plumber.h"

int sporth_loadfile2(sporth_stack *stack, void *ud)
{
#ifdef NO_LIBSNDFILE
    return PLUMBER_NOTOK;
#else
    plumber_data *pd = ud;

    sp_ftbl *ft;
    sp_ftbl *ft_left;
    sp_ftbl *ft_right;
    const char *left_str;
    const char *right_str;
    const char *filename;
    unsigned int i;

    switch(pd->mode){
        case PLUMBER_CREATE:
            plumber_add_ugen(pd, SPORTH_LOADFILE, NULL);
            if(sporth_check_args(stack, "sss") != SPORTH_OK) {
                plumber_print(pd, "Init: not enough arguments for loadfile2\n");
                return PLUMBER_NOTOK;
            }
            filename = sporth_stack_pop_string(stack);
            right_str = sporth_stack_pop_string(stack);
            left_str = sporth_stack_pop_string(stack);
            if(sp_ftbl_loadfile(pd->sp, &ft, filename) == SP_NOT_OK) {
                plumber_print(pd, "There was an issue creating loadfile2's temporary ftable\n");
                stack->error++;
                return PLUMBER_NOTOK;
            } else {
                sp_ftbl_create(pd->sp, &ft_left, ft->size / 2);
                sp_ftbl_create(pd->sp, &ft_right, ft->size / 2);
                for (i = 0; i < ft->size; i++) {
                    if (i % 2 == 0) {
                        ft_left->tbl[i / 2] = ft->tbl[i];
                    } else {
                        ft_right->tbl[i / 2] = ft->tbl[i];
                    }
                }
                sp_ftbl_destroy(&ft);
                plumber_ftmap_add(pd, left_str, ft_left);
                plumber_ftmap_add(pd, right_str, ft_right);
            }
            break;

        case PLUMBER_INIT:
            filename = sporth_stack_pop_string(stack);
            right_str = sporth_stack_pop_string(stack);
            left_str = sporth_stack_pop_string(stack);
            break;

        case PLUMBER_COMPUTE:
            break;

        case PLUMBER_DESTROY:
            break;

        default:
           printf("Error: Unknown mode!");
           break;
    }
    return PLUMBER_OK;
#endif
}
