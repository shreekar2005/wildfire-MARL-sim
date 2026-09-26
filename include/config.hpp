#pragma once

namespace config {

    // simulation configs
    constexpr int screenWidth = 1500;
    constexpr int screenHeight = 900;
    constexpr int blockSize = 10;
    
    
    constexpr float simSpeedFactor = 1;
    
    // agent configs
    constexpr int numAgentsOnStart = 5;
    constexpr int agentLocalPerceptionWidth = 15;
    constexpr float agentAccMag = 1000;
    
    // environment configs
	constexpr unsigned int envSeed = 47; //if config seed is 0 it will be random according to time
    constexpr int envBlockSize = 10;

	constexpr float waterGrassFraction = 0.2; // (more threshold = more water)
	constexpr float expectedCellCatchFireTimeSec = 1; // less value, more fire spread
	constexpr float cellBurnOutTimeSec = 2; // more value, more fire spread

}
