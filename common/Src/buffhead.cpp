/*
 * buffhead.cpp
 *
 *  Created on: Apr 28, 2021
 *      Author: xrj
 */

#include "buffhead.hpp"
#include <cstring>
#include "buffer.hpp"
#include <algorithm>
#include "mylibs_config.hpp"

//Methods of BufferHead
BuffHead::BuffHead(Buffer *buff, u32 fid)
{
	this->buff = buff;
	this->fid = fid;
}

u32 BuffHead::increse(u32 i)
{
	u32 fi = fid;
	fid += i;
	return fi;
}

u32 BuffHead::bytes_elem()
{
	return buff->be;
}


//Methods of BufferHeadWrite
BuffHeadWrite::BuffHeadWrite(Buffer *buff, u32 fid):BuffHead(buff, fid)
{
	func = nullptr;
	put_timer.ctim = nullptr;
#ifdef USE_FREERTOS
	put_type = t_null;
#endif
}

//Methods of BufferHeadWrite
void BuffHeadWrite::Init()
{
#ifdef USE_FREERTOS
	osSemaphoreAttr_t attr_lock = {.name = "buff_lock"};
	this->lock = osSemaphoreNew(1, 1, &attr_lock);
#else
	this->lock = false;
#endif
}

void* BuffHeadWrite::lock_p()
{
#ifdef USE_FREERTOS
	if(osSemaphoreAcquire(lock, LOCK_TIMEOUT) != osOK){
		return nullptr;
	}
#else
	while(lock);
	lock = true;
#endif
	return (*buff)[fid];
}

/*
 * @note: update `fid` before unlock.
 */
void BuffHeadWrite::unlock()
{
#ifdef USE_FREERTOS
	osSemaphoreRelease(lock);
#else
	lock = false;
#endif
}

void BuffHeadWrite::wait_lock()
{
	lock_p();
	unlock();
}


//put single element
u32 BuffHeadWrite::put_elem(void* elem)
{
	void *p = lock_p();
	if(p){
		memcpy(p, elem, buff->be);
		fid++;
		unlock();
		return fid;
	}else{
		return ERR_FID;
	}
}

/*
 * put multiple elements, usually use DMA
 * @param N_elem: number of elem
 * @retval: pointer to put data, `nullptr` failed.
 */
void* BuffHeadWrite::put_dma_once(u32 N_elem)
{
	if(fid%(buff->capacity) + N_elem > buff->capacity){
		return nullptr;  //over tail of buffer
	}
	void* p=lock_p();
	if(p){
		N_remain = N_elem;
	}
	return p;
}

/*
 * reset pointer to p0, use DMA cycle write buffer, DMA length must same to buffer capacity
 */
void* BuffHeadWrite::put_dma_cycle(u32 cycle)
{
	if(lock_p()){
		fid = CEIL_DIV(fid, buff->be) * buff->be;  //reset to p0
		return buff->p0;
	}else{
		return nullptr;
	}

}

u32 BuffHeadWrite::get_capacity()
{
	return buff->get_capacity();
}

void BuffHeadWrite::put_dma_notify(u32 N_elem)
{
	fid += N_elem;
	N_remain -= N_elem;
	if(N_remain <= 0){
		unlock();
	}
#ifdef USE_FREERTOS
	BuffHeadReads &r_heads = buff->r_heads;
	u32 ef = ~osEventFlagsGet(r_heads.ef) & 0x00ffffff;
	u32 i = 0;
	while(ef){
		//no flag
		i += __builtin_ctz(ef);
		ef >>= __builtin_ctz(ef);
		if(fid >= r_heads[i]){
			osEventFlagsSet(r_heads.ef, 1<<i);
			i += 1;
			ef >>= 1;
		}
	}
#endif
}

void hwtim_callback(void* param)
{
	((BuffHeadWrite*)param)->put_bound();
}

void BuffHeadWrite::put_bound()
{
	func((*buff)[fid++]);
}

void BuffHeadWrite::put_hardware_timer(void (*func)(void*), u32 n, u32 us, C_TIM* ctim)
{
	if(!lock_p()){
		return;
	}
	this->func = func;
#ifdef USE_FREERTOS
	if(put_type == t_os2){
		osTimerDelete(put_timer.os2);
	}
	put_timer.ctim = ctim;
#else
	put_timer = etim;
#endif
	ctim->set_ns(us*1000);
	ctim->EnableIT(TIM_IT_update);
	ctim->set_callback(TIM_IT_update, hwtim_callback, this);
}

#ifdef USE_FREERTOS
void BuffHeadWrite::put_freertos_timer(void (*func)(void*), u32 n, u32 ms)
{
	if(put_type != t_os2){
		osTimerAttr_t attr = { .name = "buff_write_put" };
		put_timer.os2 = osTimerNew(&hwtim_callback, osTimerPeriodic, this, &attr);
	}
	osTimerStart(put_timer.os2, ms);
}
#endif

void BuffHeadWrite::put_timer_stop()
{
#ifdef USE_FREERTOS
	switch(put_type){
	case t_os2:
		osTimerStop(put_timer.os2);
		break;
	case t_etim:
		put_timer.ctim->DisableIT(TIM_IT_update);
		break;
	case t_null:
		return;
	}
	unlock();
#else
	if(put_type){
		put_timer->ctim->DisableIT(TIM_IT_update);
		unlock();
	}
#endif
}

BuffHeadReads::BuffHeadReads(Buffer *buff)
{
	u32 *h = heads;
	for(int i=0;i<24;i++){
		*h++ = 0;
	}
	this->buff = buff;
}

void BuffHeadReads::Init()
{
#ifdef USE_FREERTOS
	osEventFlagsAttr_t attr_event = {.name = "read_flags"};
	ef = osEventFlagsNew(&attr_event);
	osEventFlagsSet(ef, 0x00ffffff);
#endif
}

int BuffHeadReads::new_head()
{
	int i = __builtin_ctz(~enables);
	if(0<=i &&i<24){
		enables |= 1<<i;
		heads[i] = buff->w_head.fid;
	}else{
		i = -1;  //heads is full
	}
	return i;
}

void BuffHeadReads::disable_head(int head_id)
{
	if(0<=head_id &&head_id<24){
		enables &= ~(1<<head_id);
	}
}

u32 BuffHeadReads::operator[](u32 i)
{
	return heads[i];
}

void* BuffHeadReads::get_frames(u32 head_id, u32 n)
{
	u32 fid0 = heads[head_id];
	heads[head_id] += n;
	while(buff->last_fid() < heads[head_id]){ //avoid mistake set flag
#ifdef USE_FREERTOS
		u32 mask = 1<<head_id;
		osEventFlagsClear(ef, mask);
		osEventFlagsWait(ef, mask, 0x0, LOCK_TIMEOUT);
#endif
	}
	return (*buff)[fid0];
}

//single read head
BuffHeadRead::BuffHeadRead(BuffHeadReads &heads, u32 head_id):heads(heads)
{
	this->head_id = head_id;
}

void* BuffHeadRead::get_frames(u32 n)
{
	return heads.get_frames(head_id, n);
}

u32 BuffHeadRead::bytes_elem()
{
	return heads.buff->be;
}
