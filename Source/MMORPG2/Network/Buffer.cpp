// Fill out your copyright notice in the Description page of Project Settings.

#include "Buffer.h"

Buffer::Buffer()
{
}

Buffer::~Buffer()
{
}


/*----------------
	SendBuffer
-----------------*/

SendBuffer::SendBuffer(int32 bufferSize)
{
	_buffer.resize(bufferSize);
}

SendBuffer::~SendBuffer()
{
}

void SendBuffer::CopyData(void* data, int32 len)
{
	::memcpy(_buffer.data(), data, len);
	_writeSize = len;
}

void SendBuffer::Close(uint32 writeSize)
{
	_writeSize = writeSize;
}


/*--------------
	RecvBuffer
----------------*/

RecvBuffer::RecvBuffer(int32 bufferSize) : _bufferSize(bufferSize)
{
	_capacity = bufferSize * BUFFER_COUNT;
	_buffer.resize(_capacity);
}

RecvBuffer::~RecvBuffer()
{
}

void RecvBuffer::Clean()
{
	int32 dataSize = DataSize();
	if (dataSize == 0)
	{
		// �� ��ħ �б�+���� Ŀ���� ������ ��ġ���, �� �� ����.
		_readPos = _writePos = 0;
	}
	else
	{
		// ���� ������ ���� 1�� ũ�� �̸��̸�, �����͸� ������ �����.
		if (FreeSize() < _bufferSize)
		{
			::memcpy(&_buffer[0], &_buffer[_readPos], dataSize);
			_readPos = 0;
			_writePos = dataSize;
		}
	}
}

bool RecvBuffer::OnRead(int32 numOfBytes)
{
	if (numOfBytes > DataSize())
		return false;

	_readPos += numOfBytes;
	return true;
}

bool RecvBuffer::OnWrite(int32 numOfBytes)
{
	if (numOfBytes > FreeSize())
		return false;

	_writePos += numOfBytes;
	return true;
}