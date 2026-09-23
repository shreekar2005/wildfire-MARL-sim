
#include "raylib.h"
#include <cmath>
#include <algorithm>

#include <simulation/simulation.hpp>
#include <agent/agent.hpp>
#include <environment/environment.hpp>

const int sim::screenWidth = config::screenWidth;
const int sim::screenHeight = config::screenHeight;

const Color sim::agentColor = RAYWHITE;
const Color sim::selectedAgentColor = RED;
const int sim::agentCircleRadius=config::agentCircleRadius;
int sim::selected_agent_id = -1;

void sim::drawEnvironment(Environment &env) {
    env.drawEnvironment();
}

void sim::drawAgents() {
    for(auto &it : Agent::id_to_agent) {
        int id = it.first;
        Agent* agent= it.second;
        float dt = GetFrameTime();
        
        if (agent->id == sim::selected_agent_id) {
            DrawCircleV(agent->pos, sim::agentCircleRadius, sim::selectedAgentColor);
        } else {
            DrawCircleV(agent->pos, sim::agentCircleRadius, sim::agentColor);
        }
        DrawCircleLines(agent->pos.x, agent->pos.y, sim::agentCircleRadius, BLACK);
    }
}
