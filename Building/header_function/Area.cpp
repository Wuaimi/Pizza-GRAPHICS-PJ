#include"Area.h"
#include"Building.h"

// Constructors
Area::Area(std::vector<glm::vec3>& Positions, Building* building, std::string& Id)
{
    setAreaPositions(Positions);
    setAreaId(Id);
    setBuilding(building);
    setFAR(0.0f);
    setOSR(0.0f);
}

// Setters
void Area::setAreaPositions(std::vector<glm::vec3>& pos) { Positions = pos; }
void Area::setAreaId(std::string ID) { areaId = ID; }
void Area::setBuilding(Building* bd) { building = bd; }
void Area::setFAR(float far) { FAR = far; }
void Area::setOSR(float osr) { OSR = osr; }

// Getters
std::vector<glm::vec3> Area::getAreaPositions() { return Positions; }
std::string Area::getAreaId() { return areaId; }
Building* Area::getBuilding() { return building; }
float Area::getFAR() { return FAR; }
float Area::getOSR() { return OSR; }