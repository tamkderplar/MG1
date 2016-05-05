#ifndef PATCHC0_H
#define PATCHC0_H

#include <QObject>
#include "pointcam.h"
#include "bezier3.h"
#include "linesegment.h"

class PatchC0 : public QObject
{
    Q_OBJECT

    const int points_u, points_v;//PointCAM count in each direction
    const int patches_u, patches_v;//parch count in each direction
    const bool wrap;
    int ribs_u,ribs_v;//ribs count, needed in updating
    int const_ribs;
    QList<PointCAM*> control;
    QList<Bezier3*> segments;
    QList<LineSegment*> controlPolygon;
    GLManager*man;
public:
    explicit PatchC0(glm::vec4,int,int,bool,float,float,GLManager*, QObject *parent = 0);
    ~PatchC0();
    void updateChanged(PointCAM*);

signals:

public slots:
    void updateRibsChanged(int,int);

};

#endif // PATCHC0_H
