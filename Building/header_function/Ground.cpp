#include "Ground.h"

Ground::Ground() {
	positions.push_back({ 10.0f,   10.0f, 0.0f });
	positions.push_back({ -10.0f,  10.0f, 0.0f });
	positions.push_back({ 10.0f,  -10.0f, 0.0f });
	positions.push_back({ -10.0f, -10.0f, 0.0f });
}

Ground::Ground(std::vector<glm::vec3> positions) {

}

void Ground::addGroundZone(Zone& zone) { zones.push_back(zone); }
std::vector<Zone> Ground::getGroundZones() { return zones; }

void Ground::addGroundRoad(Road& road) { roads.push_back(road); }
std::vector<Road> Ground::getGroundRoads() { return roads; }

//setter
void Ground::setGroundPosition(std::vector<glm::vec3>& pos) { positions = pos; }

//getter
std::vector<glm::vec3> Ground::getGroundPosition() { return positions; }