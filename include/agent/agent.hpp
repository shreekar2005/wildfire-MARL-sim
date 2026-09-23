#pragma once

#include <raylib.h>
#include <thread>
#include <unordered_map>
#include <config.hpp>

class Agent {
    private:
    public:
        int id; // agent id
        bool haveWater; // does agent have water or not
        int agentLocalPerceptionWidth = config::agentLocalPerceptionWidth; // agent will be observing 15x15 cells for its local perception 
        Vector2 vel; // agent's current velocity (vx,vy)
        Vector2 pos; // agent's current position (x,y)
        Vector2 acc_dir; // used for only acceleration direction (it will be normalized when to update velocity)
        bool shouldStop; // agent destructor is called or not (used for terminating agent's thread)
        
        static int numAgents; // total number of agents
        const static float acc_mag; // constant
        static int nextAgentId; // used for assigning agent id
        static std::unordered_map<int, Agent*> id_to_agent; // map from agent id to agent object pointer
        static std::unordered_map<int, std::thread*> id_to_thread; // map from agent id to agent thread pointer
        
        /// @brief agent constructor
        /// @param _pos initial position on creating agent
        Agent(Vector2 _pos = {10, 10});
        
        // copy constructor
        Agent(const Agent& other);
        
        ~Agent();

        /// @brief update the velocity assuming constant acceleration for given time
        /// @param time time for which we have to change velocity
        void updateVel(float time);

        /// @brief update the position of agent assuming constant velocity. velocity will decay after change in position
        /// @param time time for which we have to change position
        void updatePos(float time);
		void throwWater();


        // Will delete all agents and their currusponding threads
        static void destructAll();
};
