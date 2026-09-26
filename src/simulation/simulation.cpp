
#include "raylib.h"
#include <cmath>
#include <algorithm>

#include <simulation/simulation.hpp>
#include <raylib.h>
#include <raymath.h>
#include <functional> // Required for std::ref

const int sim::screenWidth = config::screenWidth;
const int sim::screenHeight = config::screenHeight;

int sim::selected_agent_id = -1;

sim::GUI::GUI(Environment &env, Agents &agents){
    guiThread = new std::thread(guiThreadTask, std::ref(env), std::ref(agents));
    TraceLog(LOG_INFO, "GUI Instance Created!!!");
}

sim::GUI::~GUI(){
    if(guiThread->joinable()) guiThread->join();
    delete guiThread;
    TraceLog(LOG_INFO, "GUI Instance Deleted!!!");
}

void sim::GUI::guiThreadTask(Environment &env, Agents &agents){
    InitWindow(sim::screenWidth, sim::screenHeight, "wildfire_marl_simulation");
    SetTargetFPS(60);
    
    Vector2 mousePos;

    while (!WindowShouldClose()) 
    {
        // create Agent
        if (IsKeyPressed(KEY_C)) {
            mousePos = GetMousePosition();
            agents.createAgent(mousePos);
        }
        if (IsMouseButtonPressed(0)) {
            mousePos = GetMousePosition();
            sim::selected_agent_id = agents.getAgentIdByPos(mousePos);
        }
		// spawn fire
		if (IsMouseButtonPressed(0) && (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL))) {
            mousePos = GetMousePosition();
			int cellCol = mousePos.x/env.getBlockSize();
			int cellRow = mousePos.y/env.getBlockSize();
            env.setFire(cellCol, cellRow);
        }


		// set agent movement direction
        if (sim::selected_agent_id != -1) {
            Vector2 wasdDir;
            if (IsKeyDown(KEY_W) && !IsKeyDown(KEY_S)) wasdDir.y = -1;
            else if (IsKeyDown(KEY_S) && !IsKeyDown(KEY_W)) wasdDir.y = 1;
            else wasdDir.y = 0;
            if (IsKeyDown(KEY_A) && !IsKeyDown(KEY_D)) wasdDir.x = -1;
            else if (IsKeyDown(KEY_D) && !IsKeyDown(KEY_A)) wasdDir.x = 1;
            else wasdDir.x = 0;
            agents.setAgentDir(sim::selected_agent_id, wasdDir);
        }
        
        BeginDrawing();
            ClearBackground(RAYWHITE);
			env.draw();
            agents.draw();
            // Draw FPS in the box in top
            DrawRectangle(0, 0, 100, 30, SKYBLUE);
            DrawFPS(10, 10);
        EndDrawing();
    }
    CloseWindow();
}
