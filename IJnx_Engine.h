#pragma once
#include <memory>
#include <windows.h>

namespace JNX {
	class IRenderEngine;
	class ISharedResources;

	enum class ThreadResourceTypes {
		T_VOID,
		T_BOOL,
		T_CHAR,
		T_THREADENGINE,
		T_RENDERENGINE,
		T_LOGICENGINE
	};

	class ILogicEngine {
	public:
		virtual ~ILogicEngine() {};
		static ILogicEngine* CreateLogicEngine(std::shared_ptr<JNX::ISharedResources>);

		virtual int MainLoop() = 0;

		virtual void connectRenderEngine(std::shared_ptr<JNX::IRenderEngine>) = 0;
	};

	class ISharedResources {
		friend class IRenderEngine;
	public:
		virtual ~ISharedResources() {};
		static ISharedResources* CreateSharedResource();

		virtual bool isRunning() = 0;
		virtual void shutdown() = 0;
	};

	class IFrameState {
	public:
		virtual ~IFrameState() {};
	};
}