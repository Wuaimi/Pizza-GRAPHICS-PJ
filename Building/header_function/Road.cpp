#include "Road.h"

// Constructors
Road::Road(std::vector<glm::vec3>& points, float width, RoadType type, std::string& roadId) {
	setPath(points);
	setWidth(width);
	setType(type);
	setId(roadId);
}

// Setters
void Road::setPath(std::vector<glm::vec3>& points) { pathPoints = points; }
void Road::setWidth(float w) { width = w; }
void Road::setType(RoadType t) { type = t; }
void Road::setId(std::string roadId) { id = roadId; }

// Getters
std::vector<glm::vec3>& Road::getPath() { return pathPoints; }
float Road::getWidth() { return width; }
RoadType Road::getType() { return type; }
std::string Road::getId() { return id; }
