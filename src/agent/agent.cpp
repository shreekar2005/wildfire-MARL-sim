#include<raylib.h>
#include<thread>

#include<agent/agent.hpp>
#include<simulation/simulation.hpp>

static void agentTask(Agent* agent){
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

const float Agent::circleRadius = 15.0f;
int Agent::numAgents = 0;
int Agent::nextAgentId = 0;
std::unordered_map<int, Agent*> Agent::id_to_agent;
std::unordered_map<int, Agent*> Agent::id_to_thread;

Agent::Agent(Vector2 _pos, Color _color) {
    id = nextAgentId++;
    id_to_agent[id] = this;
    pos = _pos;
    color = _color;
    vel = {0, 0};
    acc = {0, 0};
    haveWater = false;
    numAgents++;
    new std::thread(agentTask, this);
}

Agent::Agent(const Agent& other) {
    id = nextAgentId++;
    id_to_agent[id] = this;
    pos = other.pos;
    vel = other.vel;
    acc = other.acc;
    color = other.color;
    haveWater = other.haveWater;
    numAgents++; 
}

Agent::~Agent() {
    numAgents--;
    id_to_agent.erase(this->id);
}

void Agent::updateVel(float time) {
    vel.x += time * acc.x;
    vel.y += time * acc.y;
}

void Agent::updatePos(float time) {
    pos.x += time * vel.x;
    pos.y += time * vel.y;
    
    vel.x *= 0.95f;
    vel.y *= 0.95f;
}

void Agent::destructAll(){
    for(auto &it : Agent::id_to_agent) {
        delete it.second;
    }
}