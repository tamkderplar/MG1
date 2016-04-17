#ifndef BSPLINE_H
#define BSPLINE_H

#include <QObject>
#include "pointcam.h"
#include "bsplinebasis.h"
#include "bezier3.h"
#include "linesegment.h"

class BSpline : public QObject
{
    Q_OBJECT

    QList<PointCAM*> control;
    QList<BSplineBasis*> segments_b;
    QList<Bezier3*> segments;
    QList<LineSegment*> controlPolygon;
    GLManager*man;
    bool bezierBasis;
    void clearPolygon();
    void clearSegments();
    void genBSplinePolygon();
    void genBezierPolygon();
    void genBSplineSegments();
    void genBezierSegments();
public:
    explicit BSpline(QVector<PointCAM*>,GLManager*, QObject *parent = 0);
    ~BSpline();
    void append(PointCAM*p);

signals:
    //void destroyMe(BSpline*);

public slots:
    void changeBasis(bool showBezierBasis);
    void updateRemoved(PointCAM*);
    void updateChanged(PointCAM*);

};

#endif // BSPLINE_H
