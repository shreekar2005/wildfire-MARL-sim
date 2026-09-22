#pragma once
#include <simulation/simulation.hpp>
#include <vector>
namespace env{

enum CELL_TYPE{
		WATER_CELL,
		GRASS_CELL,
};

		const int gridWidth = sim::screenWidth/sim::blockSize;
		const int gridHeight= sim::screenHeight/sim::blockSize;
		const float TerrainGrassThreshold = 0.2;

};
class EnvironmentCell{
		public:
				float flamability;
				env::CELL_TYPE cell_type;
		EnvironmentCell(env::CELL_TYPE ctype , float flamability);
		EnvironmentCell();
				
};
class Environment{
		public:
				std::vector<std::vector<EnvironmentCell>> EnvironmentGrid;
				std::vector<std::vector<float>> PerlinNoiseGrid;
				std::vector<std::vector<float>> FireValueArray;
		

				Environment();
				~Environment();
				void GenerateEnvironmentTerrain();
				void DrawEnvironment();
				void GenratePerlinNoiseMap();



};
