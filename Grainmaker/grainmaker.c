//
//  grainmaker.c
//  Grainmaker
//
//  Created by Peter K. Gorzo on 27.08.20.
//  Copyright © 2020 Realtime Audio. All rights reserved.
//

#include "m_pd.h"
#include "stdlib.h"

static t_class *grainmaker_tilde_class;

typedef struct _grainmaker_tilde {
    t_object    x_obj;
    t_word      *x_vec;
    int         x_npoints;
    t_symbol    *x_arrayname;
    t_float     f;
    
    t_int       offset,
                num_grains,
                grain_length;
    
    t_inlet     *in_offset,
                *in_num_grains,
                *in_grain_length;
    
    t_outlet    *out;
    
//    t_int       offset, grain_length;
//    t_float     head_pos, playback_speed;
//    // sample, grain_envelope, (grain_start), direction
//    t_array     *sample;
//    t_inlet     *in_sample, *in_offset, *in_grain_length, *in_head_pos;
//    t_outlet    *out_A, *out_B, *out_synch, *out_count;
    
}t_grainmaker_tilde;

void *grainmaker_tilde_new(t_symbol *s) {
    t_grainmaker_tilde *x = (t_grainmaker_tilde *)pd_new(grainmaker_tilde_class);
    
    x->x_arrayname = s;
    x->x_vec = 0;
    x->f = 0;
    
    
    x->in_offset = inlet_new(&x->x_obj,  &x->x_obj.ob_pd, &s_float, gensym("offset"));
    x->in_num_grains = inlet_new(&x->x_obj,  &x->x_obj.ob_pd, &s_float, gensym("num_grains"));
    x->in_grain_length = inlet_new(&x->x_obj,  &x->x_obj.ob_pd, &s_float, gensym("grain_length"));
    
    x->out = outlet_new(&x->x_obj, &s_signal);
    
    return (void *)x;
}

void grainmaker_tilde_free(t_grainmaker_tilde *x) {
    inlet_free(x->in_offset);
    inlet_free(x->in_num_grains);
    inlet_free(x->in_grain_length);
    outlet_free(x->out);
}

static t_int *grainmaker_tilde_perform(t_int *w)
{
    t_grainmaker_tilde *x = (t_grainmaker_tilde *)(w[1]);
    t_sample *in = (t_sample *)(w[2]);
    t_sample *out = (t_sample *)(w[3]);
    int n = (int)(w[4]);
    int maxindex;
    t_word *buf = x->x_vec;
    int i;
    
    
    // Print all current values of inlets
    int offset = (int)x->offset;
    int grain_length = (int)x->grain_length;
    int num_grains = (int)x->num_grains;


    int length = snprintf( NULL, 0, "%d", offset );
    char* str1 = malloc( length + 1 );
    snprintf( str1, length + 1, "%d", offset );
    post("offset: ");
    post(str1);
    free(str1);
    
    length = snprintf( NULL, 0, "%d", num_grains );
    char* str = malloc( length + 1 );
    snprintf( str, length + 1, "%d", num_grains );
    post("num_grains: ");
    post(str);
    free(str);
    
    length = snprintf( NULL, 0, "%d", grain_length );
    char* str2 = malloc( length + 1 );
    snprintf( str2, length + 1, "%d", grain_length );
    post("grain_length: ");
    post(str2);
    free(str2);

    
    maxindex = x->x_npoints - 1;
    if(maxindex<0) goto zero;
    if (!buf) goto zero;

    for (i = 0; i < n; i++)
    {
        int index = *in++;
        if (index < 0)
            index = 0;
        else if (index > maxindex)
            index = maxindex;
        *out++ = buf[index].w_float;
    }
    return (w+5);
 zero:
    while (n--) *out++ = 0;

    return (w+5);
}

static void grainmaker_tilde_set(t_grainmaker_tilde *x, t_symbol *s)
{
    // Copied from tabread~ external
    t_garray *a;
    x->x_arrayname = s;
    
    if (!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
    {
        if (*s->s_name)
            pd_error(x, "grainmaker~: %s: no such array", x->x_arrayname->s_name);
        x->x_vec = 0;
    }
    else if (!garray_getfloatwords(a, &x->x_npoints, &x->x_vec))
    {
        pd_error(x, "%s: bad template for grainmaker~", x->x_arrayname->s_name);
        x->x_vec = 0;
    }
    else garray_usedindsp(a);
}

static void grainmaker_tilde_dsp(t_grainmaker_tilde *x, t_signal **sp)
{
    grainmaker_tilde_set(x, x->x_arrayname);
    dsp_add(grainmaker_tilde_perform, 4, x,
            sp[0]->s_vec,
            sp[1]->s_vec,
            sp[0]->s_n);
}

static void grainmaker_tilde_set_offset(t_grainmaker_tilde *x, t_floatarg f) {
    x->offset = f;
}

static void grainmaker_tilde_set_num_grains(t_grainmaker_tilde *x, t_floatarg f) {
    x->num_grains = f;
}

static void grainmaker_tilde_set_grain_length(t_grainmaker_tilde *x, t_floatarg f) {
    x->grain_length = f;
}

void grainmaker_tilde_setup(void) {
    grainmaker_tilde_class = class_new(gensym("grainmaker~"),
                                 (t_newmethod)grainmaker_tilde_new,
                                 (t_method)grainmaker_tilde_free,
                                 sizeof(t_grainmaker_tilde),
                                 0,
                                 A_DEFSYM,
                                 0);
    CLASS_MAINSIGNALIN(grainmaker_tilde_class, t_grainmaker_tilde, f);
    class_addmethod(grainmaker_tilde_class, (t_method)grainmaker_tilde_dsp,
        gensym("dsp"), A_CANT, 0);

    class_addmethod(grainmaker_tilde_class,
                    (t_method)grainmaker_tilde_set_offset,
                    gensym("offset"),
                    A_DEFFLOAT,
                    0);
    
    class_addmethod(grainmaker_tilde_class,
                    (t_method)grainmaker_tilde_set_num_grains,
                    gensym("num_grains"),
                    A_DEFFLOAT,
                    0);
    
    class_addmethod(grainmaker_tilde_class,
                    (t_method)grainmaker_tilde_set_grain_length,
                    gensym("grain_length"),
                    A_DEFFLOAT,
                    0);
}
