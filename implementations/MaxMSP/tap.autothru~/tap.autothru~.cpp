// MSP External: tap.auto_thru~.c
// A thru object that switches to the 2nd inlet if it is connected

#include "TTClassWrapperMax.h"


typedef struct _thru				// Data Structure for this object
{
	t_pxobject	x_obj;
} t_thru;

// Prototypes for methods: need a method for each incoming message type
void *thru_new(t_symbol *name, long argc, t_atom* argv);
void thru_perform64(t_thru *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long sampleframes, long flags, void *userparam);
void thru_perform264(t_thru *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long sampleframes, long flags, void *userparam);
void thru_dsp64(t_thru *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);
void thru_assist(t_thru *x, void *b, long m, long a, char *s);	// Assistance Method


static t_class *autothru_class;
static t_class *auto_thru_class;


/************************************************************************************/
// Main() Function

extern "C" int TTCLASSWRAPPERMAX_EXPORT main(void)
{
	t_class *c1;
	t_class *c2;

	// We changed the name to tap.autothru~ in TT3, but are keeping the old name for backward compatibility
	c1 = class_new("tap.autothru~",(method)thru_new, (method)dsp_free, sizeof(t_thru), (method)0L, A_GIMME, 0);
	c2 = class_new("tap.auto_thru~",(method)thru_new, (method)dsp_free, sizeof(t_thru), (method)0L, A_GIMME, 0);

	common_symbols_init();

	class_addmethod(c1, (method)thru_dsp64,	"dsp64", A_CANT, 0);
	class_addmethod(c2, (method)thru_dsp64,	"dsp64", A_CANT, 0);
	class_addmethod(c1, (method)thru_assist, "assist", A_CANT, 0L); 
	class_addmethod(c2, (method)thru_assist, "assist", A_CANT, 0L); 
	class_addmethod(c1, (method)stdinletinfo,	"inletinfo",	A_CANT, 0);
	class_addmethod(c2, (method)stdinletinfo,	"inletinfo",	A_CANT, 0);

	class_dspinit(c1);									// Setup object's class to work with MSP
	class_dspinit(c2);									// Setup object's class to work with MSP
	autothru_class = c1; class_register(_sym_nobox, c1);
	auto_thru_class = c2; class_register(_sym_nobox, c2);
}


/************************************************************************************/
// Object Creation Method

void *thru_new(t_symbol *name, long argc, t_atom* argv)
{
	t_thru *x = NULL;

	if(name == gensym("tap.autothru~"))
		x = (t_thru *)object_alloc(autothru_class);
	else
		x = (t_thru *)object_alloc(auto_thru_class);

	if(x){
    	object_obex_store((void *)x, _sym_dumpout, (object *)outlet_new(x,NULL));	// dumpout		
		dsp_setup((t_pxobject *)x,2);				// Create Object and 2 Inlet (last argument)
		outlet_new((t_pxobject *)x, "signal");		// Create a signal Outlet
	}	
	return (x);										// Return the pointer
}


/************************************************************************************/
// Methods bound to input/inlets

// Method for Assistance Messages
void thru_assist(t_thru *x, void *b, long msg, long arg, char *dst)
{
	if(msg==1){ 	// Inlets
		switch(arg){
			case 0: strcpy(dst, "(signal) Will go thru if inlet 2 is not connected"); break;
			case 1: strcpy(dst, "(signal) will cut-off inlet 1 if connected"); break;
		}
	}
	else if(msg==2) // Outlets
		strcpy(dst, "(signal) signal 2 if connected, else signal 1");
}


void thru_perform64(t_thru *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long sampleframes, long flags, void *userparam)
{
	double *in = ins[0];
	double *out = outs[0];
	int		n = sampleframes;
	
	memcpy(out, in, sizeof(double)*n);
	return;
}


void thru_perform264(t_thru *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long sampleframes, long flags, void *userparam)
{
	double *in = ins[1];
	double *out = outs[0];
	int		n = sampleframes;
	
	memcpy(out, in, sizeof(double)*n);
	return;
}


void thru_dsp64(t_thru *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
{
	if (count[1])
		object_method(dsp64, gensym("dsp_add64"), (t_object*)x, thru_perform264, 0, NULL);
	else
		object_method(dsp64, gensym("dsp_add64"), (t_object*)x, thru_perform64, 0, NULL);
}
