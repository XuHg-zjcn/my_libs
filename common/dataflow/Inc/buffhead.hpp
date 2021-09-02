/*
 * buffhead.hpp
 *
 *  Created on: Apr 28, 2021
 *      Author: xrj
 */

#ifndef DATAFLOW_INC_BUFFHEAD_HPP_
#define DATAFLOW_INC_BUFFHEAD_HPP_

#include "myints.h"
#include "mylibs_config.hpp"
#include <vector>
#include "c_tim.hpp"

using namespace std;

#define ERR_FID      0xffffffff
#define LOCK_TIMEOUT 10000

class Buffer;

class BuffHead{
protected:
	Buffer *buff;
	u32 increse(u32 i);
public:
	u32 fid;
	BuffHead(Buffer *buff, u32 fid);
	u32 bytes_elem();
	friend Buffer;
};

class BuffHeadWrite : public BuffHead{
private:
	int32_t N_remain;
	void (*func)(void*);
#ifdef USE_FREERTOS
	osSemaphoreId_t lock;  //lock for putting data
	union {osTimerId_t os2; C_TIM* ctim;}put_timer;
	enum {t_null=0,t_os2,t_etim}put_type;
#else
	bool lock;  //true: locking
	C_TIM* put_timer;
#endif
public:
	BuffHeadWrite(Buffer *buff, u32 fid);
	void Init();
	void* lock_p();
	void unlock();
	void wait_lock();  //rename to `wait_unlock()`
	u32 put_elem(void* elem);
	void* put_dma_once(u32 N_elem);
	void* put_dma_cycle(u32 cycle);
	u32 get_capacity();
	void put_dma_notify(u32 N_elem);
	u32 put_dma_finish();
	//bound put
	void put_bound();
	void put_hardware_timer(void (*func)(void*), u32 n, u32 us, C_TIM* etim);
#ifdef USE_FREERTOS
	void put_freertos_timer(void (*func)(void*), u32 n, u32 ms);
#endif
	void put_timer_stop();
};

class BuffHeadReads{
private:
	Buffer *buff;
	u32 heads[24]; //24heads
	u32 enables;
#ifdef USE_FREERTOS
	osEventFlagsId_t ef; //bit without flag: head is waiting
#endif
public:
	BuffHeadReads(Buffer *buff);
	void Init();
	int new_head();  //TODO: return Head obj
	void disable_head(int head_id);
	void* get_frames(u32 head_id, u32 n);
	u32 operator[](u32 i);
	void notify(u32 fid);
	friend class BuffHeadWrite;
	friend class BuffHeadRead;
};

class BuffHeadRead{
private:
	BuffHeadReads &heads;
	u32 head_id;
public:
	BuffHeadRead(BuffHeadReads &heads, u32 head_id);
	void* get_frames(u32 n);
	u32 bytes_elem();
};


#endif /* DATAFLOW_INC_BUFFHEAD_HPP_ */
