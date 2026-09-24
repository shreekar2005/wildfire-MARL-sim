
#include "raylib.h"
#include <cmath>
#include <algorithm>

#include <simulation/simulation.hpp>
#include <agent/agent.hpp>
#include <environment/environment.hpp>

const int sim::screenWidth = config::screenWidth;
const int sim::screenHeight = config::screenHeight;

const int sim::agentCircleRadius=config::agentCircleRadius;
int sim::selected_agent_id = -1;

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
        DrawCircleLinesEx(agent->pos, sim::agentCircleRadius, 0.3f, WHITE);

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
