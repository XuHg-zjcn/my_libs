/*
 * buffer.hpp
 *
 *  Created on: Apr 28, 2021
 *      Author: xrj
 */

#include "mylibs_config.hpp"
#include "buffhead.hpp"

#if !defined(INC_BUFFER_HPP_) && defined(DATAFLOW_INC_BUFFHEAD_HPP_)
#define INC_BUFFER_HPP_

#include "../../others/Inc/myints.h"

typedef enum{
	Buff_OK = 0,
	Buff_Error,
	Buff_Timeout
}BuffState;

typedef enum{
	Elem_GetAble = 0,
	Elem_OverWritten,
	Elem_NoPuted
}ElemState;

//TODO: read heads keep, when write head over the heads, will stop put data
class Buffer{
protected:
	void *p0;
	const u32 be;
	u32 capacity;
	u32 n_wait;
	void increse(u32 i);
public:
	BuffHeadWrite w_head;
	BuffHeadReads r_heads;
	Buffer(u32 byte_elem);
	~Buffer();
	void Init();
	BuffState remalloc(u32 capacity);
	//u32 remalloc(float sys_perc);
	u32 get_capacity();
	u32 bytes_elem();
	ElemState get_ElemState(u32 i);
	void* operator[] (u32 i);
	u32 last_fid();
	void reset();
	//put element operates
	friend BuffHead;
	friend BuffHeadRead;
	friend BuffHeadWrite;
};

#endif /* INC_BUFFER_HPP_ */
