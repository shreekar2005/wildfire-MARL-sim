#include "config.hpp"
#include "simulation/simulation.hpp"
#include <cstdlib>
#include <environment/environment.hpp>
#include <iostream>
#include <queue>
#include <raylib.h>
#include <thread>
#include <time.h>
#include <vector>

// TODO
//  generate grass logic
//  generate fire logic
//  update fire logic

EnvironmentCell::EnvironmentCell() {

};

EnvironmentCell::EnvironmentCell(env::CELL_TYPE ctype, float flamability) {
  this->cell_type = ctype;
  this->flamability = flamability;
}
Environment::Environment() {

  EnvironmentGrid = std::vector<std::vector<EnvironmentCell>>(
      env::gridHeight, std::vector<EnvironmentCell>(env::gridWidth));
  PerlinNoiseGrid = std::vector<std::vector<float>>(
      env::gridHeight, std::vector<float>(env::gridWidth));
  FireThreadid = 0;

  // EnvironmentGrid = new
  // std::vector<std::vector<EnvironmentCell>>(env::gridHeight ,
  // std::vector<EnvironmentCell>(env::gridWidth));
}

Environment::~Environment() {

  EnvironmentGrid.clear();
  PerlinNoiseGrid.clear();
}
void Environment::GenerateEnvironmentTerrain() {

  // Generate Perlin Noise Map
  GenratePerlinNoiseMap();

  for (int cols = 0; cols < env::gridHeight; cols++) {
    for (int rows = 0; rows < env::gridWidth; rows++) {

      if (PerlinNoiseGrid[cols][rows] > env::TerrainWaterThreshold) {
        // Cell is Grass with flamibility of the noise map

        EnvironmentGrid[cols][rows] =
            EnvironmentCell(env::GRASS_CELL, PerlinNoiseGrid[cols][rows]);

      } else {
        // cell is water

        EnvironmentGrid[cols][rows] = EnvironmentCell(env::WATER_CELL, 0);
      }
    }
  }
}
void Environment::GenratePerlinNoiseMap() {
  // Seed the random number generator

  if (config::seed == 0) {

    srand((unsigned int)time(NULL));
  } else {
    SetRandomSeed(config::seed);
  }

  // Generate random offsets
  int randomOffsetX = rand() % 10000;
  int randomOffsetY = rand() % 10000;

  Image img = GenImagePerlinNoise(env::gridWidth, env::gridHeight,
                                  randomOffsetX, randomOffsetY, 2);
  Color *data = LoadImageColors(img);
  for (int cols = 0; cols < env::gridHeight; cols++) {
    for (int rows = 0; rows < env::gridWidth; rows++) {
      // load data of image to grid
      //  since it is grayscale R == G == B
      int index = cols * env::gridWidth + rows;

      // normalize between 0 and 1
      PerlinNoiseGrid[cols][rows] = data[index].r / 255.0f;
    }
  }
  UnloadImage(img);
}
void Environment::SpreadFireSingleThread() {

  std::vector<std::pair<int, int>> directions = {
      {-1, 0}, {1, 0}, {0, 1}, {0, -1}};
  srand((unsigned int)time(NULL));

  for (int cols = 0; cols < env::gridHeight; cols++) {
    for (int rows = 0; rows < env::gridWidth; rows++) {

      if (EnvironmentGrid[cols][rows].cell_type == env::BURNING_GRASS_CELL) {

        bool checkLeaf = false;
        for (auto dir : directions) {
          if (cols + dir.first < 0)
            continue;
          if (cols + dir.first >= env::gridWidth)
            continue;
          if (rows + dir.second < 0)
            continue;
          if (rows + dir.second > env::gridHeight)
            continue;
          if (EnvironmentGrid[cols + dir.first][rows + dir.second].cell_type ==
              env::GRASS_CELL) {
            checkLeaf = true;
            break;
          }
        }

        if (checkLeaf) {

          // generate random number between 0 and 1
          float FireSpreadRandomChange = 1.0 * rand() / RAND_MAX;
          for (auto dir : directions) {
            if (cols + dir.first < 0)
              continue;
            if (cols + dir.first >= env::gridWidth)
              continue;
            if (rows + dir.second < 0)
              continue;
            if (rows + dir.second > env::gridHeight)
              continue;
            if (EnvironmentGrid[cols + dir.first][rows + dir.second]
                    .flamability >= FireSpreadRandomChange) {
				EnvironmentGrid[cols + dir.first][rows + dir.second].cell_type = env::BURNING_GRASS_CELL;

            }
          }
        }
      }
    }
  }
}
void Environment::SpreadFire(int cellRow, int cellCol) {

  // mutlithreaded code
  //
  //  std::queue<std::pair<int,int>> ThisFireQueue;
  //  ThisFireQueue.push({cellRow , cellCol});
  //  while (!ThisFireQueue.empty()) {
  //
  //  		auto element = ThisFireQueue.front();
  //  		int ThisFireCellRow = element.first;
  //  		int ThisFireCellCol= element.second;
  //  		EnvironmentGrid[ThisFireCellCol][ThisFireCellRow].cell_type =
  //  env::BURNING_GRASS_CELL;
  //
  //
  //  }
  //
}
void Environment::SpawnFire(Vector2 mousePos) {
  FireThreadid++;
  int cellCol = mousePos.y / config::blockSize;
  int cellRow = mousePos.x / config::blockSize;

  EnvironmentGrid[cellCol][cellRow].cell_type = env::BURNING_GRASS_CELL;
  // FireThreads[FireThreadid] = new std::thread(&Environment::SpreadFire ,this
  // , cellRow,cellCol);
}

void Environment::DrawEnvironment() {

  for (int cols = 0; cols < env::gridHeight; cols++) {
    for (int rows = 0; rows < env::gridWidth; rows++) {
      if (EnvironmentGrid[cols][rows].cell_type == env::GRASS_CELL) {
        // RGB
        //  0.25 to 0.85 --> 0 to 1
        //  yahan ghass hai ki pani 0.25 ke niche hai pani aur 1 hai no ghaas
        float lerpValue = ((EnvironmentGrid[cols][rows].flamability -
                            env::TerrainWaterThreshold) /
                           (1 - env::TerrainWaterThreshold));

        Color midColor =
            ColorLerp({255, 255, 0, 200}, {0, 100, 0, 200}, (1 - lerpValue));
        DrawRectangle(rows * config::blockSize, cols * config::blockSize,
                      config::blockSize, config::blockSize, midColor);

      } else if (EnvironmentGrid[cols][rows].cell_type ==
                 env::BURNING_GRASS_CELL) {
        DrawRectangle(rows * config::blockSize, cols * config::blockSize,
                      config::blockSize, config::blockSize, ORANGE);

      }

      else {

        DrawRectangle(rows * config::blockSize, cols * config::blockSize,
                      config::blockSize, config::blockSize, BLUE);
      }
      DrawRectangleLines(rows * config::blockSize, cols * config::blockSize,
                         config::blockSize, config::blockSize, BLACK);
    }
  }
}
