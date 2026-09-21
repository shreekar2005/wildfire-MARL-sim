#include "raylib.h"
#include <cmath>
#include <algorithm>

#include <simulation/simulation.hpp>
#include <environment/grass.hpp>

const int blockSize = 30;


static double hash2D(int blockX, int blockY, int seed = 1337) {
    double dot = blockX * 12.9898 + blockY * 78.233 + seed;
    double val = std::sin(dot) * 43758.5453123;
    return val - std::floor(val);
}

static double lerp(double a, double b, double t) {
    return a + t * (b - a);
}

static double smoothstep(double t) {
    return t * t * (3.0 - 2.0 * t);
}

static double valueNoise2D(double x, double y, int seed = 1337) {
    int x0 = static_cast<int>(std::floor(x));
    int y0 = static_cast<int>(std::floor(y));
    int x1 = x0 + 1;
    int y1 = y0 + 1;

    double sx = smoothstep(x - x0);
    double sy = smoothstep(y - y0);

    double n0 = hash2D(x0, y0, seed);
    double n1 = hash2D(x1, y0, seed);
    double ix0 = lerp(n0, n1, sx);

    n0 = hash2D(x0, y1, seed);
    n1 = hash2D(x1, y1, seed);
    double ix1 = lerp(n0, n1, sx);

    return lerp(ix0, ix1, sy);
}


static double getBlockGrassIntensity(int pixelX, int pixelY, int blockSize, double scale = 0.08) {
    int blockX = pixelX / blockSize;
    int blockY = pixelY / blockSize;

    if (pixelX < 0 && pixelX % blockSize != 0) blockX--;
    if (pixelY < 0 && pixelY % blockSize != 0) blockY--;

    double intensity = valueNoise2D(blockX * scale, blockY * scale);
    return std::max(0.0, std::min(intensity, 1.0));
}

static Color GetGrassColor(double intensity) {
    Color lowGrass  = { 160, 195, 90, 255 }; 
    Color highGrass = { 30, 120, 45, 255 };  

    return Color {
        (unsigned char)lerp(lowGrass.r, highGrass.r, intensity),
        (unsigned char)lerp(lowGrass.g, highGrass.g, intensity),
        (unsigned char)lerp(lowGrass.b, highGrass.b, intensity),
        255
    };
}


void drawGrass() {
    for (int y = 0; y < screenHeight; y += blockSize) {
        for (int x = 0; x < screenWidth; x += blockSize) {
            double intensity = getBlockGrassIntensity(x, y, blockSize);

            Color blockColor = GetGrassColor(intensity);
            DrawRectangle(x, y, blockSize, blockSize, blockColor);
            DrawRectangleLines(x, y, blockSize, blockSize, Color{ 0, 0, 0, 20 });
        }
    }
}