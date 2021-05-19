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
#include "FreeRTOS.h"

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

void BuffHeadWrite::Init()
{
	osSemaphoreAttr_t attr_lock = {.name = "buff_lock"};
	this->lock = osSemaphoreNew(1, 1, &attr_lock);
}

u32 BuffHeadWrite::bytes_elem()
{
	return buff->be;
}

//put single element
u32 BuffHeadWrite::put_elem(void* elem)
{
	if(osSemaphoreAcquire(lock, LOCK_TIMEOUT) == osOK){
		memcpy((*buff)[fid], elem, buff->be);
		osSemaphoreRelease(lock);
		return increse(1);
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
	}if(osSemaphoreAcquire(lock, LOCK_TIMEOUT) == osOK){
		N_remain = N_elem;
		return (*buff)[fid];
	}else{
		return nullptr;
	}
}

/*
 * reset pointer to p0, use DMA cycle write buffer, DMA length must same to buffer capacity
 */
void* BuffHeadWrite::put_dma_cycle(u32 cycle)
{
	if(osSemaphoreAcquire(lock, LOCK_TIMEOUT) == osOK){
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
		osSemaphoreRelease(lock);
	}
	BuffHeadReads &r_heads = buff->r_heads;
	u32 ef = osEventFlagsGet(r_heads.ef);
	for(int i=0;i<24;i++){
		//no flag
		if(!(ef&0x1) && fid > r_heads[i]){
			osEventFlagsSet(r_heads.ef, 1<<i);
		}
		ef >>= 1;
	}
}

void BuffHeadWrite::wait_lock()
{
	if(osSemaphoreAcquire(lock, LOCK_TIMEOUT) == osOK){
		osSemaphoreRelease(lock);
	}
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
	osEventFlagsAttr_t attr_event = {.name = "read_flags"};
	ef = osEventFlagsNew(&attr_event);
	osEventFlagsSet(ef, 0x00ffffff);
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
	while(buff->last_fid() <= heads[head_id]){ //avoid mistake set flag
		u32 mask = 1<<head_id;
		osEventFlagsClear(ef, mask);
		osEventFlagsWait(ef, mask, 0x0, LOCK_TIMEOUT);
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