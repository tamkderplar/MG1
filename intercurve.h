#ifndef INTERCURVE_H
#define INTERCURVE_H

#include <QObject>
#include "pointcam.h"
#include "bezier3.h"
#include "linesegment.h"

class InterCurve : public QObject
{
    Q_OBJECT

    QList<PointCAM*> control;
    QList<Bezier3*> segments;
    //QList<LineSegment*> polygon;
    GLManager*man;
    void reset();
    void clean();
    void update();
    QVector<glm::vec4> solveInterpolation(const QVector<PointCAM*>&);
public:
    InterCurve(QVector<PointCAM*>,GLManager*, QObject *parent = 0);
    ~InterCurve();
    void append(PointCAM*p);

signals:

public slots:
    void updateRemoved(PointCAM*);
    void updateChanged(PointCAM*);

};

#endif // INTERCURVE_H
