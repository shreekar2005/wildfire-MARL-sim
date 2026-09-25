#pragma once

#include <raylib.h>
#include <environment/environment.hpp>
#include <thread>
#include <types.hpp>

namespace sim{

extern const int screenWidth;
extern const int screenHeight;

extern const Color agentColor;
extern const Color selectedAgentColor;
extern const int agentCircleRadius;
extern int selected_agent_id;

extern const int blockSize;

//draw environment based on terrain functions
void drawEnvironment(env::Environment &env);
void drawAgents();

class Time{
    private:
        static simTick currentSimTick;
        static std::thread* ticker;
        static bool tickerShouldStop;
        static bool isTimeObjectCreated;
        static Time* timeObject;

        static void updateTicksTask();
        Time(); // keeping them private
        ~Time(); 
    public:
        static int initTime();
        static int finTime();

        // get current simulation tick
        static simTick getCurrentSimTick();

        // sleep for timeTick ticks
        static void sleep(simTick timeTick);
};

}
