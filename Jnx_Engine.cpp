#include "Jnx_Engine.h"

#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK, __FILE__, __LINE__ )
#define new DBG_NEW
#endif
#endif

JNX::LogicEngine::LogicEngine(std::shared_ptr<JNX::ISharedResources> sharedResource)
{
	this->sharedResource = sharedResource;
}

JNX::ILogicEngine* JNX::ILogicEngine::CreateLogicEngine(std::shared_ptr<JNX::ISharedResources> sharedResource) {
	try {
		return new JNX::LogicEngine(sharedResource);
	}
	catch (const std::exception& const) {
		exit(-1);
	}
}


int JNX::LogicEngine::MainLoop() {
	//Render deallocates LogicEngine while MainLoop is still running on another thread.
	//shared_ptr only has one refCount.

	while (!this->sharedResource->isRunning()) {
		OutputDebugStringA("testing main loop\n");
	}

	return 0;
}

void JNX::LogicEngine::connectRenderEngine(std::shared_ptr<JNX::IRenderEngine> renderEngine)
{
	this->renderEngine = renderEngine;
}

JNX::ISharedResources* JNX::ISharedResources::CreateSharedResource() {
	try {
		JNX::ISharedResources* sharedResource{ new JNX::SharedResources() };
		return sharedResource;
	}
	catch (const std::exception& const) {
		exit(-1);
	}
}
 
bool JNX::SharedResources::isRunning()
{
	const std::lock_guard<std::mutex> lock{ this->exit_mutex };
	return this->exit_flag;
}

void JNX::SharedResources::shutdown()
{
	const std::lock_guard<std::mutex> lock{ this->exit_mutex };
	this->exit_flag = true;
}