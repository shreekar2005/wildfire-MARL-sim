#pragma once

#include <raylib.h>
#include <environment/environment.hpp>
#include <agent/agent.hpp>
#include <thread>
namespace sim{

extern const int screenWidth;
extern const int screenHeight;

extern const Color agentColor;
extern const Color selectedAgentColor;
extern const int agentCircleRadius;
extern int selected_agent_id;

extern const int blockSize;

//draw environment based on terrain functions
void drawEnvironment(Environment &env);
void drawAgents(Agents &agents);
class GUI{
    private:
        static void guiThreadTask(Environment &env, Agents &agents);
        std::thread * guiThread;
    public:
        GUI(Environment &env, Agents &agents);
        ~GUI();

};

}
