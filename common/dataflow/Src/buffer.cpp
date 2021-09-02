/*
 * buffer.cpp
 *
 *  Created on: Apr 28, 2021
 *      Author: xrj
 */

#include "../Inc/buffer.hpp"
#include <cstdlib>
#include "../Inc/myints.h"
#include "mylibs_config.hpp"


Buffer::Buffer(u32 byte_elem):be(byte_elem), w_head(this, 0), r_heads(this)
{
	this->p0 = nullptr;
	this->n_wait = 0;
	w_head.fid = 0;
}

Buffer::~Buffer()
{
	XFree(this->p0);
}

void Buffer::Init()
{
	w_head.Init();
	r_heads.Init();
}

BuffState Buffer::remalloc(u32 capacity)
{
	if(this->p0){
		XFree(this->p0);
	}
	void *p = XMalloc(be*capacity);
	if(p){
		this->p0 = p;
		this->capacity = capacity;
		return Buff_OK;
	}return Buff_Error;
}

u32 Buffer::get_capacity()
{
	return capacity;
}

u32 Buffer::bytes_elem()
{
	return be;
}

ElemState Buffer::get_ElemState(u32 i)
{
	if(i > w_head.fid){
		return Elem_NoPuted;
	}if(i+capacity >= w_head.fid){ //TODO: u32 overflow
		return Elem_GetAble;
	}return Elem_OverWritten;
}

void* Buffer::operator[] (u32 i)
{
	if(get_ElemState(i) != Elem_GetAble){
		return nullptr;
	}
	return (void*)((u8*)p0 + (i%capacity)*be);
}

u32 Buffer::last_fid()
{
	return w_head.fid;
}

void Buffer::reset()
{
	w_head.fid = 0;
}
