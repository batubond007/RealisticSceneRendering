#include "Runner.h"
#include "WaterRenderer.h"
#include "TerrainRenderer.h"
#include "VolumetricClouds.h"

int main() {
	Runner runner;
	VolumetricClouds* clouds = new VolumetricClouds();
	TerrainRenderer *terrain = new TerrainRenderer();
	WaterRenderer* water = new WaterRenderer();
	runner.run();
}
