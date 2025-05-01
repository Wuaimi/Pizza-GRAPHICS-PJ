#ifndef BUILDING_CLASS_H
#define BUILDING_CLASS_H

#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <string>
#include<iostream>
#include<vector>


class Building {
private:
    std::string type;
    std::vector<glm::vec3> positions;
    int floors;
    float floorHeight;
    float maxHeight;

public:
    // Constructors
    Building(std::string type, int floors = 1.0f, float floorHeight = 20.0f, std::vector<glm::vec3>& positions, float maxHeight = 20.0f);

    // Setters
    void setType(std::string type);
    void setFloors(int floors);
    void setFloorHeight(float fheight);
    void setMaxFloorHeight(float fmaxheight);
    void setPosition(std::vector<glm::vec3>& positions);

    // Getters
    std::string getType();
    int getFloors();
    float getFloorHeight();
    float getMaxFloorHeight();
    std::vector<glm::vec3> getPosition();

    // Utilities
    float getTotalHeight();
};

#endif
