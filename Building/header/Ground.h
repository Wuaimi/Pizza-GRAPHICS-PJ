#ifndef GROUND_CLASS_H
#define GROUND_CLASS_H

#include <glm/glm.hpp>
#include <vector>
#include "Zone.h"
#include"Road.h"

class Ground {
    private:
        std::vector<glm::vec3> positions;
        std::vector<Zone> zones;
        std::vector<Road> roads;

    public:
        Ground();
        Ground(std::vector<glm::vec3> positions);

        void addGroundZone(Zone& zone);
        std::vector<Zone> getGroundZones();

        void addGroundRoad(Road& road);
        std::vector<Road> getGroundRoads();

        void setGroundPosition(std::vector<glm::vec3>& positions);
        std::vector<glm::vec3> getGroundPosition();
};
#endif
