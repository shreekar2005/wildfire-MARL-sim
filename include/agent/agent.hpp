#pragma once

#include <raylib.h>
#include <thread>
#include <unordered_map>
#include <config.hpp>
#include <environment/environment.hpp>

class Agent {
    private:
        Vector2 pos; // agent's current position (float x,float y)
        Vector2 dir; // used for only acceleration direction (it will be normalized when to update velocity)
        Vector2 vel; // agent's current velocity (vx,vy)
        bool doesHaveWater; // does agent have water or not

    public:
        void updateVel(const float &time, const float &accMag);
        void updatePos(const float &time);
        
        Agent(Vector2 pos = {10, 10});
        ~Agent();

        Vector2 getDir();
        Vector2 getVel();
        Vector2 getPos();

        void setDir(const Vector2 &dir);
        // void throwWater(); // dummy
        // void pickWater(); // dummy

};


class Agents{
    private:
        std::unordered_map <int, Agent*> agentsMap;
        int nextAgentId=0;
        int numAgents=0;
        
        int agentLocalPerceptionWidth = config::agentLocalPerceptionWidth;
        float agentAccMag = config::agentAccMag;
        float agentCircleRadius = 15;

        Environment* env;

    public:
        Agents(Environment* env);
        Agents(Environment* env, int numAgents); // will also spawn random numAgents agents

        ~Agents();

        int createAgent(Vector2 pos); // returns id of created agent
        void deleteAgent(int agentID);
        void deleteAllAgents();

        void updateState(const float &time);
        void setAgentDir(const int &agentID, const Vector2 &dir);

        Vector2 getAgentDir(const int &agentID);
        Vector2 getAgentVel(const int &agentID);
        Vector2 getAgentPos(const int &agentID);
        int getAgentIdByPos(const Vector2 &pos);

        void draw();

};
