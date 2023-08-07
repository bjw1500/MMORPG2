// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class MMORPG2_API BufferHelper
{
public:
	BufferHelper();
	~BufferHelper();
};


class BufferReader
{
public:
	BufferReader();
	BufferReader(BYTE* buffer, uint32 size, uint32 pos = 0);
	~BufferReader();

	BYTE* Buffer() { return _buffer; }
	uint32			Size() { return _size; }
	uint32			ReadSize() { return _pos; }
	uint32			FreeSize() { return _size - _pos; }

	template<typename T>
	bool			Peek(T* dest) { return Peek(dest, sizeof(T)); }
	bool			Peek(void* dest, uint32 len);

	template<typename T>
	bool			Read(T* dest) { return Read(dest, sizeof(T)); }
	bool			Read(void* dest, uint32 len);

	template<typename T>
	BufferReader& operator>>(OUT T& dest);

private:
	BYTE* _buffer = nullptr;
	uint32			_size = 0;
	uint32			_pos = 0;
};

template<typename T>
inline BufferReader& BufferReader::operator>>(OUT T& dest)
{
	dest = *reinterpret_cast<T*>(&_buffer[_pos]);
	_pos += sizeof(T);
	return *this;
}

class BufferWriter
{
public:
	BufferWriter();
	BufferWriter(BYTE* buffer, uint32 size, uint32 pos = 0);
	~BufferWriter();

	BYTE* Buffer() { return _buffer; }
	uint32			Size() { return _size; }
	uint32			WriteSize() { return _pos; }
	uint32			FreeSize() { return _size - _pos; }

	template<typename T>
	bool			Write(T* src) { return Write(src, sizeof(T)); }
	bool			Write(void* src, uint32 len);

	template<typename T>
	T* Reserve();

	template<typename T>
	BufferWriter& operator<<(T&& src);

private:
	BYTE* _buffer = nullptr;
	uint32			_size = 0;
	uint32			_pos = 0;
};

template<typename T>
T* BufferWriter::Reserve()
{
	if (FreeSize() < sizeof(T))
		return nullptr;

	T* ret = reinterpret_cast<T*>(&_buffer[_pos]);
	_pos += sizeof(T);
	return ret;
}

template<typename T>
BufferWriter& BufferWriter::operator<<(T&& src)
{
	using DataType = std::remove_reference_t<T>;
	*reinterpret_cast<DataType*>(&_buffer[_pos]) = std::forward<DataType>(src);
	_pos += sizeof(T);
	return *this;
}