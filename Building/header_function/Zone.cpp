#include "Zone.h"

Zone::Zone(std::string Id, std::vector<glm::vec3> positions) {
	setZoneId(Id);
	setZonepositions(positions);
}

void Zone::addZoneArea(Area& area) { areas.push_back(area); }
std::vector<Area>& Zone::getZoneAreas() { return areas; }

void Zone::addZoneRoad(Road& road) { roadsRelated.push_back(road); }
std::vector<Road>& Zone::getZoneRoads() { return roadsRelated; }

// Setters
void Zone::setZonepositions(std::vector<glm::vec3>& positions) { Positions = positions; }
void Zone::setZoneId(std::string& zoneId) { id = zoneId; }

// Getters
std::vector<glm::vec3>& Zone::getZonePositions() { return Positions; }
std::string Zone::getZoneId() { return id; }