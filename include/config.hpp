#pragma once

namespace config {
    // simulation configs
    //if config seed is 0 it will be random according to time
	constexpr unsigned int seed = 2;

    constexpr int screenWidth = 1500;
    constexpr int screenHeight = 900;
    constexpr int blockSize = 5;
    constexpr int agentCircleRadius = 15;

    constexpr int simulationSpeedFactor = 100;

    // agent configs
    constexpr int agentLocalPerceptionWidth = 15;
    constexpr float acc_mag = 1000;
	constexpr float TerrainWaterThreshold = 0.25; // (more threshold = more water)

	constexpr int GrassBurnTimeMs = 2000;//ms (time taken to burn the grass cell)
	constexpr int expectedCatchFireTimeMs = 1000; //ms (which is inversly proportional to rate of fire speed)

    
}
