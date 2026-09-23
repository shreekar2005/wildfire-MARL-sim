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

EnvironmentCell::EnvironmentCell() {

};

EnvironmentCell::EnvironmentCell(env::CELL_TYPE ctype, float flamability) {
  this->cell_type = ctype;
  this->flamability = flamability;
}
Environment::Environment() {

  envShouldStop = false;
  EnvironmentGrid = std::vector<std::vector<EnvironmentCell>>(
      env::gridWidth, std::vector<EnvironmentCell>(env::gridHeight));
  PerlinNoiseGrid = std::vector<std::vector<float>>(
      env::gridWidth, std::vector<float>(env::gridHeight));
  FireThreadid = 0;
  FireThread = new std::thread(&Environment::SpreadFire, this);
}

Environment::~Environment() {

  envShouldStop = true;
  EnvironmentGrid.clear();
  PerlinNoiseGrid.clear();
  FireThread->join();
}

void Environment::GenerateEnvironmentTerrain() {

  // Generate Perlin Noise Map
  GenratePerlinNoiseMap();

  for (int cols = 0; cols < env::gridWidth; cols++) {
    for (int rows = 0; rows < env::gridHeight; rows++) {

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
  for (int cols = 0; cols < env::gridWidth; cols++) {
    for (int rows = 0; rows < env::gridHeight; rows++) {
      // load data of image to grid
      //  since it is grayscale R == G == B
      int index = rows * env::gridWidth + cols;

      // normalize between 0 and 1
      PerlinNoiseGrid[cols][rows] = data[index].r / 255.0f;
    }
  }
  UnloadImage(img);
}



void Environment::SpreadFire() {

  // random seed according to time
  srand((unsigned int)time(NULL));
  std::vector<std::pair<int, int>> directions = {
      {-1, 0}, {1, 0}, {0, 1}, {0, -1}};

  while (!envShouldStop) {
    while (!FireQueue.empty()) {
      auto element = FireQueue.front();
      int cellRow = element.second;
      int cellCol = element.first;
      FireQueue.pop();
      bool isFireLeaf = false;
      for (auto dir : directions) {

        if (cellCol + dir.first < 0)
          continue;
        if (cellCol + dir.first >= env::gridWidth)
          continue;
        if (cellRow + dir.second >= env::gridHeight)
          continue;
        if (cellRow + dir.second < 0)
          continue;

        if (EnvironmentGrid[cellCol + dir.first][cellRow + dir.second]
                .cell_type == env::GRASS_CELL) {
          isFireLeaf = true;
          break;
        }
      }
      if (isFireLeaf) {
        // generate random number
        float FireSpreadProbability = 1.0f * rand() / RAND_MAX;
        for (auto dir : directions) {

          if (cellCol + dir.first < 0)
            continue;
          if (cellCol + dir.first >= env::gridWidth)
            continue;
          if (cellRow + dir.second >= env::gridHeight)
            continue;
          if (cellRow + dir.second < 0)
            continue;
          if (EnvironmentGrid[cellCol + dir.first][cellRow + dir.second]
                  .flamability >= FireSpreadProbability) {
            // mark this cell as burning
            EnvironmentGrid[cellCol + dir.first][cellRow + dir.second]
                .cell_type = env::BURNING_GRASS_CELL;
            FireQueue.push({cellCol + dir.first, cellRow + dir.second});
          }
        }

          // push cell back to queue
          FireQueue.push(element);

      }

  std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }

  }
}
void Environment::SpawnFire(Vector2 mousePos) {
  FireThreadid++;
  int cellCol = mousePos.x/ config::blockSize;
  int cellRow = mousePos.y / config::blockSize;

  EnvironmentGrid[cellCol][cellRow].cell_type = env::BURNING_GRASS_CELL;
  FireQueue.push({cellCol ,cellRow});
}

void Environment::DrawEnvironment() {

  for (int cols = 0; cols < env::gridWidth; cols++) {
    for (int rows = 0; rows < env::gridHeight; rows++) {
      if (EnvironmentGrid[cols][rows].cell_type == env::GRASS_CELL) {
        // RGB
        //  0.25 to 0.85 --> 0 to 1
        //  yahan ghass hai ki pani 0.25 ke niche hai pani aur 1 hai no ghaas
        float lerpValue = ((EnvironmentGrid[cols][rows].flamability -
                            env::TerrainWaterThreshold) /
                           (1 - env::TerrainWaterThreshold));

        Color midColor =
            ColorLerp({255, 255, 0, 200}, {0, 100, 0, 200}, (1 - lerpValue));
        DrawRectangle(cols * config::blockSize, rows * config::blockSize,
                      config::blockSize, config::blockSize, midColor);

      } else if (EnvironmentGrid[cols][rows].cell_type ==
                 env::BURNING_GRASS_CELL) {
        DrawRectangle(cols * config::blockSize, rows * config::blockSize,
                      config::blockSize, config::blockSize, ORANGE);

      }

      else if (EnvironmentGrid[cols][rows].cell_type == env::WATER_CELL){

        DrawRectangle(cols * config::blockSize, rows * config::blockSize,
                      config::blockSize, config::blockSize, BLUE);
      }
      DrawRectangleLines(cols * config::blockSize, rows * config::blockSize,
                         config::blockSize, config::blockSize, BLACK);
    }
  }
}
