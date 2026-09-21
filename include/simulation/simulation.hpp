#pragma once

namespace sim{

extern const int blockSize;
extern const int screenWidth;
extern const int screenHeight;
extern int selected_agent_id;

//draw grass based on terrain functions
void drawGrass();
void drawAgents();

}
