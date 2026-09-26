#include <iostream>
#include <vector>
#include <unordered_map>
#include <thread>
#include <chrono>

#include "config.hpp"
#include "raylib.h"
#include "raymath.h"

#include <agent/agent.hpp>
#include <simulation/simulation.hpp>
#include <environment/environment.hpp>

// TODO : 

int main(void)
{
    // Environment env; // default
    Environment env(10, 0.3); // block size = 10 and water fraction = 0.3
    // Agents agents(env); // default 
    Agents agents(&env, 5); // 5 agents will be spawned

	// GUI THREAD
    sim::GUI guiInstance(env, agents);// can only create one instance

    auto lastTime = std::chrono::high_resolution_clock::now();
    while(true)
    {
        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> elapsed = currentTime - lastTime;
        lastTime = currentTime;
        float dt = elapsed.count(); // dt will be in second
        // TraceLog(LOG_INFO, "%f", dt);
        dt = dt*config::simSpeedFactor;
		env.updateState(dt);
        agents.updateState(dt);

        std::this_thread::sleep_for(std::chrono::microseconds(1));
    }
    return 0;
}
