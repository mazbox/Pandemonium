/*
 *  marek_sched_tick_hack.h
 *  emptyExample
 *
 *  Created by Marek Bereza on 19/12/2010.
 *  Copyright 2010 Marek Bereza. All rights reserved.
 *
 */

#ifdef marek_sched_tick_hack_IMPLEMENTATION

int get_sched_diddsp() {
	return sched_diddsp;
}

void set_sched_diddsp(int sched_didsp) {
	sched_diddsp = sched_didsp;
}

#else
void set_sched_diddsp(int sched_didsp) ;
int get_sched_diddsp();

#endif