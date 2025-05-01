#ifndef ROAD_CLASS_H
#define ROAD_CLASS_H

#include <vector>
#include <glm/glm.hpp>
#include <string>

enum class RoadType {
    Public,
    Private
};

class Road {
private:
    RoadType type;
    std::vector<glm::vec3> pathPoints;
    float width;
    std::string id;

public:
    // Constructors
    Road(std::vector<glm::vec3>& points, float width, RoadType type, std::string& roadId);

    // Setters
    void setPath(std::vector<glm::vec3>& points);
    void setWidth(float width);
    void setType(RoadType type);
    void setId(std::string roadId);

    // Getters
    std::vector<glm::vec3>& getPath();
    float getWidth();
    RoadType getType();
    std::string getId();

};

#endif
