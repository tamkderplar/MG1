#include "pointcam.h"

PointCAM::PointCAM(QObject *parent) :
    QObject(parent)
{
}

PointCAM::PointCAM(glm::vec3 p,QObject *parent) :
    QObject(parent)
  , position(glm::vec4(p,1))
{
}

PointCAM::PointCAM(const PointCAM &other)
{
    position = other.position;
    name = other.name;
}

PointCAM &PointCAM::operator=(const PointCAM &other)
{
    position = other.position;
    name = other.name;
    return *this;
}

void PointCAM::setPos(glm::vec3 p)
{
    position = glm::vec4(p,1);
}

glm::vec4 PointCAM::pos() const
{
    return position;
}
