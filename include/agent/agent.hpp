#pragma once

#include <raylib.h>
#include <thread>
#include <unordered_map>

class Agent {
    private:
    public:
        bool haveWater;
        Vector2 vel;
        int id;
        Vector2 pos;
        Vector2 acc_dir; // this will be converted to unit vector only while updating velocity
        const static float acc_mag;
        bool shouldStop;

        static int numAgents;
        static int nextAgentId;
        static std::unordered_map<int, Agent*> id_to_agent;
        static std::unordered_map<int, std::thread*> id_to_thread;

        Agent(Vector2 _pos = {10, 10});
        
        Agent(const Agent& other);
        
        ~Agent();

        void updateVel(float time);

        void updatePos(float time);

        static void destructAll();
};