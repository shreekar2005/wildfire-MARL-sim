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
std::mutex FireQueueMutex;

EnvironmentCell::EnvironmentCell() {}

EnvironmentCell::EnvironmentCell(env::CELL_TYPE ctype, float flamability) {
  this->cell_type = ctype;
  this->flamability = flamability;
}
Environment::Environment() {
  envShouldStop = false;
  environmentGrid = std::vector<std::vector<EnvironmentCell>>(
      env::gridWidth, std::vector<EnvironmentCell>(env::gridHeight));
  perlinNoiseGrid = std::vector<std::vector<float>>(
      env::gridWidth, std::vector<float>(env::gridHeight));
}

Environment::~Environment() {
  envShouldStop = true;
  environmentGrid.clear();
  perlinNoiseGrid.clear();
  for (auto FireThread : fireThreads) {
    if (FireThread->joinable())
      FireThread->join();
  }
  for (auto FireThread : fireThreads) {
    delete FireThread;
  }
  fireThreads.clear();
}

void Environment::generateEnvironmentTerrain() {
  // Generate Perlin Noise Map
  generatePerlinNoiseMap();
  for (int cols = 0; cols < env::gridWidth; cols++) {
    for (int rows = 0; rows < env::gridHeight; rows++) {
      if (perlinNoiseGrid[cols][rows] > env::TerrainWaterThreshold) {
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

  Image img = GenImagePerlinNoise(env::gridWidth, env::gridHeight,
                                  randomOffsetX, randomOffsetY, 2);
  Color *data = LoadImageColors(img);
  for (int cols = 0; cols < env::gridWidth; cols++) {
    for (int rows = 0; rows < env::gridHeight; rows++) {
      // load data of image to grid
      //  since it is grayscale R == G == B
      int index = rows * env::gridWidth + cols;
      // normalize between 0 and 1
      perlinNoiseGrid[cols][rows] = data[index].r / 255.0f;
    }
  }
  UnloadImage(img);
}

void Environment::spreadFireTask(std::pair<int, int> fireStartCell) {
  // random seed according to time
  srand((unsigned int)time(NULL));
  std::vector<std::pair<int, int>> directions = {
      {-1, 0}, {1, 0}, {0, 1}, {0, -1}};

  std::chrono::steady_clock::time_point current_time = std::chrono::steady_clock::now();
  environmentGrid[fireStartCell.first][fireStartCell.second].cell_type = env::BURNING_GRASS_CELL;
  environmentGrid[fireStartCell.first][fireStartCell.second].expiryBurningTime =
      current_time + (std::chrono::milliseconds)config::GrassBurnTimeMs;
  std::queue<std::pair<int, int>> fireCellsQueue;
  fireCellsQueue.push(fireStartCell);

  // std::lock_guard<std::mutex> lock(FireQueueMutex);
  while (!fireCellsQueue.empty() && !envShouldStop) {
    current_time = std::chrono::steady_clock::now();

    auto element = fireCellsQueue.front();
    int cellRow = element.second;
    int cellCol = element.first;
    fireCellsQueue.pop();

    // TraceLog(LOG_INFO, "%f", environmentGrid[cellCol][cellRow].timeBurned);
    if (environmentGrid[cellCol][cellRow].expiryBurningTime <= current_time) {
      environmentGrid[cellCol][cellRow].cell_type = env::BURNED_GRASS_CELL;
      continue;
    }

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

      if (environmentGrid[cellCol + dir.first][cellRow + dir.second]
              .cell_type == env::GRASS_CELL) {
        isFireLeaf = true;
        break;
      }
    }

    // push cell back to queue
    fireCellsQueue.push(element);

    if (isFireLeaf) {
      // generate random number
      float randomNumForFireSpread = 1.0f * rand() / RAND_MAX;
      for (auto dir : directions) {

        if (cellCol + dir.first < 0)
          continue;
        if (cellCol + dir.first >= env::gridWidth)
          continue;
        if (cellRow + dir.second >= env::gridHeight)
          continue;
        if (cellRow + dir.second < 0)
          continue;

        if ((environmentGrid[cellCol + dir.first][cellRow + dir.second]
                 .flamability /
             config::expectedCatchFireTimeMs) >= randomNumForFireSpread &&
            environmentGrid[cellCol + dir.first][cellRow + dir.second]
                    .cell_type == env::GRASS_CELL) {
          // mark this cell as burning
          environmentGrid[cellCol + dir.first][cellRow + dir.second].cell_type =
              env::BURNING_GRASS_CELL;
          environmentGrid[cellCol + dir.first][cellRow + dir.second]
              .expiryBurningTime =
              current_time + (std::chrono::milliseconds)config::GrassBurnTimeMs;
          fireCellsQueue.push({cellCol + dir.first, cellRow + dir.second});
        }
      }
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
}
void Environment::spawnFire(Vector2 mousePos) {
  int cellCol = mousePos.x / config::blockSize;
  int cellRow = mousePos.y / config::blockSize;
  std::pair<int, int> fireStartCell = {cellCol, cellRow};
  fireThreads.push_back(
      new std::thread(&Environment::spreadFireTask, this, fireStartCell));
}

void Environment::drawEnvironment() {
  for (int cols = 0; cols < env::gridWidth; cols++) {
    for (int rows = 0; rows < env::gridHeight; rows++) {
      if (environmentGrid[cols][rows].cell_type == env::GRASS_CELL) {
        // RGB
        //  0.25 to 0.85 --> 0 to 1
        //  yahan ghass hai ki pani 0.25 ke niche hai pani aur 1 hai no ghaas
        float lerpValue = ((environmentGrid[cols][rows].flamability -
                            env::TerrainWaterThreshold) /
                           (1 - env::TerrainWaterThreshold));

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

void Environment::updateEnvironment(float dt) {

  // by copy
  auto CurrentEnvGrid = environmentGrid;

  //check for burned cell time expiry
  for (int cellCol = 0; cellCol < env::gridWidth; cellCol++) {
    for (int cellRow = 0; cellRow < env::gridHeight; cellRow++) {
      if(CurrentEnvGrid [cellCol][cellRow].cell_type != env::BURNING_GRASS_CELL)continue;
      environmentGrid[cellCol][cellRow].timeBurned += dt;
      if(environmentGrid[cellCol][cellRow].timeBurned >= config::GrassBurnOutTimeSec){
        environmentGrid[cellCol][cellRow].cell_type = env::BURNED_GRASS_CELL;
      }
    }
  }
  
  std::vector<std::pair<int, int>> directions = {{-1, 0}, {1, 0}, {0, 1}, {0, -1}};

  TimeCounter += dt;
  // let x = time between 2 iterations
  // x = 1/n, if n iterations in 1 sec
  // assuming for 1 cell it take k expected iterations
  // then we want expectedCatchFireTimeSec have k iterations
  // therefore 1 sec should have k/expectedCatchFireTimeSec iterations
  // therefore time between 2 iterations will be "expectedCatchFireTimeSec/k"
  // assuming k = 1/(alpha^2) = 1/(0.7^2) --- taking avg alpha = 0.7
  // x = expectedCatchFireTimeSec * (0.49)
  float x = config::expectedCatchFireTimeSec * (0.49) * 2; //keeping *2 because generally 1 cell is surrounded by 2 fire cells
  while(TimeCounter >= x){
    for (int cellCol = 0; cellCol < env::gridWidth; cellCol++) {
      for (int cellRow = 0; cellRow < env::gridHeight; cellRow++) {
        if(CurrentEnvGrid [cellCol][cellRow].cell_type != env::BURNING_GRASS_CELL) continue;

        float randomNumForFireSpread = 1.0f * rand() / RAND_MAX;
        for (auto dir : directions) {
          if (cellCol + dir.first < 0) continue;
          if (cellCol + dir.first >= env::gridWidth) continue;
          if (cellRow + dir.second >= env::gridHeight) continue;
          if (cellRow + dir.second < 0) continue;
          if(CurrentEnvGrid [cellCol + dir.first][cellRow + dir.second].cell_type != env::GRASS_CELL) continue;
          if ((CurrentEnvGrid [cellCol + dir.first][cellRow + dir.second].flamability) >= randomNumForFireSpread) {
                environmentGrid[cellCol + dir.first][cellRow + dir.second].cell_type = env::BURNING_GRASS_CELL;
          }
        }
      }
    }
    TimeCounter -= x;
  }
    }
