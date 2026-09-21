#include<agent/agent.hpp>
#include<raylib.h>

const float Agent::circleRadius = 15.0f;
int Agent::numAgents = 0;
int Agent::nextAgentId = 0;
std::unordered_map<int, Agent*> Agent::id_to_agents;

Agent::Agent(Vector2 _pos, Color _color) {
    id = nextAgentId++;
    id_to_agents[id] = this;
    pos = _pos;
    color = _color;
    vel = {0, 0};
    acc = {0, 0};
    haveWater = false;
    numAgents++;
}

Agent::Agent(const Agent& other) {
    id = nextAgentId++;
    id_to_agents[id] = this;
    pos = other.pos;
    vel = other.vel;
    acc = other.acc;
    color = other.color;
    haveWater = other.haveWater;
    numAgents++; 
}

Agent::~Agent() {
    numAgents--;
    id_to_agents.erase(this->id);
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
    for(auto &it : Agent::id_to_agents) {
        delete it.second;
    }
}