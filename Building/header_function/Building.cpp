#include "Building.h"

// Default constructor
Building::Building(std::string type, int floors, float floorHeight, std::vector<glm::vec3>& position, float maxHeight)
{
    setType(type);
    setFloors(floors);
    setFloorHeight(floorHeight);
    setPosition(position);
    setMaxFloorHeight(maxHeight);
}

// Setters
void Building::setType(std::string t) { type = t; }
void Building::setFloors(int f) { floors = f; }
void Building::setFloorHeight(float fh) { floorHeight = fh; }
void Building::setMaxFloorHeight(float fmaxheight) { maxHeight = fmaxheight; }
void Building::setPosition(std::vector<glm::vec3>& pos) { positions = pos; }

// Getters
std::string Building::getType() { return type; }
int Building::getFloors() { return floors; }
float Building::getFloorHeight() { return floorHeight; }
float Building::getMaxFloorHeight() { return maxHeight; }
std::vector<glm::vec3> Building::getPosition() { return positions; }

// Utility
float Building::getTotalHeight() {
    return floors * floorHeight;
}
