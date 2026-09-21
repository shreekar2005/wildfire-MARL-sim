#include <iostream>
#include <vector>
#include <unordered_map>

#include "raylib.h"
#include "raymath.h"

#include <agent/agent.hpp>

int main(void)
{
    const int screenWidth = 1500;
    const int screenHeight = 900;

    InitWindow(screenWidth, screenHeight, "wildfire_marl_simulation");
    SetTargetFPS(60);
    
    Agent::id_to_agents.reserve(1000);
    
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
            for(auto &it : Agent::id_to_agents) {
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
            auto it = Agent::id_to_agents.find(selected_agent_id);
            if (it != Agent::id_to_agents.end()) {
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

            for(auto &it : Agent::id_to_agents) {
                int id = it.first;
                Agent* agent= it.second;
                float dt = GetFrameTime();
                agent->updateVel(dt);
                agent->updatePos(dt);

                if (agent->pos.x < 0 || agent->pos.x > screenWidth) {
                    agent->vel.x = 0;
                    agent->pos.x = (agent->pos.x < 0) ? 0.0f : (float)screenWidth;
                }
                if (agent->pos.y < 0 || agent->pos.y > screenHeight) {
                    agent->vel.y = 0;
                    agent->pos.y = (agent->pos.y < 0) ? 0.0f : (float)screenHeight;
                }
                
                if (agent->id == selected_agent_id) {
                    DrawCircleV(agent->pos, Agent::circleRadius, BLUE);
                } else {
                    DrawCircleV(agent->pos, Agent::circleRadius, agent->color);
                }
            }

            DrawText(TextFormat("FPS: %i", GetFPS()), 10, 10, 20, DARKGRAY);
        EndDrawing();
    }
    Agent::destructAll();

    CloseWindow();
    return 0;
}