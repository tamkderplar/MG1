#ifndef BEZIER3C0_H
#define BEZIER3C0_H

#include <QObject>
#include "pointcam.h"
#include "bezier3.h"
#include "linesegment.h"

class Bezier3C0 : public QObject
{
    Q_OBJECT

    QList<PointCAM*> control;
    QList<Bezier3*> segments;
    QList<LineSegment*> controlPolygon;
    GLManager*man;
    void clearPolygon();
    void clearSegments();
    void genBezierPolygon();
    void genBezierSegments();
public:
    explicit Bezier3C0(QVector<PointCAM*>,GLManager*, QObject *parent = 0);
    ~Bezier3C0();
    void append(PointCAM*p);

signals:

public slots:
    void updateRemoved(PointCAM*);
    void updateChanged(PointCAM*);

};

#endif // BEZIER3C0_H
