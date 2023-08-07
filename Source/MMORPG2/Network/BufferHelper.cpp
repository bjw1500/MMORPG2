// Fill out your copyright notice in the Description page of Project Settings.


#include "BufferHelper.h"

BufferHelper::BufferHelper()
{
}

BufferHelper::~BufferHelper()
{
}



/*----------------
	BufferReader
-----------------*/

BufferReader::BufferReader()
{
}

BufferReader::BufferReader(BYTE* buffer, uint32 size, uint32 pos)
	: _buffer(buffer), _size(size), _pos(pos)
{

}

BufferReader::~BufferReader()
{

}

bool BufferReader::Peek(void* dest, uint32 len)
{
	if (FreeSize() < len)
		return false;

	::memcpy(dest, &_buffer[_pos], len);
	return true;
}

bool BufferReader::Read(void* dest, uint32 len)
{
	if (Peek(dest, len) == false)
		return false;

	_pos += len;
	return true;
}

/*----------------
	BufferWriter
-----------------*/

BufferWriter::BufferWriter()
{

}

BufferWriter::BufferWriter(BYTE* buffer, uint32 size, uint32 pos)
	: _buffer(buffer), _size(size), _pos(pos)
{

}

BufferWriter::~BufferWriter()
{

}

bool BufferWriter::Write(void* src, uint32 len)
{
	if (FreeSize() < len)
		return false;

	::memcpy(&_buffer[_pos], src, len);
	_pos += len;
	return true;
}