//#pragma once
//#include "Thread_Engine.h";
//#include <memory>;
//
//static JNX::IThreadEngine* s_threadengine;
//
//
//JNX::IThreadEngine& JNX::IThreadEngine::CreateThreadEngine(unsigned int allocatedthreads) {
//	try {
//		if (!s_threadengine)
//			s_threadengine = new ThreadEngine(allocatedthreads);
//		return *s_threadengine;
//	}
//	catch (const std::exception& const e){
//		if (s_threadengine)
//			delete s_threadengine;
//			exit(-1);
//	}
//}