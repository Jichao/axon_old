//Timer Event Manager

#ifndef _AX_TIMER_H_
#define _AX_TIMER_H_

#include "mem_pool.h"
#include "ax_hashmap.h"

namespace axon {

typedef void (*ev_timer_proc)(int handle, void* data);

//TimerWheel implementation
//Reference:
//  George Varghese , Tony Lauck. Hashed and Hierarchical Timing Wheels: Efficient Data Structures for Implementing a Timer Facility (1996)
//  example: linux kernel timer, timer module in ACE library

class EvTimer
{
public:
	struct TimerEntry {
		int handle;
		ev_timer_proc cbfunc;
		uint32_t tick;    //which tick to do timeout callback
		void* data;
		TimerEntry *next;
	};
	//max_time == SLOT_PER_WHEEL^TOTAL_WHEEL * tick_interval_
	//process muset restart when reach max_time
	//max_time nearly 3 years
	enum {
		SLOT_PER_WHEEL = 1024,
		TOTAL_WHEEL = 3,
	};

public:
	EvTimer(uint32_t t);
	~EvTimer();

public:
	int add_timer(ev_timer_proc cbfunc, void* data, uint32_t timeout);
	int del_timer(int handle);
	void timer_routine();		

private:
	int which_slot(uint32_t tick, uint32_t* slot);
	uint64_t get_timenow();
	void process_tick();

private:
	static const int kEntryPoolSize = 10000;   //timer entry pool maximum

	uint32_t ticknow_;
	uint32_t tick_interval_;  //the time tick unit

	//unit below: millisecond of gettimeofday
	uint64_t tm_now_;
	uint64_t tm_slap_;	
	int handle_;

	uint32_t slot_now_[TOTAL_WHEEL];
	HashMapInt* entry_map_;
	MemPool* entry_pool_;
	TimerEntry** wheel_[TOTAL_WHEEL];
};


} //namespace

#endif
