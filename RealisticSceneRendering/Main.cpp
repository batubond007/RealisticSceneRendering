#include "Runner.h"
#include "WaterRenderer.h"
#include "TerrainRenderer.h"
#include "VolumetricClouds.h"

int main() {

	VolumetricClouds* clouds = new VolumetricClouds();
	//TerrainRenderer *terrain = new TerrainRenderer();
	WaterRenderer* water = new WaterRenderer();
	Runner runner;
	runner.run();
}
