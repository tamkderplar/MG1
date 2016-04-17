#include "bezier3c0.h"
#include "glmanager.h"

Bezier3C0::Bezier3C0(QVector<PointCAM*> controlPoints,
                     GLManager* manager,
                     QObject *parent) :
    QObject(parent),
    man(manager)
{
    if(controlPoints.size()==0){
        throw std::logic_error("empty control points");
    }
    for(int i=0;i<controlPoints.size();++i){
        control.append(controlPoints[i]);
        connect(controlPoints[i],&PointCAM::pointChanged,
                this,&Bezier3C0::updateChanged);
        connect(controlPoints[i],&PointCAM::pointRemoved,
                this,&Bezier3C0::updateRemoved);
    }
    genBezierSegments();
    genBezierPolygon();
}

Bezier3C0::~Bezier3C0()
{
    clearPolygon();
    clearSegments();
}

void Bezier3C0::append(PointCAM *p)
{
    //TODO:?do not reset, just append?
    clearPolygon();
    clearSegments();
    control.append(p);
    connect(p,&PointCAM::pointChanged,
            this,&Bezier3C0::updateChanged);
    connect(p,&PointCAM::pointRemoved,
            this,&Bezier3C0::updateRemoved);
    genBezierSegments();
    genBezierPolygon();
}

void Bezier3C0::clearPolygon()
{
    for(int i=0;i<controlPolygon.size();++i){
        man->removeDrawable(controlPolygon[i]);
        delete controlPolygon[i];
    }
    controlPolygon.clear();
}

void Bezier3C0::clearSegments()
{
    if(segments.size()==0){
        return;
    }
    for(int i=0;i<segments.size();++i){
        man->removeDrawable(segments[i]);
        delete segments[i];
    }
    segments.clear();
}

void Bezier3C0::genBezierPolygon()
{
    clearPolygon();
    for(int i=1;i<control.size();++i){
        LineSegment*l1 = new LineSegment({control[i-1],control[i]});
        controlPolygon.append(l1);
        man->addDrawable(l1);
    }
}

void Bezier3C0::genBezierSegments()
{
    for(int i=3;i<control.size();i+=3){
        Bezier3* bezier = new Bezier3({control[i-3],control[i-2],
                                       control[i-1],control[i]});
        man->addDrawable<Bezier3>(bezier);
        segments.append(bezier);
    }
    int n = control.size();
    if(n%3 == 0){
        Bezier3* bezier = new Bezier3({control[n-3],control[n-2],
                                       control[n-1],control[n-1]});
        man->addDrawable<Bezier3>(bezier);
        segments.append(bezier);
    }else if(n%3 == 2){
        Bezier3* bezier = new Bezier3({control[n-2],control[n-1],
                                       control[n-1],control[n-1]});
        man->addDrawable<Bezier3>(bezier);
        segments.append(bezier);
    }
}

void Bezier3C0::updateRemoved(PointCAM *p)
{
    //clear polygon and all segments before reset
    clearPolygon();
    clearSegments();

    control.removeAll(p);
    if(control.size()==0){
        return;
    }

    genBezierSegments();
    genBezierPolygon();
}

void Bezier3C0::updateChanged(PointCAM *)
{

}

