#ifndef BSPLINE_H
#define BSPLINE_H

#include <QObject>
#include "pointcam.h"
#include "bezier3.h"

class BSpline : public QObject
{
    Q_OBJECT

    QList<const PointCAM*> control;
    //QList<BSplineBasis> segments;
    QList<Bezier3*> segments;
    GLManager*man;
public:
    explicit BSpline(QVector<PointCAM*>,GLManager*, QObject *parent = 0);

signals:

public slots:
    void updateRemoved(PointCAM*);
    void updateChanged(PointCAM*);

};

#endif // BSPLINE_H
