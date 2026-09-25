#pragma once

namespace config {

    //if config seed is 0 it will be random according to time
	constexpr unsigned int seed = 2;

    // simulation configs
    constexpr int screenWidth = 1500;
    constexpr int screenHeight = 900;
    constexpr int blockSize = 10;
    
    
    constexpr float simSpeedFactor = 1;
    
    // agent configs
    constexpr int agentLocalPerceptionWidth = 15;
    constexpr float acc_mag = 1000;
    
    // environment configs
	constexpr float TerrainWaterThreshold = 0.25; // (more threshold = more water)
	constexpr float expectedCatchFireTimeSec = 1; // less value, more fire spread
	constexpr float GrassBurnOutTimeSec = 2; // more value, more fire spread

}
