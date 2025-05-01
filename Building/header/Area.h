#ifndef AREA_CLASS_H
#define AREA_CLASS_H

#include <vector>
#include <glm/glm.hpp>
#include <string>
#include"Building.h"
#include"Road.h"

class Area {
private:
    std::vector<glm::vec3> Positions;
    std::vector<Road> adjacentRoads;
    std::string areaId;
    Building* building;
    float FAR = 0.0; // Floor Area Ratio
    float OSR = 0.0; // Open Space Ratio

public:
    // Constructors
    Area(std::vector<glm::vec3>& Positions, Building* building, std::string& Id);

    // Setters
    void setAreaPositions(std::vector<glm::vec3>& Positions);
    void setAreaId(std::string Id);
    void setBuilding(Building* building);
    void setFAR(float FAR);
    void setOSR(float OSR);

    // Getters
    std::vector<glm::vec3> getAreaPositions();
    std::string getAreaId() ;
    Building* getBuilding();
    float getFAR();
    float getOSR();
};

#endif