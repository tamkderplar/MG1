#include "torus.h"

#include <QDebug>

Torus::Torus()
    :nr(8)
    ,nR(8)
    ,r(0.3)
    ,R(1.0)
{
    updateTopology();
}

glm::vec4 Torus::f(float u, float v) const
{
    float pi = glm::pi<float>();
    return {
        (R+r*cos(v*2*pi))*cos(u*2*pi),
        (R+r*cos(v*2*pi))*sin(u*2*pi),
        r*sin(v*2*pi),
        1.0
    };
}

void Torus::updateTopology()
{
    updateShape();

    edges.clear();
    for(int i=0;i<nr;i++){
        edges.append({ i*nR + (nR-1),
                       i*nR +  0});
    //}
    //for(int i=0;i<nr;i++){
        for(int j=1;j<nR;j++)
            edges.append({ i*nR + (j-1),
                           i*nR +  j});
    }

    for(int j=0;j<nR;j++){
        edges.append({ (nr-1)*nR + j,
                        0    *nR + j});
    //}
    //for(int j=0;j<nR;j++){
        for(int i=1;i<nr;i++)
            edges.append({(i-1)*nR + j,
                           i   *nR + j});
    }
}

void Torus::updateShape()
{
    points.clear();
    for(int i=0;i<nr;i++)
        for(int j=0;j<nR;j++)
            points.append(f(i/float(nr),j/float(nR)));
}

void Torus::setNR(int nR)
{
    this->nR = qMax(nR,3);
    updateTopology();
}

void Torus::setNr(int nr)
{
    this->nr = qMax(nr,3);
    updateTopology();
}

void Torus::setR(float R)
{
    this->R = R;
    updateShape();
}

void Torus::setr(float r)
{
    this->r = r;
    updateShape();
}

int Torus::NR()
{
    return nR;
}

int Torus::Nr()
{
    return nr;
}
