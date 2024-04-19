#pragma once
#include "IRender.h"
#include "Jnx_Threading_Lib.h"
#include "IJnx_Engine.h"
#include <iostream>
#include <debugapi.h>

#include <d3d12.h>
#include <wrl/client.h>
using namespace Microsoft::WRL;

#pragma comment(lib, "d3d12.lib")

#define WLABEL L"Chromatic Engine"
#define WIDTH  640
#define HEIGHT 480

namespace JNX {
	class ThreadEngine;

	class RenderEngine : public JNX::IRenderEngine {
		std::shared_ptr<JNX::ILogicEngine> logicEngine {};
		std::shared_ptr<JNX::ISharedResources> sharedResources {};

	private:
		//ComPtr<

		void InitializeDirectX();
	public:
		RenderEngine() = delete;
		RenderEngine(std::shared_ptr<JNX::ISharedResources>);
		virtual ~RenderEngine() {
			int i = 0;
		};

		void connectLogicEngine(std::shared_ptr<JNX::ILogicEngine>);

		RenderLoop Render(IPackedArguments** args);
		void shutdown();
	};
};