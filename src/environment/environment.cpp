#include "simulation/simulation.hpp"
#include <environment/environment.hpp>
#include <iostream>
#include <vector>
#include <raylib.h>

//TODO 
// generate grass logic
// generate fire logic
// update fire logic


EnvironmentCell::EnvironmentCell(){

};

EnvironmentCell::EnvironmentCell(env::CELL_TYPE ctype ,  float flamability){
		this->cell_type = ctype;
		this->flamability = flamability;
}
Environment::Environment(){

		EnvironmentGrid = std::vector<std::vector<EnvironmentCell>>(env::gridHeight , std::vector<EnvironmentCell>(env::gridWidth));
		PerlinNoiseGrid = std::vector<std::vector<float>>(env::gridHeight , std::vector<float>(env::gridWidth));
		FireValueArray = std::vector<std::vector<float>>(env::gridHeight , std::vector<float>(env::gridWidth));

		// EnvironmentGrid = new std::vector<std::vector<EnvironmentCell>>(env::gridHeight , std::vector<EnvironmentCell>(env::gridWidth));
}

Environment::~Environment(){

		EnvironmentGrid.clear();
		PerlinNoiseGrid.clear();

}
void Environment::GenerateEnvironmentTerrain(){

		//Generate Perlin Noise Map
		GenratePerlinNoiseMap();

		for(int cols = 0 ; cols < env::gridHeight ;cols++ ){
				for(int rows = 0 ; rows < env::gridWidth ;rows++){
						
						if(PerlinNoiseGrid[cols][rows] > env::TerrainGrassThreshold){
								// Cell is Grass with flamibility of the noise map
								
								EnvironmentGrid[cols][rows] = EnvironmentCell (env::GRASS_CELL , PerlinNoiseGrid[cols][rows]);

						}else{
								//cell is water

								
								EnvironmentGrid[cols][rows] = EnvironmentCell (env::WATER_CELL , 0);


						}

				}

		}


}
void Environment::GenratePerlinNoiseMap(){
		Image img = GenImagePerlinNoise(env::gridWidth, env::gridHeight, 0, 0, 2);
		Color * data = LoadImageColors(img); 
		for(int cols = 0 ; cols < env::gridHeight ;cols++ ){
				for(int rows = 0 ; rows < env::gridWidth ;rows++){
						//load data of image to grid
						// since it is grayscale R == G == B 
						int index = cols*env::gridWidth + rows;

						//normalize between 0 and 1
						PerlinNoiseGrid[cols][rows] = data[index].r/255.0f;

				}

		}
		UnloadImage(img);

}

void Environment::DrawEnvironment(){

		for(int cols = 0 ; cols < env::gridHeight ; cols++){
				for(int rows = 0 ; rows < env::gridWidth ; rows++){
						if(EnvironmentGrid[cols][rows].cell_type == env::GRASS_CELL){

						DrawRectangle(rows*sim::blockSize, cols*sim::blockSize, sim::blockSize, sim::blockSize, GREEN);
						}else{

						DrawRectangle(rows*sim::blockSize, cols*sim::blockSize, sim::blockSize, sim::blockSize, BLUE);
						}
						DrawRectangleLines(rows*sim::blockSize	, cols*sim::blockSize	, sim::blockSize, sim::blockSize, BLACK);
						
				}
		}


}
