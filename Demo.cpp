//#pragma once
//#pragma comment(lib, "Onecore.lib")
//
////#include "Jnx_Threading_Lib.h";
//#include <iostream>;
//#include <Windows.h>;
//#include <stdexcept>;
//#include <string>;
//import <d3d12.h>;
//
//void testFunc(UINT&& argCount, ...) {
//	va_list argList;
//	UINT count = argCount;
//	va_start(argList, count);
//	for (size_t i = 0; i < argCount; ++i)
//	{
//		std::string test {"Func print: " + std::to_string(va_arg(argList, int))};
//		OutputDebugStringA(test.c_str());
//	}
//	va_end(argList);
//}

//int main() {
//	try {
//		std::string tmp { "Existing threads: " + std::to_string(std::jthread::hardware_concurrency()) + "\n"};
//		OutputDebugStringA(tmp.c_str());
//		//JNX::IThreadEngine& th { JNX::IThreadEngine::CreateThreadEngine(std::jthread::hardware_concurrency()) };
//
//		//th.test();
//
//
//		//std::shared_ptr<JNX::ICommandQueue> cq {};
//		//std::shared_ptr<JNX::IBarrier> b{};
//
//		//th.createQueue(cq, b);
//
//		//cq->addJob(&testFunc, 1, 1);
//
//
//	}
//	catch (const std::exception& e) {
//		OutputDebugStringA(e.what());
//	}
//	return 0;
//}








//#include "Jnx_Engine.h";
//#include "Jnx_Threading_Lib.h";
//#include <cstdio>
//#include <memory>
//#include <queue>
//#include <thread>
//#include <string>
//
//void someFunc(JNX::IPackedArguments** args) {
//    JNX::IArgument* arg{ (*args)->fetchArgument() };
//    int* some{ reinterpret_cast<int*>(arg->get_arg()) };
//
//    switch (static_cast<JNX::ThreadResourceTypes>(arg->get_type()))
//    {
//    case JNX::ThreadResourceTypes::TEST:
//        break;
//    case JNX::ThreadResourceTypes::TEST1:
//        break;
//    case JNX::ThreadResourceTypes::INT:
//        some = reinterpret_cast<int*>(arg->get_arg());
//        OutputDebugStringA("some values\n");
//        break;
//    default:
//        break;
//    }
//}
//
//int main()
//{
//    JNX::IThreadEngine* threadEngine{ &JNX::IThreadEngine::CreateThreadEngine(std::jthread::hardware_concurrency() - 1) };
//    JNX::IPackedArguments* argList;
//    std::shared_ptr<JNX::ICommandQueue> queue;
//    std::shared_ptr<JNX::IBarrier> barrier;
//    threadEngine->createArgumentList(&argList);
//    threadEngine->createQueue(queue, barrier);
//
//    int* someVar1 = new int(10);
//    argList->addArgument(reinterpret_cast<void**>(someVar1), static_cast<unsigned long long>(JNX::ThreadResourceTypes::INT));
//    queue->addJob(someFunc, &argList);
//
//    threadEngine->addQueue(std::move(queue));
//    threadEngine->test();
//
//    system("pause");
//}