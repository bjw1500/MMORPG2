#include "pch.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"

unique_ptr<ThreadManager> GThreadManager = make_unique<ThreadManager>();

