//Timer Event

#include "stdheader.h"
#include "ax_debug.h"
#include "utest.h"
#include "ax_timer.h"
#include <sys/time.h>

namespace axon {

EvTimer::EvTimer(uint32_t t)
{
	entry_map_ = new HashMapInt(SLOT_PER_WHEEL * TOTAL_WHEEL);
	entry_pool_ = new MemPool(kEntryPoolSize, sizeof(TimerEntry));

	for(int i=0; i<TOTAL_WHEEL; i++) {
		wheel_[i] = (TimerEntry**)malloc(SLOT_PER_WHEEL * sizeof(TimerEntry**));
		memset(wheel_[i], 0, SLOT_PER_WHEEL * sizeof(TimerEntry*));
	}

	tick_interval_ = t;
	ticknow_ = 0;
	tm_now_ = get_timenow();
	tm_start_ = tm_last_ = tm_slap_ = 0;
	handle_ = 0;
}

EvTimer::~EvTimer()
{
	delete entry_map_;
	delete entry_pool_;
	for(int i=0; i<TOTAL_WHEEL; i++) {
		free(wheel_[i]);
	}
}

uint64_t EvTimer::get_timenow()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000 + (tv.tv_usec / 1000);
}

uint32_t EvTimer::get_timepass()
{
	tm_now_ = get_timenow();
	if (tm_start_ == 0) tm_start_ = tm_now_;
	return (uint32_t)(tm_now_ - tm_start_);
}

//return: which slot and wheel
int EvTimer::which_slot(uint32_t tick, uint32_t* slot)
{
	int i;
	for(i=0; i<TOTAL_WHEEL; i++) {
		slot[i] = tick % SLOT_PER_WHEEL;
		tick /= SLOT_PER_WHEEL;
	}
	RT_ASSERT(tick < 1);   //timer upper limit

	for(i=TOTAL_WHEEL-1; i>=0; i--) {
		if (slot[i] > slot_now_[i]) return i;
	}
	return 0;
}

int EvTimer::add_timer(ev_timer_proc cbfunc, void* data, uint32_t timeout)
{
	int wheel;
	uint32_t slot[TOTAL_WHEEL];
	TimerEntry *pe = (TimerEntry*)entry_pool_->alloc();
	RT_ASSERT(pe != NULL);
	pe->cbfunc = cbfunc;
	pe->data = data;
	++handle_;
	if (handle_ <= 0) handle_ = 1;
	pe->handle = handle_;
	if (timeout > tick_interval_) {
		pe->tick = ticknow_ + timeout / tick_interval_;
	} else {
		pe->tick = ticknow_ + 1;
	}

	entry_map_->insert(pe->handle, pe);

	wheel = which_slot(pe->tick, slot);
	pe->next = wheel_[wheel][slot[wheel]];
	wheel_[wheel][slot[wheel]] = pe;
	return pe->handle;
}

int EvTimer::del_timer(int handle)
{
	uint32_t slot[TOTAL_WHEEL];
	int	wheel;
	TimerEntry *pe = (TimerEntry*)entry_map_->remove_get(handle);
	TimerEntry *pw, *qw;
	if (NULL == pe) return -1;

	wheel = which_slot(pe->tick, slot);
	pw = wheel_[wheel][slot[wheel]];
	if (NULL == pw) return -1;
	if (pw->handle == handle) {
		wheel_[wheel][slot[wheel]] = pw->next;
		entry_pool_->free(pe);
		return 0;
	}
	
	qw = pw->next;
	while (qw != NULL) {
		if (qw->handle == handle) {
			pw->next = qw->next;
			entry_pool_->free(pe);
			return 0;
		}
		pw = qw;
		qw = pw->next;
	}

	return -1;
}

void EvTimer::timer_routine()
{
	tm_last_ = tm_now_;
	tm_now_ = get_timenow();	
	if (tm_start_ == 0) tm_start_ = tm_now_;
	if (tm_now_ < tm_slap_ || tm_now_ > tm_slap_ + 1000*60)
	{
		tm_slap_ = tm_now_;
	}
	while (tm_now_ > tm_slap_) {
		tm_slap_ += tick_interval_;
		process_tick();
	}
}

void EvTimer::process_tick()
{
	int slot;
	uint32_t target_slot[TOTAL_WHEEL];
	int target_wheel;
	uint32_t tslot;
	TimerEntry *p, *q, *head;

	++ticknow_;
	++slot_now_[0];
	//update wheel
	for(int i=0; i<TOTAL_WHEEL-1; i++) {
		if (slot_now_[i] < SLOT_PER_WHEEL) continue;

		slot_now_[i] = 0;
		++slot_now_[i+1];
		slot = slot_now_[i+1];
		//move wheel
		p = wheel_[i+1][slot];	
		while (p != NULL) {
			q = p->next;
			target_wheel = which_slot(p->tick, target_slot);
			tslot = target_slot[target_wheel];
			head = wheel_[target_wheel][tslot];
			p->next = head;
			wheel_[target_wheel][tslot] = p;
			p = q;
		}
		wheel_[i+1][slot] = NULL;
	}	

	//run current slot
	head = wheel_[0][slot_now_[0]];	
	if (head == NULL) return;
	p = head;
	while (p != NULL) {
		q = p->next;
		if (p->cbfunc != NULL) p->cbfunc(p->handle, p->data);	
		p = q;
	}
	
	//delete all entry (caller should add_timer again for routine callback)
	p = head;
	while (p != NULL) {
		q = p->next;
		entry_map_->remove_get(p->handle);		
		entry_pool_->free(p);
		p = q;
	}	
	wheel_[0][slot_now_[0]] = NULL;
}



//Unit test===================================

UTEST(EvTimer)
{
	EvTimer timer(100);

}

} //namespace
