#pragma once

namespace config {
    // simulation configs
    constexpr int screenWidth = 1500;
    constexpr int screenHeight = 900;
    constexpr int blockSize = 10;
    constexpr int agentCircleRadius = 15;

    // agent configs
    constexpr int agentLocalPerceptionWidth = 15;
    constexpr float acc_mag = 1000;
	constexpr float TerrainWaterThreshold = 0.25; // (more threshold = more water)
	constexpr int GrassBurnTimeMs = 8000;//ms (time taken to burn the grass cell)
	constexpr int expectedCatchFireTimeMs = 100; //ms (which is inversly proportional to rate of fire speed)
												 
	// dhruv single thread fire variables
	constexpr float updateTime = 0.5;
	constexpr float GrassBurnTime = 10*updateTime;

		//if config seed is 0 it will be random according to time
	constexpr unsigned int seed = 2;
    
}
