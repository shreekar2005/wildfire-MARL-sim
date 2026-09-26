#pragma once
#include "config.hpp"
#include <queue>
#include <raylib.h>
#include <thread>
#include <unordered_map>
#include <vector>
#include <chrono>
#include <iostream>

namespace env
{

	enum CELL_TYPE
	{
		WATER_CELL,
		GRASS_CELL,
		BURNED_GRASS_CELL,
		BURNING_GRASS_CELL
	};

};
class EnvironmentCell
{
public:
	float flamability;
	env::CELL_TYPE cell_type;
	float burningTimeRemaining;

	EnvironmentCell(env::CELL_TYPE ctype, float flamability);
	EnvironmentCell();
};
class Environment
{
	private:

		int gridWidth = config::screenWidth / config::blockSize;
		int gridHeight = config::screenHeight / config::blockSize;
		int blockSize = config::blockSize;
		float TerrainWaterThreshold = config::TerrainWaterThreshold; // less than 0.25 matlab water

		void generatePerlinNoiseMap();
		void generateEnvironmentTerrain();
		std::vector<std::vector<EnvironmentCell>> environmentGrid;
		std::vector<std::vector<float>> perlinNoiseGrid;
		float tempTimeCounter = 0;

	public:

		Environment();
		Environment(const int &blockSize, const float &terrainWaterFraction);
		~Environment();

		void draw();
		void setFire(int cellCol, int cellRow);
		void updateState(float dt);

		const int getBlockSize();
		const int getGridWidth();
		const int getGridHeight();
};
