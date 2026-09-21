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
* **Threading:** POSIX Threads (`pthread`)
* **Graphics:** Raylib (`libraylib-dev`)
* **IPC (Cross-Platform):** Boost.Interprocess (`libboost-all-dev`)

### Linux Setup for Developers

```bash
sudo apt update
sudo apt install build-essential cmake git libraylib-dev libboost-all-dev

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
