#include <raylib.h>
#include <raymath.h>
#include <thread>
#include <chrono>
#include <mutex>
#include <cmath>

#include <agent/agent.hpp>
#include <simulation/simulation.hpp>

const float Agent::acc_mag = config::acc_mag;
int Agent::nextAgentId = 0;
std::unordered_map<int, Agent*> Agent::id_to_agent;

Agent::Agent(Vector2 _pos) {
    id = nextAgentId++;
    id_to_agent[id] = this;
    pos = _pos;
    vel = {0, 0};
    acc_dir = {0, 0};
    haveWater = false;
    TraceLog(LOG_INFO, "Agent Created");
}

Agent::Agent(const Agent& other) {
    id = nextAgentId++;
    id_to_agent[id] = this;
    pos = other.pos;
    vel = other.vel;
    acc_dir = other.acc_dir;
    haveWater = other.haveWater;
}

Agent::~Agent() {
    id_to_agent.erase(this->id);
    TraceLog(LOG_INFO, "Agent Deleted");
}

void Agent::updateVel(float time) {
    float acc_dir_mag = Vector2Length(acc_dir);
    if(acc_dir_mag == 0) return;
    vel.x += time * (acc_dir.x / acc_dir_mag) * acc_mag;
    vel.y += time * (acc_dir.y / acc_dir_mag) * acc_mag;
}

void Agent::updatePos(float time) {
    pos.x += time * vel.x;
    pos.y += time * vel.y;
    
    float damping = std::pow(0.95f, time * 60.0f);
    vel.x *= damping;
    vel.y *= damping;
}


void Agent::updateState(float time){
    for(auto &it : Agent::id_to_agent) {
        Agent* agent= it.second;
        agent->updatePos(time);
        agent->updateVel(time);

        if (agent->pos.x < 0 || agent->pos.x > sim::screenWidth) {
            agent->vel.x = 0;
            agent->pos.x = (agent->pos.x < 0) ? 0.0f : (float)sim::screenWidth;
        }
        if (agent->pos.y < 0 || agent->pos.y > sim::screenHeight) {
            agent->vel.y = 0;
            agent->pos.y = (agent->pos.y < 0) ? 0.0f : (float)sim::screenHeight;
        }
    }
}

void Agent::destructAll() {
    while (!Agent::id_to_agent.empty()) {
        auto it = Agent::id_to_agent.begin();
        delete it->second;
    }
}
