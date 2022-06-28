#include "Runner.h"
#include "WaterRenderer.h"
#include "TerrainRenderer.h"


int main() {
	TerrainRenderer *terrain = new TerrainRenderer();
	WaterRenderer* water = new WaterRenderer();
	Runner runner;
	runner.run();
}
