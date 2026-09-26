#include <raylib.h>
#include <raymath.h>
#include <thread>
#include <chrono>
#include <mutex>
#include <cmath>

#include <agent/agent.hpp>
#include <simulation/simulation.hpp>

Agent::Agent(Vector2 pos) {
    this->pos = pos;
    this->vel = {0, 0};
    this->dir = {0, 0};
    this->doesHaveWater = false;
    TraceLog(LOG_INFO, "Agent Created");
}

Agent::~Agent() {
    TraceLog(LOG_INFO, "Agent Deleted");
}

void Agent::updateVel(const float &time, const float &accMag) {
    if(Vector2Length(dir) == 0) return;
    vel.x += time * dir.x * accMag;
    vel.y += time * dir.y * accMag;
}

void Agent::updatePos(const float &time) {
    pos.x += time * vel.x;
    pos.y += time * vel.y;
    
    float damping = std::pow(0.95f, time * 60.0f);
    vel.x *= damping;
    vel.y *= damping;
}

Vector2 Agent::getDir(){return this->dir;}
Vector2 Agent::getVel(){return this->vel;}
Vector2 Agent::getPos(){return this->pos;}

void Agent::setDir(const Vector2 &dir){this->dir = Vector2Normalize(dir);}

///////////////////////////////////////////////////////////////////////////////////////////

Agents::Agents(Environment* env){
    this->env=env;
    this->agentsMap.reserve(1000);
    this->agentAccMag=config::acc_mag;
    this->nextAgentId=0;
    this->numAgents=0;
}

Agents::Agents(Environment* env, int numAgents){
    this->env=env;
    this->agentsMap.reserve(1000);
    this->agentAccMag=config::acc_mag;
    this->nextAgentId=0;
    this->numAgents=0;
    for(int i=0; i<numAgents; i++){
        Vector2 randomPos;
        randomPos.x = (float)GetRandomValue(0, sim::screenWidth);
        randomPos.y = (float)GetRandomValue(0, sim::screenHeight);
        this->createAgent(randomPos);
    }
}

Agents::~Agents(){
    this->deleteAllAgents();
}

int Agents::createAgent(Vector2 pos){
    this->agentsMap[this->nextAgentId++] = new Agent(pos);
    this->numAgents++;
    TraceLog(LOG_INFO, "Agents number : %d", this->numAgents);
    return this->nextAgentId;
}

void Agents::deleteAgent(int agentID){
    if(this->agentsMap[agentID]) delete this->agentsMap[agentID];
    this->agentsMap.erase(agentID);
    this->numAgents--;
    TraceLog(LOG_INFO, "Agents number : %d", this->numAgents);
}

void Agents::deleteAllAgents(){
    while (!this->agentsMap.empty()) {
        auto it = this->agentsMap.begin();
        delete it->second;
        this->agentsMap.erase(it->first);
        this->numAgents--;
    }
    TraceLog(LOG_INFO, "All agents deleted, Agents number : %d", this->numAgents);
}

void Agents::updateState(const float &time){
    for(auto &it : this->agentsMap){
        it.second->updateVel(time, this->agentAccMag);
        it.second->updatePos(time);
    }
}

void Agents::setAgentDir(const int &agentID, const Vector2 &dir){
    this->agentsMap[agentID]->setDir(Vector2Normalize(dir));
}

Vector2 Agents::getAgentDir(const int &agentID){
    return this->agentsMap[agentID]->getDir();
}
Vector2 Agents::getAgentVel(const int &agentID){
    return this->agentsMap[agentID]->getVel();
}
Vector2 Agents::getAgentPos(const int &agentID){
    return this->agentsMap[agentID]->getPos();
}

int Agents::getAgentIdByPos(const Vector2 &pos){
    bool found = false;
    for(auto &it : this->agentsMap) {
        int id = it.first;
        Agent* agent= it.second;
        float distAgentMouse = Vector2Distance(pos, agent->getPos());
        if (distAgentMouse <= this->agentCircleRadius) return it.first;
    }
    return -1;
}

void Agents::draw(){
    float droneImgScale = 0.08f;
    static Texture2D droneTexture = LoadTexture("assets/drone_white.png");
    // static Texture2D droneTexture_BLACK = LoadTexture("assets/drone_black.png");
    // static Texture2D droneTexture_BLUE = LoadTexture("assets/drone_blue.png");
    static Texture2D selectedDroneTexture = LoadTexture("assets/drone_red.png");

    Vector2 droneTexturePos;

    for(auto &it : this->agentsMap) {
        int agentID = it.first;
        Agent* agent= it.second;
        float dt = GetFrameTime();

        Vector2 agentPos = agent->getPos();

		// adding this instead of above for legacy raylib support
		DrawCircleLines(agentPos.x, agentPos.y, this->agentCircleRadius, WHITE);
        if (agentID == sim::selected_agent_id) {
            droneTexturePos.x = agentPos.x - (selectedDroneTexture.width*droneImgScale/2);
            droneTexturePos.y = agentPos.y - (selectedDroneTexture.height*droneImgScale/2);
            DrawTextureEx(selectedDroneTexture, droneTexturePos, 0.0f, droneImgScale, WHITE);

        } else {
            droneTexturePos.x = agentPos.x - (droneTexture.width*droneImgScale/2);
            droneTexturePos.y = agentPos.y - (droneTexture.height*droneImgScale/2);
            DrawTextureEx(droneTexture, droneTexturePos, 0.0f, droneImgScale, WHITE);
        }
    }
}
