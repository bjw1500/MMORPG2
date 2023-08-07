// Fill out your copyright notice in the Description page of Project Settings.


#include "FWorker.h"
#include "GameManager.h"

FWorker::FWorker(function<void(void)> callback) : _callback(callback)
{
	FString string = *FString::Printf(TEXT("Create Worker"));
	Utils::DebugLog(string);
	bRunThread = true;
	Thread = FRunnableThread::Create(this, TEXT("Thread ID 0"), 0, TPri_BelowNormal);
}

FWorker::FWorker()
{
	Thread = FRunnableThread::Create(this, TEXT("Thread ID [%d]"), 0, TPri_BelowNormal);
}

FWorker::~FWorker()
{
	Utils::DebugLog(TEXT("~Worker"));
	if (Thread)
	{
		Thread->Kill();
		delete Thread;
	}
}

bool FWorker::Init()
{
	return true;
}

uint32 FWorker::Run()
{

	// recv while loop ����
	// StopTaskCounter Ŭ���� ������ ����� Thread Safety�ϰ� ����
	while (bRunThread)
	{
		//Utils::DebugLog(TEXT("Dispatch"));
		_callback();
	}
	Utils::DebugLog(TEXT("Run End"));
	return 0;
}

void FWorker::Stop()
{
	Utils::DebugLog(TEXT("Woker Run Stop"));
	bRunThread = false;

	//Thread->WaitForCompletion();
	//Thread->Kill();
	//delete Thread;
	//Thread = nullptr;
}