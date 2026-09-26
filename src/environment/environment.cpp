#include "config.hpp"
#include "simulation/simulation.hpp"
#include <chrono>
#include <cstdlib>
#include <environment/environment.hpp>
#include <iostream>
#include <mutex>
#include <queue>
#include <raylib.h>
#include <thread>
#include <time.h>
#include <vector>

EnvironmentCell::EnvironmentCell() {
  this->burningTimeRemaining = config::GrassBurnOutTimeSec;
}

EnvironmentCell::EnvironmentCell(env::CELL_TYPE ctype, float flamability) {
  this->cell_type = ctype;
  this->flamability = flamability;
  this->burningTimeRemaining = config::GrassBurnOutTimeSec;
}


Environment::Environment() {
  this->gridWidth = config::screenWidth / config::blockSize;
	this->gridHeight = config::screenHeight / config::blockSize;
	this->blockSize = config::blockSize;
  this->TerrainWaterThreshold = config::TerrainWaterThreshold;

  this->environmentGrid = std::vector<std::vector<EnvironmentCell>>(this->gridWidth, std::vector<EnvironmentCell>(this->gridWidth));
  this->perlinNoiseGrid = std::vector<std::vector<float>>(this->gridWidth, std::vector<float>(this->gridWidth));
  this->generateEnvironmentTerrain();
}

Environment::Environment(const int &blockSize, const float &terrainWaterFraction){
  this->blockSize = blockSize;
  this->gridWidth = config::screenWidth / blockSize;
  this->gridHeight = config::screenHeight / blockSize;
  this->TerrainWaterThreshold = terrainWaterFraction;

  this->environmentGrid = std::vector<std::vector<EnvironmentCell>>(this->gridWidth, std::vector<EnvironmentCell>(this->gridWidth));
  this->perlinNoiseGrid = std::vector<std::vector<float>> (this->gridWidth, std::vector<float>(this->gridWidth));
  this->generateEnvironmentTerrain();

}
Environment::~Environment() {
  environmentGrid.clear();
  perlinNoiseGrid.clear();
}

void Environment::generateEnvironmentTerrain() {
  // Generate Perlin Noise Map
  generatePerlinNoiseMap();
  for (int cols = 0; cols < this->gridWidth; cols++) {
    for (int rows = 0; rows < this->gridWidth; rows++) {
      if (perlinNoiseGrid[cols][rows] > this->TerrainWaterThreshold) {
        // Cell is Grass with flamibility of the noise map

        environmentGrid[cols][rows] =
            EnvironmentCell(env::GRASS_CELL, perlinNoiseGrid[cols][rows]);
      } else {
        // cell is water
        environmentGrid[cols][rows] = EnvironmentCell(env::WATER_CELL, 0);
      }
    }
  }
}
void Environment::generatePerlinNoiseMap() {
  // Seed the random number generator
  if (config::seed == 0)
    srand((unsigned int)time(NULL));
  else
    SetRandomSeed(config::seed);

  // Generate random offsets
  int randomOffsetX = rand() % 10000;
  int randomOffsetY = rand() % 10000;

  Image img = GenImagePerlinNoise(this->gridWidth, this->gridWidth,
                                  randomOffsetX, randomOffsetY, 2);
  Color *data = LoadImageColors(img);
  for (int cols = 0; cols < this->gridWidth; cols++) {
    for (int rows = 0; rows < this->gridWidth; rows++) {
      // load data of image to grid
      //  since it is grayscale R == G == B
      int index = rows * this->gridWidth + cols;
      // normalize between 0 and 1
      this->perlinNoiseGrid[cols][rows] = data[index].r / 255.0f;
    }
  }
  UnloadImage(img);
}

void Environment::setFire(int cellCol, int cellRow){
  this->environmentGrid[cellCol][cellRow].cell_type = env::BURNING_GRASS_CELL;
}

const int Environment::getBlockSize(){return this->blockSize;}
const int Environment::getGridWidth(){return this->gridWidth;}
const int Environment::getGridHeight(){return this->gridHeight;}

void Environment::draw() {
  for (int cols = 0; cols < this->gridWidth; cols++) {
    for (int rows = 0; rows < this->gridWidth; rows++) {
      if (environmentGrid[cols][rows].cell_type == env::GRASS_CELL) {
        // RGB
        //  0.25 to 0.85 --> 0 to 1
        //  yahan ghass hai ki pani 0.25 ke niche hai pani aur 1 hai no ghaas
        float lerpValue = ((environmentGrid[cols][rows].flamability -
                            this->TerrainWaterThreshold) /
                           (1 - this->TerrainWaterThreshold));

        // more green -> less flamability as close to water , less green more
        // flamability as dry grass

        Color midColor =
            ColorLerp({255, 255, 0, 200}, {0, 100, 0, 200}, (1 - lerpValue));
        DrawRectangle(cols * config::blockSize, rows * config::blockSize,
                      config::blockSize, config::blockSize, midColor);
      } else if (environmentGrid[cols][rows].cell_type ==
                 env::BURNING_GRASS_CELL) {
        DrawRectangle(cols * config::blockSize, rows * config::blockSize,
                      config::blockSize, config::blockSize, ORANGE);
      }

      else if (environmentGrid[cols][rows].cell_type == env::WATER_CELL) {
        DrawRectangle(cols * config::blockSize, rows * config::blockSize,
                      config::blockSize, config::blockSize, BLUE);
      } else if (environmentGrid[cols][rows].cell_type ==
                 env::BURNED_GRASS_CELL) {
        DrawRectangle(cols * config::blockSize, rows * config::blockSize,
                      config::blockSize, config::blockSize, GRAY);
      }
    }
  }
}

void Environment::updateState(float dt) {

  // by copy
  auto CurrentEnvGrid = environmentGrid;

  //check for burned cell time expiry
  for (int cellCol = 0; cellCol < this->gridWidth; cellCol++) {
    for (int cellRow = 0; cellRow < this->gridWidth; cellRow++) {
      if(CurrentEnvGrid [cellCol][cellRow].cell_type != env::BURNING_GRASS_CELL)continue;
      environmentGrid[cellCol][cellRow].burningTimeRemaining -= dt;
      if(environmentGrid[cellCol][cellRow].burningTimeRemaining <= 0){
        environmentGrid[cellCol][cellRow].cell_type = env::BURNED_GRASS_CELL;
      }
    }
  }
  
  std::vector<std::pair<int, int>> directions = {{-1, 0}, {1, 0}, {0, 1}, {0, -1}};

  tempTimeCounter += dt;
  // let x = time between 2 iterations
  // x = 1/n, if n iterations in 1 sec
  // assuming for 1 cell it take k expected iterations
  // then we want expectedCatchFireTimeSec have k iterations
  // therefore 1 sec should have k/expectedCatchFireTimeSec iterations
  // therefore time between 2 iterations will be "expectedCatchFireTimeSec/k"
  // assuming k = 1/(alpha^2) = 1/(0.7^2) --- taking avg alpha = 0.7
  // x = expectedCatchFireTimeSec * (0.49)
  float x = config::expectedCatchFireTimeSec * (0.49) * 2; //keeping *2 because generally 1 cell is surrounded by 2 fire cells
  while(tempTimeCounter >= x){
    for (int cellCol = 0; cellCol < this->gridWidth; cellCol++) {
      for (int cellRow = 0; cellRow < this->gridWidth; cellRow++) {
        if(CurrentEnvGrid [cellCol][cellRow].cell_type != env::BURNING_GRASS_CELL) continue;

        float randomNumForFireSpread = 1.0f * rand() / RAND_MAX;
        for (auto dir : directions) {
          if (cellCol + dir.first < 0) continue;
          if (cellCol + dir.first >= this->gridWidth) continue;
          if (cellRow + dir.second >= this->gridWidth) continue;
          if (cellRow + dir.second < 0) continue;
          if(CurrentEnvGrid [cellCol + dir.first][cellRow + dir.second].cell_type != env::GRASS_CELL) continue;
          if ((CurrentEnvGrid [cellCol + dir.first][cellRow + dir.second].flamability) >= randomNumForFireSpread) {
                environmentGrid[cellCol + dir.first][cellRow + dir.second].cell_type = env::BURNING_GRASS_CELL;
          }
        }
      }
    }
    tempTimeCounter -= x;
  }
}
