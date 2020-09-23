#ifndef grain_scheduler_h
#define grain_scheduler_h

#include <stdio.h>
#include <m_pd.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct grain_scheduler
{
    t_word      *sample;
    t_int       *grains;
    int         sample_length,
                offset,
                num_grains,
                grain_length;
    
}grain_scheduler;

grain_scheduler *grain_scheduler_new(t_word *sample, int sample_length);

void grain_scheduler_free(grain_scheduler *x);

void grain_scheduler_set_props(grain_scheduler *x, int offset, int num_grains, int grain_length);

void grain_scheduler_perform(grain_scheduler *x, int sample_pos, t_sample *out);

#ifdef __cplusplus
}
#endif

#endif /* grain_scheduler_h */
