#pragma once

namespace config {
    // simulation configs
    constexpr int screenWidth = 1500;
    constexpr int screenHeight = 900;
    constexpr int blockSize = 10;
    constexpr int agentCircleRadius = 10;

    // agent configs
    constexpr int agentLocalPerceptionWidth = 15;
    constexpr float acc_mag = 1000;
	constexpr float TerrainWaterThreshold = 0.25;
	constexpr float GrassBurnTime = 10;//seconds
	constexpr int FireThreadWaitTime = 100; //ms

		//if config seed is 0 it will be random according to time
	constexpr unsigned int seed = 2;
    
}
