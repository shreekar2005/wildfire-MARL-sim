#include "config.hpp"
#include "simulation/simulation.hpp"
#include <chrono>
#include <cstdlib>
#include <environment/environment.hpp>
#include <iostream>
#include <queue>
#include <raylib.h>
#include <thread>
#include <time.h>
#include <vector>
#include <mutex>

#include <random>

int randomIntNearNum(int num) {
    int target = num;
    int radius = 100;

    std::random_device rd;
    std::mt19937 gen(rd());
    
    // Define the range: [target - radius, target + radius]
    std::uniform_int_distribution<int> dist(target - radius, target + radius);

    return dist(gen);
}


std::mutex FireQueueMutex;

env::EnvironmentCell::EnvironmentCell() {
  this->expiryBurningTime = 0;
	this->expectedCatchFireTime = 0;
}

env::EnvironmentCell::EnvironmentCell(env::CELL_TYPE ctype, float flamability)
{
  this->cell_type = ctype;
  this->flamability = flamability;
  this->expiryBurningTime = 0;
	this->expectedCatchFireTime = 0;

}
env::Environment::Environment()
{
  envShouldStop = false;
  environmentGrid = std::vector<std::vector<EnvironmentCell>>(
  env::gridWidth, std::vector<EnvironmentCell>(env::gridHeight));
  perlinNoiseGrid = std::vector<std::vector<float>>(
  env::gridWidth, std::vector<float>(env::gridHeight));
}

env::Environment::~Environment()
{
  envShouldStop = true;
  environmentGrid.clear();
  perlinNoiseGrid.clear();
  for (auto FireThread : fireThreads)
  {
    if (FireThread->joinable()) FireThread->join();
  }
  for (auto FireThread : fireThreads)
  {
    delete FireThread;
  }
  fireThreads.clear();
}

void env::Environment::generateEnvironmentTerrain()
{
  // Generate Perlin Noise Map
  generatePerlinNoiseMap();
  for (int cols = 0; cols < env::gridWidth; cols++)
  {
    for (int rows = 0; rows < env::gridHeight; rows++)
    {
      if (perlinNoiseGrid[cols][rows] > env::TerrainWaterThreshold)
      {
        // Cell is Grass with flamibility of the noise map

        environmentGrid[cols][rows] = EnvironmentCell(env::GRASS_CELL, perlinNoiseGrid[cols][rows]);
      }
      else
      {
        // cell is water
        environmentGrid[cols][rows] = EnvironmentCell(env::WATER_CELL, 0);
      }
    }
  }
}
void env::Environment::generatePerlinNoiseMap()
{
  // Seed the random number generator
  if (config::seed == 0) srand((unsigned int)time(NULL));
  else SetRandomSeed(config::seed);

  // Generate random offsets
  int randomOffsetX = rand() % 10000;
  int randomOffsetY = rand() % 10000;

  Image img = GenImagePerlinNoise(env::gridWidth, env::gridHeight, randomOffsetX, randomOffsetY, 2);
  Color *data = LoadImageColors(img);
  for (int cols = 0; cols < env::gridWidth; cols++)
  {
    for (int rows = 0; rows < env::gridHeight; rows++)
    {
      // load data of image to grid
      //  since it is grayscale R == G == B
      int index = rows * env::gridWidth + cols;
      // normalize between 0 and 1
      perlinNoiseGrid[cols][rows] = data[index].r / 255.0f;
    }
  }
  UnloadImage(img);
}

void env::Environment::spreadFireTask(std::pair<int, int> fireStartCell)
{
  // random seed according to time
  srand((unsigned int)time(NULL));
  std::vector<std::pair<int, int>> directions = {{-1, 0}, {1, 0}, {0, 1}, {0, -1}};

  simTick current_time = sim::Time::getCurrentSimTick();

  environmentGrid[fireStartCell.first][fireStartCell.second].cell_type = env::BURNING_GRASS_CELL;
  environmentGrid[fireStartCell.first][fireStartCell.second].expiryBurningTime = current_time + (simTick)config::GrassBurnTimeMs;

  std::queue<std::pair<int, int>> fireCellsQueue;
  fireCellsQueue.push(fireStartCell);

  // std::lock_guard<std::mutex> lock(FireQueueMutex);
  while (!fireCellsQueue.empty() && !envShouldStop)
  {
    current_time = sim::Time::getCurrentSimTick();

    auto element = fireCellsQueue.front();
    int cellRow = element.second;
    int cellCol = element.first;
    fireCellsQueue.pop();

   
    if (environmentGrid[cellCol][cellRow].expiryBurningTime <= current_time)
    {
      environmentGrid[cellCol][cellRow].cell_type = env::BURNED_GRASS_CELL;

      // reset its neighbour grass cells if they dont have any neighbour burning grass cells
      for (auto dir : directions){
        if (cellCol + dir.first < 0) continue;
        if (cellCol + dir.first >= env::gridWidth) continue;
        if (cellRow + dir.second >= env::gridHeight) continue;
        if (cellRow + dir.second < 0) continue;

        if(environmentGrid[cellCol + dir.first][cellRow + dir.second].cell_type != env::GRASS_CELL) continue;

        bool haveBurningNeigh = false;
        for(auto dir2 : directions){
          if (cellCol + dir.first + dir2.first < 0) continue;
          if (cellCol + dir.first + dir2.first>= env::gridWidth) continue;
          if (cellRow + dir.second + dir2.second >= env::gridHeight) continue;
          if (cellRow + dir.second + dir2.second < 0) continue;
          if(environmentGrid[cellCol + dir.first + dir2.first][cellRow + dir.second + dir2.second].cell_type == env::BURNING_GRASS_CELL) haveBurningNeigh = true;
        }
        if(!haveBurningNeigh) environmentGrid[cellCol + dir.first][cellRow + dir.second].expectedCatchFireTime=0;
      }

      continue;
    }

    for (auto dir : directions)
    {

      if (cellCol + dir.first < 0) continue;
      if (cellCol + dir.first >= env::gridWidth) continue;
      if (cellRow + dir.second >= env::gridHeight) continue;
      if (cellRow + dir.second < 0) continue;

      if (environmentGrid[cellCol + dir.first][cellRow + dir.second].cell_type == env::GRASS_CELL)
      {
        if(environmentGrid[cellCol + dir.first][cellRow + dir.second].expectedCatchFireTime!=0){
          if(environmentGrid[cellCol + dir.first][cellRow + dir.second].expectedCatchFireTime<=current_time){
            // mark this cell as burning
            environmentGrid[cellCol + dir.first][cellRow + dir.second].cell_type = env::BURNING_GRASS_CELL;
            environmentGrid[cellCol + dir.first][cellRow + dir.second].expiryBurningTime =current_time + (simTick)config::GrassBurnTimeMs;
            fireCellsQueue.push({cellCol + dir.first, cellRow + dir.second});
          }

          // reduce extra time as as research :)
          // float factor=0.2;
          // environmentGrid[cellCol + dir.first][cellRow + dir.second].expectedCatchFireTime=
          //   ((factor)*environmentGrid[cellCol + dir.first][cellRow + dir.second].expectedCatchFireTime+(1-factor)*current_time);
        }
        else {
          int timeToBurn = (int)(config::expectedCatchFireTimeMs/
                                environmentGrid[cellCol + dir.first][cellRow + dir.second].flamability);
          environmentGrid[cellCol + dir.first][cellRow + dir.second].expectedCatchFireTime = current_time+ randomIntNearNum(timeToBurn);
          TraceLog(LOG_INFO, "%d %d", randomIntNearNum(timeToBurn), randomIntNearNum(timeToBurn));
                          
        }
      }
    }

    // push cell back to queue
    fireCellsQueue.push(element);

    std::this_thread::sleep_for(std::chrono::microseconds(1));
  }
}
void env::Environment::spawnFire(Vector2 mousePos)
{
  int cellCol = mousePos.x / config::blockSize;
  int cellRow = mousePos.y / config::blockSize;
  std::pair<int, int> fireStartCell = {cellCol, cellRow};
  fireThreads.push_back(new std::thread(&Environment::spreadFireTask, this, fireStartCell));
}

void env::Environment::drawEnvironment()
{
  for (int cols = 0; cols < env::gridWidth; cols++)
  {
    for (int rows = 0; rows < env::gridHeight; rows++)
    {
      if (environmentGrid[cols][rows].cell_type == env::GRASS_CELL)
      {
        // RGB
        //  0.25 to 0.85 --> 0 to 1
        //  yahan ghass hai ki pani 0.25 ke niche hai pani aur 1 hai no ghaas
        float lerpValue = ((environmentGrid[cols][rows].flamability - env::TerrainWaterThreshold) /
                           (1 - env::TerrainWaterThreshold));

        // more green -> less flamability as close to water , less green more flamability as dry grass

        Color midColor = ColorLerp({255, 255, 0, 200}, {0, 100, 0, 200}, (1 - lerpValue));
        DrawRectangle(cols * config::blockSize, rows * config::blockSize, config::blockSize, config::blockSize, midColor);
      }
      else if (environmentGrid[cols][rows].cell_type == env::BURNING_GRASS_CELL)
      {
        DrawRectangle(cols * config::blockSize, rows * config::blockSize, config::blockSize, config::blockSize, ORANGE);
      }

      else if (environmentGrid[cols][rows].cell_type == env::WATER_CELL)
      {
        DrawRectangle(cols * config::blockSize, rows * config::blockSize, config::blockSize, config::blockSize, BLUE);
      }
      else if (environmentGrid[cols][rows].cell_type == env::BURNED_GRASS_CELL)
      {
        DrawRectangle(cols * config::blockSize, rows * config::blockSize, config::blockSize, config::blockSize, GRAY);
      }
    }
  }
}
