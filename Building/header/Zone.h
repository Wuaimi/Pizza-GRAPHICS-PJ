#ifndef ZONE_CLASS_H
#define ZONE_CLASS_H

#include <glm/glm.hpp>
#include <vector>
#include "Area.h"
#include "Building.h"
#include "Road.h"

class Zone {
    private:
        std::vector<glm::vec3> Positions;
        std::vector<Area> areas;
        std::vector<Road> roadsRelated;
        std::string id;

    public:
        Zone(std::string id, std::vector<glm::vec3> positions);

        void addZoneArea(Area& area);
        std::vector<Area>& getZoneAreas();

        void addZoneRoad(Road& road);
        std::vector<Road>& getZoneRoads();

        // Setters
        void setZonepositions(std::vector<glm::vec3>& positions);
        void setZoneId(std::string& areaId);

        // Getters
        std::vector<glm::vec3>& getZonePositions();
        std::string getZoneId();
};

#endif
