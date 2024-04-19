#pragma once
#include <stdarg.h>;
#include <Windows.h>;
#include <utility>;
#include <vector>;
#include <memory>;


namespace JNX {
	//enum memory_order {
	//	memory_order_relaxed,
	//	memory_order_consume,
	//	memory_order_acquire,
	//	memory_order_release,
	//	memory_order_acq_rel,
	//	memory_order_seq_cst
	//};

	enum JOBTYPES : char {
		EXIT = 1 << 0, // 0001
		JOB = 1 << 1, // 0010
		SIGNAL = 1 << 2  // 0100
	};

	struct IArgument {
		virtual ~IArgument() {};
		virtual void** get_arg() = 0;
		virtual const unsigned long long get_type() = 0;
		virtual void set_type(const unsigned long long type) = 0;
		virtual void release_arg() = 0;
	};

	struct IPackedArguments {
		virtual ~IPackedArguments() {};
		virtual void addArgument(void** data, unsigned long long type) = 0;
		virtual IArgument* fetchArgument() = 0;
		virtual size_t getArgumentCount() = 0;
	};

	struct IBarrier {
		virtual ~IBarrier() {};
		virtual HRESULT getCurrentCheckpoint() = 0;
	};

	struct ICommandQueue {
		virtual ~ICommandQueue() {};
		virtual HRESULT addJob(void(*functionPointer)(IPackedArguments**), IPackedArguments**) = 0;
		virtual HRESULT setCheckpoint() = 0;
		virtual HRESULT setCheckpointWithEvent(void(*functionPointer)(IPackedArguments**), IPackedArguments**) = 0;
		virtual const UINT const getCurrentFence() = 0;
	};

	struct IThreadEngine {
		virtual ~IThreadEngine() {};
		static IThreadEngine* CreateThreadEngine(unsigned int allocatedthreads);

		virtual HRESULT createArgumentList(IPackedArguments**) = 0;

		virtual HRESULT createQueue(std::shared_ptr<ICommandQueue>&, std::shared_ptr<IBarrier>&) = 0;
		virtual HRESULT addQueue(std::shared_ptr<ICommandQueue>&&) = 0;
		virtual unsigned int getClassSize() = 0;

		[[noexcept]]
		virtual void terminate() = 0;
	};

	//typedef IThreadEngine& (__stdcall* CreateThreadEngine)(unsigned int);
	//extern "C++" IThreadEngine& CreateThreadEngine(unsigned int);
}




















































