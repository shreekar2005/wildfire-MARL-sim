
#include "raylib.h"
#include <cmath>
#include <algorithm>

#include <simulation/simulation.hpp>
#include <agent/agent.hpp>
#include <environment/environment.hpp>
#include <raylib.h>
#include <raymath.h>
#include <functional> // Required for std::ref

const int sim::screenWidth = config::screenWidth;
const int sim::screenHeight = config::screenHeight;

const int sim::agentCircleRadius=config::agentCircleRadius;
int sim::selected_agent_id = -1;

sim::GUI::GUI(Environment &env){
    guiThread = new std::thread(guiThreadTask, std::ref(env));
    TraceLog(LOG_INFO, "GUI Instance Created!!!");
}

sim::GUI::~GUI(){
    if(guiThread->joinable()) guiThread->join();
    delete guiThread;
    TraceLog(LOG_INFO, "GUI Instance Deleted!!!");
}

void sim::GUI::guiThreadTask(Environment &env){
    InitWindow(sim::screenWidth, sim::screenHeight, "wildfire_marl_simulation");
    SetTargetFPS(60);
    
    Vector2 mousePos;

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
		if (IsMouseButtonPressed(0) && (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL))) {
            mousePos = GetMousePosition();
			int cellCol = mousePos.x/env::blocksize;
			int cellRow = mousePos.y/env::blocksize;
			env.environmentGrid[cellCol][cellRow].cell_type = env::BURNING_GRASS_CELL;
			// env.spawnFire(mousePos);
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
			sim::drawEnvironment(env);
            sim::drawAgents();
            // Draw FPS in the box in top
            DrawRectangle(0, 0, 100, 30, SKYBLUE);
            DrawFPS(10, 10);
        EndDrawing();
    }
    Agent::destructAll();
    CloseWindow();
}

void sim::drawEnvironment(Environment &env) {
    env.drawEnvironment();
}

void sim::drawAgents() {
    float droneImgScale = 0.08f;
    static Texture2D droneTexture = LoadTexture("assets/drone_white.png");
    // static Texture2D droneTexture_BLACK = LoadTexture("assets/drone_black.png");
    // static Texture2D droneTexture_BLUE = LoadTexture("assets/drone_blue.png");
    static Texture2D selectedDroneTexture = LoadTexture("assets/drone_red.png");

    Vector2 droneTexturePos;

    for(auto &it : Agent::id_to_agent) {
        int id = it.first;
        Agent* agent= it.second;
        float dt = GetFrameTime();
        
        // DrawCircleLines(agent->pos.x, agent->pos.y, sim::agentCircleRadius, WHITE);
       // DrawCircleLinesEx(agent->pos, sim::agentCircleRadius, 0.3f, WHITE);

		// adding this instead of above for legacy raylib support
		DrawCircleLines(agent->pos.x, agent->pos.y, sim::agentCircleRadius, WHITE);
        if (agent->id == sim::selected_agent_id) {
            // DrawCircleV(agent->pos, sim::agentCircleRadius, sim::selectedAgentColor);
            droneTexturePos.x = agent->pos.x - (selectedDroneTexture.width*droneImgScale/2);
            droneTexturePos.y = agent->pos.y - (selectedDroneTexture.height*droneImgScale/2);
            DrawTextureEx(selectedDroneTexture, droneTexturePos, 0.0f, droneImgScale, WHITE);

        } else {
            // DrawCircleV(agent->pos, sim::agentCircleRadius, sim::agentColor);
            droneTexturePos.x = agent->pos.x - (droneTexture.width*droneImgScale/2);
            droneTexturePos.y = agent->pos.y - (droneTexture.height*droneImgScale/2);
            DrawTextureEx(droneTexture, droneTexturePos, 0.0f, droneImgScale, WHITE);

        }

        
        
    }
}
