#include <iostream>
#include <vector>
#include <unordered_map>
#include <thread>

#include "config.hpp"
#include "raylib.h"
#include "raymath.h"

#include <agent/agent.hpp>
#include <simulation/simulation.hpp>
#include <environment/environment.hpp>


int main(void)
{

    InitWindow(sim::screenWidth, sim::screenHeight, "wildfire_marl_simulation");
    SetTargetFPS(60);
    
    Agent::id_to_agent.reserve(1000);
    Agent::id_to_thread.reserve(1000);
    
    Vector2 mousePos;

    Environment environment;
	environment.GenerateEnvironmentTerrain();
    while (!WindowShouldClose()) 
    {
        if (IsKeyPressed(KEY_C)) {
            mousePos = GetMousePosition();
            new Agent(mousePos);
        }

		//agent finding logic
        if (IsMouseButtonPressed(0)) {
            mousePos = GetMousePosition();
            bool found = false;
            for(auto &it : Agent::id_to_agent) {
                int id = it.first;
                Agent* agent= it.second;
                float distAgentMouse = Vector2Distance(mousePos, agent->pos);
                if (distAgentMouse <= sim::agentCircleRadius) {
                    sim::selected_agent_id = id;
                    found = true;
                    break;
                }
            }
            if (!found) sim::selected_agent_id = -1;
        }
		//fire spawn
		if (IsMouseButtonPressed(1)) {
            mousePos = GetMousePosition();
			environment.SpawnFire(mousePos);
        }


		// agent movement logic

        if (sim::selected_agent_id != -1) {
            auto it = Agent::id_to_agent.find(sim::selected_agent_id);
            if (it != Agent::id_to_agent.end()) {
                Agent* agent = it->second;
                if (IsKeyDown(KEY_W) && !IsKeyDown(KEY_S)) agent->acc_dir.y = -1;
                else if (IsKeyDown(KEY_S) && !IsKeyDown(KEY_W)) agent->acc_dir.y = 1;
                else agent->acc_dir.y = 0;
                if (IsKeyDown(KEY_A) && !IsKeyDown(KEY_D)) agent->acc_dir.x = -1;
                else if (IsKeyDown(KEY_D) && !IsKeyDown(KEY_A)) agent->acc_dir.x = 1;
                else agent->acc_dir.x = 0;
            }
        }
        
        BeginDrawing();
            ClearBackground(RAYWHITE);
			environment.DrawEnvironment();
            sim::drawAgents();
            DrawFPS(10, 10);
        EndDrawing();
    }
    Agent::destructAll();
    CloseWindow();
    return 0;
}
