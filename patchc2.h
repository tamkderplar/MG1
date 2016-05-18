#ifndef PATCHC2_H
#define PATCHC2_H

#include <QObject>
#include "pointcam.h"
#include "bsplinebasis.h"
#include "linesegment.h"

class PatchC2 : public QObject
{
    Q_OBJECT

    const int points_u, points_v;//PointCAM count in each direction
    const int patches_u, patches_v;//parch count in each direction
    const bool wrap;
    int ribs_u,ribs_v;//ribs count, needed in updating
    int const_ribs;
    QList<PointCAM*> control;
    QVector<BSplineBasis*> segments;
    QList<LineSegment*> controlPolygon;
    GLManager*man;
public:
    explicit PatchC2(glm::vec4,int,int,bool,float,float,GLManager*, QObject *parent = 0);
    ~PatchC2();
    void updateChanged(PointCAM*);

signals:

public slots:
    void updateRibsChanged(int,int);

};

#endif // PATCHC2_H
