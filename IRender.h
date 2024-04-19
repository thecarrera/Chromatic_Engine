#pragma once
#include <iostream>

namespace JNX {
	class ISharedResources;
	class IPackedArguments;
	class ILogicEngine;

	class IRenderEngine {
	public:
		virtual ~IRenderEngine() {};

		static IRenderEngine* CreateRenderEngine(std::shared_ptr<JNX::ISharedResources>);
		
		virtual void connectLogicEngine(std::shared_ptr<JNX::ILogicEngine>) = 0;

		typedef void (*RenderLoop)(IPackedArguments** args);
		virtual RenderLoop Render(IPackedArguments** args) = 0;
		virtual void shutdown() = 0;
	};
	
	void render(JNX::IPackedArguments**);
};