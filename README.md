# Wildfire MARL Simulation

A high-performance C++14 multi-agent reinforcement learning (MARL) environment and cellular automata simulation for training autonomous drone swarms to suppress wildfires, featuring native Raylib visualization.

**Project Specification & Architecture:** [Master Technical Document](https://docs.google.com/document/d/1LFJaIudFjCv2jnBK6_rWYz8eyyE4_KfTdR_Rlw3vi2g/edit?usp=sharing)

---

## Architecture Overview

The system uses a Decentralized Partially Observable Markov Decision Process (Dec-POMDP) where drone agents work together to contain wildfires.

* **Cellular Automata Engine:** Simulates procedurally generated 2D terrain, fuel dynamics, and fire spread based on grass type flammability, operating on a single thread with zero wind influence and interactive manual ignition.
* **Autonomous Swarm Agents:** Drones operate with localized fields of view (FOV), tracking state variables including position, velocity, acceleration, and water payload.
* **Redis Blackboard:** Uses Redis channels and key-value datastores as the central IPC broker. Agents publish perception and state updates via PUB/SUB to a standalone RL-server, enabling decoupled process communication.
* **Multi-Threaded Execution:** Each drone agent runs on an independent C++ thread to execute asynchronous polling and decision loops.
* **Native Visualization:** Hardware-accelerated 2D graphical rendering and real-time debugging dashboard using Raylib and Dear ImGui.

---

## Directory Layout

```text
wildfire-MARL-sim/
├── CMakeLists.txt              # Root build configuration
├── LICENSE                     # Project License
├── Makefile                    # Build and execution commands
├── README.md                   # Repository overview and setup instructions
├── docs/                       # Technical notes and architecture documentation
├── include/                    # Public C++ header files (.hpp)
└── src/                        # Implementation source files (.cpp)
    ├── agent/                  # Drone logic and state management
    ├── environment/            # Cellular automata and grid dynamics
    ├── marl/
    │   ├── test-model/         # Evaluation routines and tests
    │   └── train-model/        # Centralized training pipeline
    ├── simulation/             # Shared memory structure and renderer
    └── main.cpp                # Main orchestrator and GUI entry point

```

---

## Dependencies

* **Compiler:** Clang 3.4+ or GCC 4.9+ (C++14 compliance required)
* **Build System:** CMake (>= 3.10) and Make
* **Threading:** C++11 Threads (`std::thread`)
* **Graphics:** Raylib (builds from source)
* **IPC:** Redis Plus Plus (builds on `libhiredis-dev`)

### Linux Setup for Developers

Because certain dependencies are not available in standard Ubuntu package managers (or are outdated), you must install their underlying dependencies and build the wrappers from source:

```bash
# 1. Install system prerequisites (X11, GL, and hiredis)
sudo apt update
sudo apt install build-essential cmake git libasound2-dev libx11-dev libxrandr-dev libxi-dev libgl1-mesa-dev libglu1-mesa-dev libxcursor-dev libxinerama-dev libwayland-dev libxkbcommon-dev libhiredis-dev redis-server

# 2. Build and install Raylib from source (Shared Library)
git clone https://github.com/raysan5/raylib.git
cd raylib
mkdir build && cd build
cmake -DBUILD_SHARED_LIBS=ON -DBUILD_EXAMPLES=OFF ..
make -j$(nproc)
sudo make install
cd ../..
rm -rf raylib

# 3. Build and install redis-plus-plus system-wide
git clone https://github.com/sewenew/redis-plus-plus.git
cd redis-plus-plus
mkdir build && cd build
cmake -DREDIS_PLUS_PLUS_CXX_STANDARD=14 ..
make -j$(nproc)
sudo make install
cd ../..
rm -rf redis-plus-plus

# 4. Ensure your system's dynamic linker cache is updated
sudo ldconfig
```

---

## Build and Execution

This project uses a `Makefile` to simplify out-of-source CMake builds.

```bash
# Clone the repository
git clone https://github.com/shreekar2005/wildfire-MARL-sim.git
cd wildfire-MARL-sim

# Generate the build directory and compile all targets
make build

```

### Running Targets

The build generates discrete binaries for testing, training, and running the simulation. Use the following make commands to execute them:

```bash
# Run the complete simulation with the Raylib GUI
make run

# Run the headless training pipeline
make train-model

# Run model evaluation and verification tests
make test-model

# Clean the build directory to start fresh
make clean

```
