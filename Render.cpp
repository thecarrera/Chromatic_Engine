#include "Render.h"
#include "Jnx_Graphics_Lib.h"s

#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK, __FILE__, __LINE__ )
#define new DBG_NEW
#endif
#endif

JNX::RenderEngine::RenderEngine(std::shared_ptr<JNX::ISharedResources> sharedResources) {
	this->sharedResources = sharedResources;
}

JNX::IRenderEngine* JNX::IRenderEngine::CreateRenderEngine(std::shared_ptr<JNX::ISharedResources> sharedResources) {
	OutputDebugStringA("test\n");
	try {
		return new JNX::RenderEngine(sharedResources);
	}
	catch (const std::exception& const) {
		exit(-1);
	}
}

void JNX::RenderEngine::connectLogicEngine(std::shared_ptr<JNX::ILogicEngine> logicEngine) {
	this->logicEngine = logicEngine;
}

void JNX::RenderEngine::shutdown() {
	this->sharedResources->shutdown();
}

HWND InitWindow(HINSTANCE hInstance);
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);

JNX::RenderEngine::RenderLoop JNX::RenderEngine::Render(JNX::IPackedArguments** args) {
	return [](IPackedArguments** args) -> void {
		std::shared_ptr<JNX::IThreadEngine> threadEngine{};
		std::shared_ptr<JNX::IRenderEngine> renderEngine{};
		std::shared_ptr<JNX::ILogicEngine>  logicEngine{};

		while ((*args)->getArgumentCount() > 0) {
			JNX::IArgument* arg{ (*args)->fetchArgument() };
			if (arg != nullptr) {
				JNX::ThreadResourceTypes argType{ static_cast<JNX::ThreadResourceTypes>(arg->get_type()) };
				if (argType == JNX::ThreadResourceTypes::T_THREADENGINE) {
					std::shared_ptr<JNX::IThreadEngine>* thread_ref { reinterpret_cast<std::shared_ptr<JNX::IThreadEngine>*>(arg->get_arg()) };
					threadEngine = *thread_ref;
				}
				else if (argType == JNX::ThreadResourceTypes::T_RENDERENGINE) {
					std::shared_ptr<JNX::IRenderEngine>* render_ref { reinterpret_cast<std::shared_ptr<JNX::IRenderEngine>*>(arg->get_arg()) };
					renderEngine = *render_ref;
				}
				else if (argType == JNX::ThreadResourceTypes::T_LOGICENGINE) {
					std::shared_ptr<JNX::ILogicEngine>* logic_ref { reinterpret_cast<std::shared_ptr<JNX::ILogicEngine>*>(arg->get_arg()) };
					logicEngine = *logic_ref;
				}
				arg->release_arg();

				int i = 0;
			}
		}
		delete[] * args;
		args = nullptr;


		MSG msg{ 0 };
		HINSTANCE hInstance{};
		HWND wndHandle{ InitWindow(hInstance) };

		if (wndHandle) {
			ShowWindow(wndHandle, 1);

			while (WM_QUIT != msg.message) {
				if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
				else
				{
					OutputDebugStringA("testing render loop\n");
				}
			}
			DestroyWindow(wndHandle);
			renderEngine->shutdown();
		}
	};
}

HWND InitWindow(HINSTANCE hInstance)
{
	HICON icon = (HICON)LoadImage(NULL,
		L"Icon.ico",
		IMAGE_ICON,
		0,
		0,
		LR_LOADFROMFILE |
		LR_DEFAULTSIZE |
		LR_SHARED);


	WNDCLASSEX wce = { 0 };
	wce.hIcon = icon;
	wce.cbSize = sizeof(WNDCLASSEX);
	wce.style = CS_HREDRAW | CS_VREDRAW;
	wce.lpfnWndProc = WndProc;
	wce.hInstance = hInstance;
	wce.lpszClassName = WLABEL;
	if (!RegisterClassEx(&wce))
	{
		exit(-1);
	}

	RECT rc = { 0, 0, (long)WIDTH, (long)HEIGHT };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, TRUE);

	auto wWidth = rc.right - rc.left;
	auto wHeight = rc.bottom - rc.top;

	HWND handle = CreateWindow(
		WLABEL,
		WLABEL,
		WS_OVERLAPPEDWINDOW,
		((GetSystemMetrics(SM_CXSCREEN) / 2) - (wWidth / 2)),
		((GetSystemMetrics(SM_CYSCREEN) / 2) - (wHeight / 2)),
		wWidth,
		wHeight,
		nullptr,
		nullptr,
		hInstance,
		nullptr);

	return handle;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:

		if (wp == VK_ESCAPE)
		{
			PostQuitMessage(0);
			break;

		}
	}
	return DefWindowProc(hWnd, msg, wp, lp);
}