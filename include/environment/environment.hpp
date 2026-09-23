#pragma once
#include "config.hpp"
#include <queue>
#include <raylib.h>
#include <simulation/simulation.hpp>
#include <thread>
#include <unordered_map>
#include <vector>
namespace env{

enum CELL_TYPE{
		WATER_CELL,
		GRASS_CELL,
		BURNED_GRASS_CELL,
		BURNING_GRASS_CELL
};

		const int gridWidth = config::screenWidth/config::blockSize;
		const int gridHeight= config::screenHeight/config::blockSize;
		const int blocksize = config::blockSize;
		const float TerrainWaterThreshold= config::TerrainWaterThreshold; // less than 0.25 matlab water

};
class EnvironmentCell{
		public:
				float flamability;
				env::CELL_TYPE cell_type;
				float timeBurned = 0;

		EnvironmentCell(env::CELL_TYPE ctype , float flamability);
		EnvironmentCell();
				
};
class Environment{
		public:
				std::vector<std::vector<EnvironmentCell>> EnvironmentGrid;
				std::vector<std::vector<float>> PerlinNoiseGrid;
				std::thread * FireThread;
				std::queue<std::pair<int ,int>> FireQueue;
				int FireThreadid = 0;
				bool envShouldStop;
		

				Environment();
				~Environment();
				void GenerateEnvironmentTerrain();
				void DrawEnvironment();
				void GenratePerlinNoiseMap();
				void SpawnFire(Vector2 mousePos);
				void SpreadFire();



};
