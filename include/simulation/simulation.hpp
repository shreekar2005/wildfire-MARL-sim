#pragma once

namespace sim{

extern const int screenWidth;
extern const int screenHeight;

extern const Color agentColor;
extern const Color selectedAgentColor;
extern const int agentCircleRadius;
extern int selected_agent_id;

extern const int blockSize;

//draw grass based on terrain functions
void drawGrass();
void drawAgents();

}
