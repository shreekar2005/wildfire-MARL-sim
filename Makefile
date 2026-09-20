.PHONY: all build clean run train-model test-model

all: build

build:
	cmake -B build
	cmake --build build -j$$(nproc)

clean:
	rm -rf build/

run: build
	./build/wildfire_marl_sim

train-model: build
	./build/marl/train-model/train_model

test-model: build
	./build/marl/test-model/eval_model