#pragma once
#include "Thread_Engine.h";
#include <iostream>;
#include <Windows.h>;
#include <memory>;

#ifdef _DEBUG
	#ifndef DBG_NEW
		#define DBG_NEW new ( _NORMAL_BLOCK, __FILE__, __LINE__ )
		#define new DBG_NEW
	#endif
#endif

#pragma region Thread_Engine
#pragma region ThreadEngine
JNX::ThreadEngine::ThreadEngine(const UINT allocatedThreads)
try : queueSystem(std::make_shared<Queue_System>()), 
	  threadPool(std::make_unique<Thread_Pool>(allocatedThreads, this->queueSystem)) 
{}
catch (std::exception& e) {
	OutputDebugStringA(e.what());
}
JNX::ThreadEngine::~ThreadEngine() 
{
	this->threadPool->Shutdown();
}

HRESULT JNX::ThreadEngine::createArgumentList(IPackedArguments** packedArguments) {
	try {
		*packedArguments = new PackedArguments();
	}
	catch (std::exception&) {
		return E_FAIL;
	}
	return S_OK;
}

HRESULT JNX::ThreadEngine::createQueue(std::shared_ptr<ICommandQueue>& queue, std::shared_ptr<IBarrier>& barrier) {
	try {
		barrier = std::make_shared<Barrier>();
		queue = std::make_shared<CommandQueue>(barrier);
	}
	catch (std::exception&) {
		return E_FAIL;
	}
	return S_OK;
}

HRESULT JNX::ThreadEngine::addQueue(std::shared_ptr<ICommandQueue>&& queue) {
	try {
		return this->queueSystem->addQueue(std::move(queue));
	}
	catch (std::exception&) {
		return E_FAIL;
	}
}

unsigned int JNX::ThreadEngine::getClassSize() {
	return sizeof(this);
}

void JNX::ThreadEngine::terminate() {
	this->queueSystem->terminate();
}

#pragma endregion

#pragma region Task
JNX::Task::Task(JNX::JOBTYPES type)
try : type(type)
{

}
catch (std::exception&)
{

}

JNX::Task::~Task()
{
	auto i = 0;
}
#pragma endregion

#pragma region Job
JNX::Job::Job(void(*function)(IPackedArguments**), IPackedArguments** args)
try : Task(JOBTYPES::JOB),
functionPointer(std::move(function)),
arguments(args)
{

}
catch (std::exception&)
{
	
}

JNX::Job::~Job()
{
	auto i = 0;
	//SAFE_DELETE(this->arguments);
	//functionPointer = nullptr;
}
#pragma endregion

#pragma region Event
JNX::Event::Event(void(*eventPointer)(IPackedArguments**), IPackedArguments**)
try : eventPointer(eventPointer), arguments(arguments)
{

}
catch (std::exception&)
{

}

JNX::Event::~Event()
{
	SAFE_DELETE(this->arguments);
}
#pragma endregion

#pragma region CommandQueue
JNX::CommandQueue::CommandQueue(std::shared_ptr<IBarrier>& barrier)
try : barrier(new Barrier())
{
	barrier = this->barrier;
}
catch (std::exception&) {}

JNX::CommandQueue::~CommandQueue()
{
	int i = 0;
}

HRESULT JNX::CommandQueue::push_task(std::shared_ptr<JNX::Task>&& task) {
	if (this->head != nullptr)
		this->head->next = task;
	task->previous = this->head;
	this->head = task;

	if (this->tail == nullptr)
		this->tail = this->head;

	return S_OK;
}

HRESULT JNX::CommandQueue::addJob(void(*function)(IPackedArguments**), IPackedArguments** argList) {
	try {
		std::shared_ptr<Job> job{
			std::make_shared<Job>(
				function,
				std::move(argList))
		};
		this->push_task(std::move(std::dynamic_pointer_cast<Task>(job)));

		return S_OK;
	}
	catch (std::exception&) {
		return E_FAIL;
	}
}

HRESULT JNX::CommandQueue::setCheckpoint() {
	try {
		std::shared_ptr<Task> barrier{
			std::make_shared<Task>(JNX::JOBTYPES::SIGNAL)
		};
		this->push_task(std::move(barrier));

		return S_OK;
	}
	catch (std::exception&) {
		return E_FAIL;
	}
}
HRESULT JNX::CommandQueue::setCheckpointWithEvent(void(*functionPointer)(IPackedArguments**), IPackedArguments** packedArguments) {
	try {
		if (SUCCEEDED(this->setCheckpoint())) {
			std::unique_ptr<Event> eventPtr{ std::make_unique<Event>(functionPointer, std::move(packedArguments)) };
			std::pair<UINT, std::unique_ptr<Event>> eventPair{ std::make_pair(this->barrier->count, std::move(eventPtr)) };
			this->barrier->eventQueue.push_back(std::move(eventPair));
			return S_OK;
		}
		return E_FAIL;
	}
	catch (std::exception&) {
		return E_FAIL;
	}
}

HRESULT JNX::CommandQueue::fetchNextTask(std::shared_ptr<Task>& task)
{
	try
	{
		if (this && this->tail) {
			task = this->tail;
			this->tail = task->next;
			if (!this->tail)
				if (this->head)
					this->head = this->tail;
		}
		else
			return E_FAIL;

		return S_OK;
	}
	catch (std::exception&)
	{
		return E_FAIL;
	}
}

HRESULT JNX::CommandQueue::executeTask(std::shared_ptr<JNX::Task>& task) {
	try {
		if (task->type & JNX::JOB) {

			std::shared_ptr<Job> job{ std::dynamic_pointer_cast<Job>(task) };
			job->functionPointer(std::move(job->arguments)); // variadic arguments
		}
		else if (task->type & JNX::SIGNAL) {
			for (auto i = 0; i < this->barrier->eventQueue.size(); ++i) 
			{
				if (this->barrier->eventQueue[i].first <= this->barrier->count &&
						this->barrier->eventQueue[i].second->eventPointer) 
				{
					this->barrier->eventQueue[i].second->eventPointer(
						std::move(this->barrier->eventQueue[i].second->arguments)
					);
				}
				else if (this->barrier->eventQueue[i].first > this->barrier->count) {
					return E_FAIL;
				}
			}
			++this->barrier->count;
			//std::shared_ptr<Fence> fence{ std::dynamic_pointer_cast<Fence>(task) };
			//TODO: ++this->fenceTail;
			//TODO: --this->fSize;

		}
		else if (task->type & JNX::EXIT) {
			return E_APPLICATION_EXITING;
		}
		return S_OK;
	}
	catch (std::exception&) {
		return E_FAIL;
	}
}

#pragma endregion

#pragma region Queue_System
JNX::Queue_System::~Queue_System() {
	if (!this->shutdown())
		this->terminate();
}

HRESULT JNX::Queue_System::addQueue(std::shared_ptr<ICommandQueue>&& p_commandQueue) {
	try {
		std::shared_ptr<CommandQueue> commandQueue{
			std::move(std::dynamic_pointer_cast<CommandQueue>(p_commandQueue))
		};
		
		commandQueue->previous = this->head.load();
		
		while (!this->head.compare_exchange_strong(commandQueue->previous, commandQueue));
		
		if (commandQueue->previous) [[unlikely]]
			commandQueue->previous->next = commandQueue;
		else
			this->tail.compare_exchange_strong(commandQueue->previous, commandQueue);
		
		//if (!this->tail.load()) [[unlikely]]
		//	this->tail.store(commandQueue);			
	}
	catch (std::exception&) {
		return E_FAIL;
	}
	return S_OK;
}

HRESULT JNX::Queue_System::fetchQueue(std::shared_ptr<CommandQueue>& p_commandQueue) {
	try {
		p_commandQueue = this->tail.load();
		if (p_commandQueue) {
			while (!this->tail.compare_exchange_strong(p_commandQueue, p_commandQueue->next));
			if (!p_commandQueue)
				this->head.store(nullptr);
		}
	}
	catch (std::exception& ex) {
		std::cout << ex.what() << std::endl;

		return E_FAIL;
	}
	return S_OK;
}
#pragma endregion

#pragma region Thread_Pool
JNX::Thread_Pool::Thread_Pool(const UINT allocatedThreads, std::shared_ptr<Queue_System> const& queueSystem)
try : queueSystem(queueSystem) 
{
	//auto lambda{
	//	[&queueSystem](/*std::shared_ptr<std::stop_token>& const stop_token*/) {
	//		//while (stop_token->stop_requested()) {
	//			std::shared_ptr<JNX::CommandQueue> queue{};
	//			queueSystem->fetchQueue(queue);
	//
	//			std::shared_ptr<JNX::Task> task {};
	//			while (SUCCEEDED(queue->fetchNextTask(task))) {
	//				while(queue->executeTask(task) != S_OK)
	//					std::this_thread::yield();
	//			}
	//		//};
	//	}
	//};

	//std::shared_ptr<std::stop_token> token{ std::make_shared<std::stop_token>() };

	for (size_t i = 0; i < allocatedThreads; ++i) {
		this->threads.push_back(std::jthread([&queueSystem](/*std::shared_ptr<std::stop_token>& const stop_token*/) {
			while (!queueSystem->shutdown()) {
				std::shared_ptr<JNX::CommandQueue> queue{};
				if (SUCCEEDED(queueSystem->fetchQueue(queue))) {
					if (queue) {
						std::shared_ptr<JNX::Task> task{};
						while (SUCCEEDED(queue->fetchNextTask(task))) {
							if (task) {
								HRESULT res = queue->executeTask(task);
								if (res == E_APPLICATION_EXITING)
									queueSystem->terminate();
								//if (FAILED(res))
								//	throw;
							}
						}
					}
				}
			};
		}));
	}
}
catch (std::exception& e) {
	OutputDebugStringA(e.what());
}

JNX::Thread_Pool::~Thread_Pool() {
	int i = 0;
}

HRESULT JNX::Thread_Pool::Shutdown() {
	try {
		for (auto& _thread : this->threads)
		{
			_thread.request_stop();
		}
	}
	catch (std::exception&) {
		return E_FAIL;
	}
	return S_OK;
}

//HRESULT JNX::Thread_Pool::ShutdownThread(UINT threadID) {
//	try {
//		for (auto& thread : this->threads) {
//			std::jthread::id id { thread.get_id() };
//			if (id == threadID) //Not allowed, find another way.
//				this->threads.at(threadID).request_stop();
//		}
//		return S_OK;
//	}
//	catch (std::exception& e) {
//		return E_FAIL;
//	}
//}

HRESULT JNX::Thread_Pool::ShutdownThreadByIndex(UINT index) {
	try {
		this->threads.at(index).request_stop();
	}
	catch (std::exception&) {
		return E_FAIL;
	}
	return S_OK;
}

#pragma endregion

#pragma endregion

#pragma region Window
static JNX::IThreadEngine* s_threadengine;


JNX::IThreadEngine* JNX::IThreadEngine::CreateThreadEngine(unsigned int allocatedthreads) {
	try {
		if (!s_threadengine)
			s_threadengine = new ThreadEngine(allocatedthreads);
		return s_threadengine;
	}
	catch (std::exception&) {
		if (s_threadengine)
			delete s_threadengine;
		exit(-1);
	}
}
#pragma endregion