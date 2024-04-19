#include "IJnx_Engine.h"
#include "IRender.h"
#include "Jnx_Threading_Lib.h"
#include <thread>;

#ifdef _DEBUG
    #ifndef DBG_NEW
        #define DBG_NEW new ( _NORMAL_BLOCK, __FILE__, __LINE__ )
        #define new DBG_NEW
    #endif
#endif

int main()
{
    // ###################################################################################
    // ##                                       WIP                                     ##
    // ##                                                                               ##
    // ##   This repository is only a proof of concept.                                 ##
    // ##   I wanted to have a thread queue that could add any function pointer or      ##
    // ##   variadic arguments. But since I'll be using dlls I didn't want to take      ##
    // ##   advantage of any C++ feature like templates or smart pointers because of    ##
    // ##   name mangling.                                                              ##
    // ##   My original thought was to cast function pointers and member function       ##
    // ##   pointers to regular void pointers and only store their first memory         ##
    // ##   address. I also originally wanted to use C's variadic arguments, but since  ##
    // ##   You either can't cast function pointers and C's variadic arguments ends up  ##
    // ##   sending as pass-by-value. I reverted to this implementation                 ##
    // ##                                                                               ##
    // ##   Intellisense also didn't work on my original project with dlls and c++      ##
    // ##   modules.                                                                    ##
    // ##   (this is fairly known since as of date modules are still experimental).     ##
    // ##   To save some time I've temporarily moved the code here so that I can focus  ##
    // ##   on implementing a functional thread engine and profiling/debug engine.      ##
    // ##   I'm also planning on implementing a memory allocator for saving guids etc.  ##
    // ##                                                                               ##
    // ##   There's also gonna be a lot of refactoring adding more abstaction with      ##
    // ##   templates and factories.                                                    ##
    // ##                                                                               ##
    // ###################################################################################   

    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    //_CrtSetBreakAlloc(175);

    std::shared_ptr<JNX::ISharedResources> sharedResources {};
    std::shared_ptr<JNX::IThreadEngine>    threadEngine    {};
    std::shared_ptr<JNX::IRenderEngine>    renderEngine    {};
    std::shared_ptr<JNX::ILogicEngine>     logicEngine     {};
    JNX::IPackedArguments*                 argList         {};
    std::shared_ptr<JNX::ICommandQueue> queue   {};
    std::shared_ptr<JNX::IBarrier>      barrier {};
    
    try {
        sharedResources.reset(JNX::ISharedResources::CreateSharedResource());
        threadEngine.reset(JNX::IThreadEngine::CreateThreadEngine(1 /*std::jthread::hardware_concurrency() - 1)*/ ));
        renderEngine.reset(JNX::IRenderEngine::CreateRenderEngine(sharedResources));
        logicEngine.reset(JNX::ILogicEngine::CreateLogicEngine(sharedResources));
        renderEngine->connectLogicEngine(logicEngine);
        logicEngine->connectRenderEngine(renderEngine);


        threadEngine->createArgumentList(&argList);
        threadEngine->createQueue(queue, barrier);

        argList->addArgument(
            reinterpret_cast<void**>(&threadEngine),
            static_cast<unsigned long long>(JNX::ThreadResourceTypes::T_THREADENGINE)
        );
        argList->addArgument(
            reinterpret_cast<void**>(&renderEngine),
            static_cast<unsigned long long>(JNX::ThreadResourceTypes::T_RENDERENGINE)
        );
        argList->addArgument(
            reinterpret_cast<void**>(&logicEngine),
            static_cast<unsigned long long>(JNX::ThreadResourceTypes::T_LOGICENGINE)
        );

        queue->addJob(renderEngine->Render(&argList), &argList);
        threadEngine->addQueue(std::move(queue));
        
        logicEngine->MainLoop();
    }
    catch (std::exception ex) {
        std::cout << ex.what() << std::endl;
    }
    sharedResources->shutdown();
    threadEngine->terminate();
    return 0;
}