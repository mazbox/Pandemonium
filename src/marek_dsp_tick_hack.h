/*
 *  marek_dsp_tick_hack.h
 *  emptyExample
 *
 *  Created by Marek Bereza on 19/12/2010.
 *  Copyright 2010 Marek Bereza. All rights reserved.
 *
 */

#ifdef marek_dsp_tick_hack_IMPLEMENTATION
t_int **get_dsp_chain() {
	return &dsp_chain;
}
int get_dsp_chainsize() {
	return dsp_chainsize;
}
void set_dsp_chainsize(int s) {
	dsp_chainsize = s;
}
#else
t_int **get_dsp_chain();
void set_dsp_chainsize(int s);
int get_dsp_chainsize();
#endif