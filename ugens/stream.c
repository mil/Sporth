/*
 * Stream
 * A small notation for dispatching a stream of numbers at
 * specific times.
 *
 * "0:200,3:400,5:9000" => 0 at 200ms, 3 at 400ms, 5 at 9s
 */
#include "plumber.h"
#include <stdlib.h>
#include <strings.h>
#include <stdio.h>

typedef struct stream_event stream_event;
struct stream_event {
  float time;
  float value;

  stream_event * next;
};

typedef struct {
  stream_event * position;
  float sample_counter;
} sporth_stream_d;


stream_event * string_to_stream_list(char * input) {
  /*char input[] = "2:7,23:200,2:400";*/
  stream_event * head = NULL, * tail = NULL;
  char * token, *rest = input, *time, *value;

  while ((token = strtok_r(rest, ",", &rest)))  {
    value = strtok(token, ":");
    time = strtok(NULL, ":");
    if (tail == NULL) {
      tail = malloc(sizeof(stream_event));
      head = tail;
    } else {
      tail->next = malloc(sizeof(stream_event));
      tail = tail->next;
      tail->next = NULL;
    }
    tail->time = atof(time);
    tail->value = atof(value);
  }

  return head;
}

int sporth_stream(sporth_stack *stack, void *ud)
{
    plumber_data *pd = ud;
    const char *specstring;
    sporth_stream_d *stream;

    switch(pd->mode) {
        case PLUMBER_CREATE:
#ifdef DEBUG_MODE
            plumber_print(pd, "stream: Creating\n");
#endif
            if(sporth_check_args(stack, "s") != SPORTH_OK) {
                plumber_print(pd,"Not enough arguments for stream\n");
                stack->error++;
                return PLUMBER_NOTOK;
           }
            specstring = sporth_stack_pop_string(stack);
            stream = malloc(sizeof(sporth_stream_d));
            stream->sample_counter = 0;
            stream->position = string_to_stream_list(specstring);
            plumber_add_ugen(pd, SPORTH_STREAM, stream);
            sporth_stack_push_float(stack, 0);
            break;
        case PLUMBER_INIT:
#ifdef DEBUG_MODE
            plumber_print(pd, "stream: Initialising\n");
#endif

            stream = pd->last->ud;
            sporth_stack_push_float(stack, 0);
            break;
        case PLUMBER_COMPUTE:
            stream = pd->last->ud;

            if (
              stream->position != NULL &&
              stream->position->next != NULL && 
              stream->sample_counter > ((stream->position->next->time / 1000.0) * pd->sp->sr)
            ) {
              stream->position = stream->position->next;
            }
            sporth_stack_push_float(stack, stream->position->value);
            /*
            fprintf(stderr, "sr=%d counter=%f thistime=%f nt=%f \n",
               pd->sp->sr,
               stream->sample_counter,
               stream->position->time,
               stream->position->next->time
            );
            */

            /*sporth_stack_push_float(stack, 0);*/
            stream->sample_counter++;
            break;
        case PLUMBER_DESTROY:
            stream = pd->last->ud;
            break;
        default:
            plumber_print(pd, "stream: Unknown mode!\n");
            break;
    }
    return PLUMBER_OK;
}
