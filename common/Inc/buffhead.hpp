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
#ifdef USE_FREERTOS
	osSemaphoreId_t lock;  //lock for putting data
#endif
public:
	BuffHeadWrite(Buffer *buff, u32 fid):BuffHead(buff, fid){};
	void Init();
	u32 bytes_elem();
	u32 put_elem(void* elem);
	void* put_dma_once(u32 N_elem);
	void* put_dma_cycle(u32 cycle);
	u32 get_capacity();
	void put_dma_notify(u32 N_elem);
	u32 put_dma_finish();
	void wait_lock();
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
	int new_head();
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
