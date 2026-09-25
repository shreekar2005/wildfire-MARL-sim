#pragma once
#include "config.hpp"
#include <queue>
#include <raylib.h>
#include <thread>
#include <unordered_map>
#include <vector>
#include <chrono>
#include <iostream>
#include <simulation/simulation.hpp>
#include <types.hpp>

namespace env
{

enum CELL_TYPE{
	WATER_CELL,
	GRASS_CELL,
	BURNED_GRASS_CELL,
	BURNING_GRASS_CELL
};

const int gridWidth = config::screenWidth / config::blockSize;
const int gridHeight = config::screenHeight / config::blockSize;
const int blocksize = config::blockSize;
const float TerrainWaterThreshold = config::TerrainWaterThreshold; // less than 0.25 matlab water

class EnvironmentCell{
	public:
		float flamability;
		env::CELL_TYPE cell_type;
		// std::chrono::steady_clock::time_point expiryBurningTime;
		simTick expiryBurningTime;

		EnvironmentCell(env::CELL_TYPE ctype, float flamability);
		EnvironmentCell();
};
class Environment{
	public:
		std::vector<std::vector<EnvironmentCell>> environmentGrid;
		std::vector<std::vector<float>> perlinNoiseGrid;
		std::vector<std::thread *> fireThreads;
		bool envShouldStop;

		Environment();
		~Environment();
		void generateEnvironmentTerrain();
		void drawEnvironment();
		void generatePerlinNoiseMap();
		void spawnFire(Vector2 mousePos);
		void spreadFireTask(std::pair<int, int> fireStartCell);
};

};