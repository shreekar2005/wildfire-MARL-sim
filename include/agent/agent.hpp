#pragma once

#include <raylib.h>
#include <thread>
#include <unordered_map>
#include <config.hpp>

class Agent {
    private:
        Vector2 vel; // agent's current velocity (vx,vy)
        bool haveWater; // does agent have water or not
        int agentLocalPerceptionWidth = config::agentLocalPerceptionWidth; // agent will be observing 15x15 cells for its local perception 
        const static float acc_mag; // constant
        static int nextAgentId; // used for assigning agent id

        /// @brief update the velocity assuming constant acceleration for given time
        /// @param time time for which we have to change velocity
        void updateVel(float time);

        /// @brief update the position of agent assuming constant velocity. velocity will decay after change in position
        /// @param time time for which we have to change position
        void updatePos(float time);

    public:
        int id; // agent id
        Vector2 pos; // agent's current position (x,y)
        Vector2 acc_dir; // used for only acceleration direction (it will be normalized when to update velocity)
        
        static std::unordered_map<int, Agent*> id_to_agent; // map from agent id to agent object pointer
        
        /// @brief agent constructor
        /// @param _pos initial position on creating agent
        Agent(Vector2 _pos = {10, 10});
        
        // copy constructor
        Agent(const Agent& other);
        
        ~Agent();

		void throwWater(); // dummy
        void pickWater(); // dummy

        static void updateState(float time);

        // Will delete all agents and their currusponding threads
        static void destructAll();

};


class Agents{
    // TO DO (some place code has been updated accordingly, please complete this class as early as possible)
};
