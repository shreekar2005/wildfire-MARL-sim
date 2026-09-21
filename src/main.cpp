#include <iostream>
#include <vector>
#include <unordered_map>
#include <thread>

#include "raylib.h"
#include "raymath.h"

#include <agent/agent.hpp>
#include <simulation/simulation.hpp>


int main(void)
{

    InitWindow(screenWidth, screenHeight, "wildfire_marl_simulation");
    SetTargetFPS(60);
    
    Agent::id_to_agent.reserve(1000);
    Agent::id_to_thread.reserve(1000);
    
    Vector2 mousePos;

    int selected_agent_id = -1; 

    while (!WindowShouldClose()) 
    {
        if (IsKeyPressed(KEY_C)) {
            mousePos = GetMousePosition();
            new Agent(mousePos);
            TraceLog(LOG_INFO, "Agent Created");
        }

        if (IsMouseButtonPressed(0)) {
            mousePos = GetMousePosition();
            bool found = false;
            for(auto &it : Agent::id_to_agent) {
                int id = it.first;
                Agent* agent= it.second;
                float distAgentMouse = Vector2Distance(mousePos, agent->pos);
                if (distAgentMouse <= Agent::circleRadius) {
                    selected_agent_id = id;
                    found = true;
                    break;
                }
            }
            if (!found) selected_agent_id = -1;
        }

        if (selected_agent_id != -1) {
            auto it = Agent::id_to_agent.find(selected_agent_id);
            if (it != Agent::id_to_agent.end()) {
                Agent* agent = it->second;
                if (IsKeyDown(KEY_W)) agent->acc.y = -1000;
                else if (IsKeyDown(KEY_S)) agent->acc.y = 1000;
                else agent->acc.y = 0;
                if (IsKeyDown(KEY_A)) agent->acc.x = -1000;
                else if (IsKeyDown(KEY_D)) agent->acc.x = 1000;
                else agent->acc.x = 0;
            }
        }
        
        BeginDrawing();
            ClearBackground(RAYWHITE);
            // Agent threads will draw the agents
            DrawText(TextFormat("FPS: %i", GetFPS()), 10, 10, 20, DARKGRAY);
        EndDrawing();
    }
    Agent::destructAll();

    CloseWindow();
    return 0;
}