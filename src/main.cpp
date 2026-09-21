#include <iostream>
#include <vector>
#include <unordered_map>
#include "raylib.h"
#include "raymath.h"

class Agent {
    public:
        int id;
        Vector2 pos;
        Vector2 vel;
        Vector2 acc;
        Color color;
        bool haveWater;

        static const float circleRadius;
        static int numAgents;
        static int nextAgentId;
        static std::unordered_map<int, Agent*> id_to_agents;

        Agent(Vector2 _pos = {10, 10}, Color _color = RED) {
            id = nextAgentId++;
            id_to_agents[id] = this;
            pos = _pos;
            color = _color;
            vel = {0, 0};
            acc = {0, 0};
            haveWater = false;
            numAgents++;
        }
        
        Agent(const Agent& other) {
            id = nextAgentId++;
            id_to_agents[id] = this;
            pos = other.pos;
            vel = other.vel;
            acc = other.acc;
            color = other.color;
            haveWater = other.haveWater;
            numAgents++; 
        }
        
        ~Agent() {
            numAgents--;
            id_to_agents.erase(this->id);
        }

        void updateVel(float time) {
            vel.x += time * acc.x;
            vel.y += time * acc.y;
        }

        void updatePos(float time) {
            pos.x += time * vel.x;
            pos.y += time * vel.y;
            
            vel.x *= 0.95f;
            vel.y *= 0.95f;
        }
};

const float Agent::circleRadius = 15.0f;
int Agent::numAgents = 0;
int Agent::nextAgentId = 0;
std::unordered_map<int, Agent*> Agent::id_to_agents;

int main(void)
{
    const int screenWidth = 1500;
    const int screenHeight = 900;

    InitWindow(screenWidth, screenHeight, "wildfire_marl_simulation");
    SetTargetFPS(60);
    
    Vector2 mousePos;
    std::vector<Agent*> agents;
    
    agents.reserve(1000);
    Agent::id_to_agents.reserve(1000);

    int selected_agent_id = -1; 

    while (!WindowShouldClose()) 
    {
        if (IsKeyPressed(KEY_C)) {
            mousePos = GetMousePosition();
            agents.push_back(new Agent(mousePos));
            TraceLog(LOG_INFO, "Agent Created");
        }

        if (IsMouseButtonPressed(0)) {
            mousePos = GetMousePosition();
            bool found = false;
            for (auto* agent : agents) {
                float distAgentMouse = Vector2Distance(mousePos, agent->pos);
                if (distAgentMouse <= Agent::circleRadius) {
                    selected_agent_id = agent->id;
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
                if (IsKeyDown(KEY_W)) agent->vel.y -= 50;
                if (IsKeyDown(KEY_S)) agent->vel.y += 50;
                if (IsKeyDown(KEY_A)) agent->vel.x -= 50;
                if (IsKeyDown(KEY_D)) agent->vel.x += 50;
            }
        }
        
        BeginDrawing();
            ClearBackground(RAYWHITE);

            for (auto* agent : agents) {
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

    for (auto* agent : agents) {
        delete agent;
    }
    agents.clear();

    CloseWindow();
    return 0;
}