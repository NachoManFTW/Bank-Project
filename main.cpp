#include "SimulationManager.h"
#include <iostream>
#include <cstdlib>

int main() {
    srand(time(0));
    SimulationManager simulation;
    simulation.startSimulation();
    return 0;
}