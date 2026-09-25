
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

// initializing Time static variables
bool sim::Time::tickerShouldStop = false;
bool sim::Time::isTimeObjectCreated = false;
std::thread* sim::Time::ticker = nullptr;
uint64_t sim::Time::currentSimTick = 0;
sim::Time* sim::Time::timeObject = nullptr;


// TIME PRIVATE FUNCTIONS

sim::Time::Time(){
    tickerShouldStop=false;
    ticker = new std::thread(sim::Time::updateTicksTask);
    isTimeObjectCreated = true;

}
sim::Time::~Time(){
    if(ticker->joinable()) ticker->join();
    delete ticker;
}

void sim::Time::updateTicksTask(){
    while(!tickerShouldStop){
        currentSimTick++;
        // TraceLog(LOG_INFO, "%ld", currentSimTick);
        std::this_thread::sleep_for(std::chrono::microseconds((int)(1000/config::simulationSpeedFactor)));
    }
}

// PUBLICLY FACED TIME FUNCTIONS 

int sim::Time::initTime(){
    if(isTimeObjectCreated==true){
        TraceLog(LOG_WARNING, "Cannot create Time Object, time Object already created");
        return -1;
    }
    tickerShouldStop=false;
    timeObject = new Time();
    TraceLog(LOG_INFO, "Time Object Created!");
    return 0;
}

int sim::Time::finTime(){
    if(timeObject==nullptr){
        TraceLog(LOG_WARNING, "Cannot delete Time Object, time Object not created");   
        return -1;
    }
    tickerShouldStop = true;
    delete timeObject;
    TraceLog(LOG_INFO, "Time Object Deleted!");
    return 0;
}

uint64_t sim::Time::getCurrentSimTick(){
    return currentSimTick;
}

void sim::Time::sleep(simTick timeTick){
    uint64_t currentTick = getCurrentSimTick();
    uint64_t goalTick = currentTick+timeTick;
    while(goalTick > currentTick){
        std::this_thread::yield();
        currentTick = getCurrentSimTick();
    }
}





void sim::drawEnvironment(env::Environment &env) {
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
