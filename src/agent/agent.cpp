#include <raylib.h>
#include <raymath.h>
#include <thread>
#include <chrono>
#include <mutex>
#include <cmath>

#include <agent/agent.hpp>
#include <simulation/simulation.hpp>


static void agentTask(Agent* agent) {
    auto lastTime = std::chrono::high_resolution_clock::now();


    
    while(!agent->shouldStop) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> elapsed = currentTime - lastTime;
        lastTime = currentTime;
        
        float dt = elapsed.count();

        agent->updateVel(dt);
        agent->updatePos(dt);

        if (agent->pos.x < 0 || agent->pos.x > sim::screenWidth) {
            agent->vel.x = 0;
            agent->pos.x = (agent->pos.x < 0) ? 0.0f : (float)sim::screenWidth;
        }
        if (agent->pos.y < 0 || agent->pos.y > sim::screenHeight) {
            agent->vel.y = 0;
            agent->pos.y = (agent->pos.y < 0) ? 0.0f : (float)sim::screenHeight;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

const float Agent::acc_mag = 1000;
const float Agent::circleRadius = 15.0f;
int Agent::numAgents = 0;
int Agent::nextAgentId = 0;
std::unordered_map<int, Agent*> Agent::id_to_agent;
std::unordered_map<int, std::thread*> Agent::id_to_thread;


Agent::Agent(Vector2 _pos, Color _color) {
    id = nextAgentId++;
    id_to_agent[id] = this;
    pos = _pos;
    color = _color;
    vel = {0, 0};
    acc_dir = {0, 0};
    haveWater = false;
    numAgents++;
    shouldStop=false;
    id_to_thread[id] = new std::thread(agentTask, this);
    TraceLog(LOG_INFO, "Agent Created");
}

Agent::Agent(const Agent& other) {
    id = nextAgentId++;
    id_to_agent[id] = this;
    pos = other.pos;
    vel = other.vel;
    acc_dir = other.acc_dir;
    color = other.color;
    haveWater = other.haveWater;
    numAgents++; 
    id_to_thread[id] = new std::thread(agentTask, this);
}

Agent::~Agent() {
    numAgents--;
    id_to_agent.erase(this->id);
    shouldStop=true;
    id_to_thread[this->id]->join();
    delete id_to_thread[this->id];
    id_to_thread.erase(this->id);
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

void Agent::destructAll() {
    while (!Agent::id_to_agent.empty()) {
        auto it = Agent::id_to_agent.begin();
        delete it->second;
    }
}
