#ifndef TORUS_H
#define TORUS_H

#include <QVector>

#include "glm.h"

class Torus
{
    int nr, nR;
    float r,R;
    //generated geometry and topology
public:
    QVector<glm::vec4> points;
    QVector<glm::ivec2> edges;
    Torus();
    glm::vec4 f(float u, float v) const;
    void updateTopology();
    void updateShape();

    void setNR(int nR);
    void setNr(int nr);
    void setR(float R);
    void setr(float r);
    int NR();
    int Nr();
};

#endif // TORUS_H
