#pragma once

#include <raylib.h>
#include <environment/environment.hpp>
#include <agent/agent.hpp>
#include <thread>
namespace sim{

extern const int screenWidth;
extern const int screenHeight;
extern int selected_agent_id;
class GUI{
    private:
        static void guiThreadTask(Environment &env, Agents &agents);
        std::thread * guiThread;
    public:
        GUI(Environment &env, Agents &agents);
        ~GUI();

};

}
