# ifndef __agent_hpp__
# define __agent_hpp__

#include<raylib.h>
#include<unordered_map>

class Agent {
    public:
        int id;
        Vector2 pos;
        Vector2 vel;
        Vector2 acc;
        Color color;
        bool haveWater;

        bool shouldStop;

        static const float circleRadius;
        static int numAgents;
        static int nextAgentId;
        static std::unordered_map<int, Agent*> id_to_agent;
        static std::unordered_map<int, std::thread*> id_to_thread;

        Agent(Vector2 _pos = {10, 10}, Color _color = RED);
        
        Agent(const Agent& other);
        
        ~Agent();

        void updateVel(float time);

        void updatePos(float time);

        static void destructAll();
};

# endif