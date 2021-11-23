#ifndef __TIMER_H
#define __TIMER_H

#ifdef __cplusplus
extern "C"
{
#endif
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////
	//		 TIMER.C		 //
	///////////////////////////
	

	void timer_phase(int hz);
	void do_timer();	
	void timer_handler(struct regs *r);
	void timer_wait(int ticks);
	void Timer_Install();
  

#ifdef __cplusplus
}
#endif

#endif

