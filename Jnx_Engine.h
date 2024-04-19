#pragma once
#include "IJnx_Engine.h";
#include <debugapi.h>
#include <atomic>
#include <mutex>

namespace JNX {
    class LogicEngine : public ILogicEngine {
    public:
        LogicEngine(std::shared_ptr<JNX::ISharedResources>);
        virtual ~LogicEngine() {
            int i = 0;
        };

        int MainLoop();

        void connectRenderEngine(std::shared_ptr<JNX::IRenderEngine>);
    private:
        std::shared_ptr<JNX::IRenderEngine>    renderEngine   {};
        std::shared_ptr<JNX::ISharedResources> sharedResource {};
    };

    class FrameState : public IFrameState {
    public:
        FrameState() {};
        virtual ~FrameState() {};

        std::atomic_bool isReady { false };
    };

    class SharedResources : public ISharedResources {
    public:
        SharedResources() {};
        virtual ~SharedResources() {
            int i = 0;
        };
        bool isRunning();
        void shutdown();

    private:
        volatile bool exit_flag { false };
        std::mutex exit_mutex {};
    };
}