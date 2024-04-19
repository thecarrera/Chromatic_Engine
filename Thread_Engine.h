#pragma once
#pragma comment(lib, "Onecore.lib")
#include "Jnx_Threading_Lib.h";

#include <Windows.h>;
#include <iostream>;
#include <utility>;
#include <functional>;
#include <vector>;
#include <deque>;
#include <exception>;
#include <thread>;
#include <atomic>;
#include <map>;
#include <mutex>;

#define SAFE_DELETE(x) if(x) delete[] *x, x = nullptr

namespace JNX {

	class Argument : public IArgument {
		unsigned long long type{};
		void** arg{};
	public:
		Argument() = delete;
		Argument(void** arg, unsigned long long type) : arg(arg), type(type) {};
		virtual ~Argument() { SAFE_DELETE(arg); };

		void** get_arg()
		{
			return this->arg;
		};
		void set_arg(void** arg) {
			this->arg = arg;
		}

		const unsigned long long get_type() {
			return this->type;
		}
		void set_type(const unsigned long long type) {
			this->type = type;
		}

		void release_arg() {
			this->arg = nullptr;
		}
	};

	class PackedArguments : public IPackedArguments {
		std::deque<Argument*> arguments{};

	public:
		PackedArguments() = default;
		virtual ~PackedArguments() {
			for (auto i = 0; i < arguments.size(); ++i)
				delete arguments[i];
		};
		void addArgument(void** data, unsigned long long type) {
			Argument* arg { new Argument(data, type) };
			this->arguments.emplace_back(arg);
		};

		IArgument* fetchArgument() {
			Argument* arg{ arguments.front() };
			this->arguments.pop_front();
			return arg;
		}

		size_t getArgumentCount() {
			return this->arguments.size();
		}

	};
};

namespace JNX {
	class Task {
	public:
		JOBTYPES type{};
		std::shared_ptr<Task> next{};
		std::shared_ptr<Task> previous{};

		Task() = delete;
		Task(JOBTYPES type);
		virtual ~Task();
	};

	class Job : public Task {
	public:
		void (*functionPointer)(IPackedArguments**) {};
		IPackedArguments** arguments {};

		Job() = delete;
		Job(void(*function)(IPackedArguments**), IPackedArguments**);
		virtual ~Job();
	};

	class Event {
		void (*eventPointer)(IPackedArguments**) {};
		IPackedArguments** arguments{};

	public:
		friend class CommandQueue;
		Event() = delete;
		Event(void(*eventPointer)(IPackedArguments**), IPackedArguments**);
		virtual ~Event();
	};

	class Barrier : public IBarrier {
		UINT count{};
		std::deque<std::pair<UINT, std::unique_ptr<Event>>> eventQueue{};
	public:
		friend class CommandQueue;
		Barrier() = default;
		virtual ~Barrier() = default;

		HRESULT getCurrentCheckpoint() { return this->count; };
	};

	class CommandQueue : public ICommandQueue {
		friend class Queue_System;

		std::shared_ptr<Barrier> barrier{};
		std::shared_ptr<Task> head{nullptr};
		std::shared_ptr<Task> tail{nullptr};
		std::shared_ptr<CommandQueue> previous{};
		std::shared_ptr<CommandQueue> next{};

		HRESULT push_task(std::shared_ptr<Task>&&);
	public:
		CommandQueue() = delete;
		CommandQueue(std::shared_ptr<IBarrier>&);
		virtual ~CommandQueue();

		HRESULT addJob(void(*function)(IPackedArguments**), IPackedArguments**);
		HRESULT setCheckpoint();
		HRESULT setCheckpointWithEvent(void(*functionPointer)(IPackedArguments**), IPackedArguments**);
		HRESULT fetchNextTask(std::shared_ptr<Task>&);
		HRESULT executeTask(std::shared_ptr<Task>&);
		const UINT const getCurrentFence() { return this->barrier->count; };

	};

	class Queue_System {
		friend class Thread_Pool;
		
		std::atomic<std::shared_ptr<CommandQueue>> head{};
		std::atomic<std::shared_ptr<CommandQueue>> tail{};
		std::mutex exit_mutex {};
		bool exit {};
	public:
		Queue_System() = default;
		virtual ~Queue_System();

		HRESULT addQueue(std::shared_ptr<ICommandQueue>&&);
		HRESULT fetchQueue(std::shared_ptr<CommandQueue>&);
		void terminate() {
			std::lock_guard<std::mutex> lock{ this->exit_mutex };
			this->exit = true; 
		};
		bool shutdown() const { return this->exit; };
	};
	
	class Thread_Pool {
		std::shared_ptr<Queue_System> queueSystem {};
		std::vector<std::jthread> threads {};

	public:
		Thread_Pool() = delete;
		Thread_Pool(const UINT allocatedThreads, std::shared_ptr<Queue_System> const& queueSystem);
		virtual ~Thread_Pool();
		
		HRESULT Shutdown();
		//HRESULT ShutdownThread(UINT threadID);
		HRESULT ShutdownThreadByIndex(UINT index);
	};
	
	class ThreadEngine : public IThreadEngine {	
	public:
		std::shared_ptr<Queue_System> queueSystem {};
		std::unique_ptr<Thread_Pool>  threadPool  {};
		
		ThreadEngine(const UINT allocatedThreads);
		virtual ~ThreadEngine();
		
		HRESULT createArgumentList(IPackedArguments**);

		HRESULT createQueue(std::shared_ptr<ICommandQueue>&, std::shared_ptr<IBarrier>&);
		HRESULT addQueue(std::shared_ptr<ICommandQueue>&&);
		unsigned int getClassSize();

		void terminate();

		//ICommandQueue&& createQueue() { return std::move(*new CommandQueue()); };
		//HRESULT addQueue(ICommandQueue&& commandQueue);
		//void QueueFence();
		//void Shutdown();
		//void ShutdownThread(UINT threadID);
		//void ShutdownThreadByIndex(UINT index);
		//HRESULT test();
	};
}