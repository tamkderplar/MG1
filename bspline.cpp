#include "bspline.h"
#include "glmanager.h"

BSpline::BSpline(QVector<PointCAM*> controlPoints,
                 GLManager* manager,
                 QObject *parent) :
    QObject(parent),
    man(manager),
    bezierBasis(false)
{
    if(controlPoints.size()==0){
        throw std::logic_error("empty control points");
    }
    for(int i=0;i<controlPoints.size();++i){
        control.append(controlPoints[i]);
        connect(controlPoints[i],&PointCAM::pointChanged,
                this,&BSpline::updateChanged);
        connect(controlPoints[i],&PointCAM::pointRemoved,
                this,&BSpline::updateRemoved);
    }
    genBSplineSegments();
    genBSplinePolygon();
}

BSpline::~BSpline()
{
    clearPolygon();
    clearSegments();
}

void BSpline::append(PointCAM *p)
{
    //TODO:?do not reset, just append?
    clearPolygon();
    clearSegments();
    control.append(p);
    connect(p,&PointCAM::pointChanged,
            this,&BSpline::updateChanged);
    connect(p,&PointCAM::pointRemoved,
            this,&BSpline::updateRemoved);
    bezierBasis?genBezierSegments():genBSplineSegments();
    bezierBasis?genBezierPolygon():genBSplinePolygon();
}

void BSpline::changeBasis(bool showBezierBasis)
{
    if(showBezierBasis != bezierBasis){
        bezierBasis = showBezierBasis;
        if(showBezierBasis){
            genBezierSegments();
            genBezierPolygon();
        }else{
            genBSplineSegments();
            genBSplinePolygon();
        }
    }
}

void BSpline::clearPolygon()
{
    for(int i=0;i<controlPolygon.size();++i){
        man->removeDrawable(controlPolygon[i]);
        delete controlPolygon[i];
    }
    controlPolygon.clear();
}

void BSpline::clearSegments()
{
    if(segments.size()!=0){
        //segments have their own points
        //  ,so remove those too
        PointCAM*p0=segments[0]->children()[0];
        man->removePoint(p0);
        man->removeDrawable(p0);
        delete p0;
        for(int i=0;i<segments.size();++i){
            PointCAM*p1=segments[i]->children()[1];
            PointCAM*p2=segments[i]->children()[2];
            PointCAM*p3=segments[i]->children()[3];
            man->removeDrawable(segments[i]);
            man->removePoint(p1);
            man->removePoint(p2);
            man->removePoint(p3);
            man->removeDrawable(p1);
            man->removeDrawable(p2);
            man->removeDrawable(p3);
            delete p1;
            delete p2;
            delete p3;
            delete segments[i];
        }
        segments.clear();
    }
    if(segments_b.size()!=0){
        for(int i=0;i<segments_b.size();++i){
            man->removeDrawable(segments_b[i]);
            delete segments_b[i];
        }
        segments_b.clear();
    }
}

void BSpline::genBSplinePolygon()
{
    clearPolygon();
    for(int i=1;i<control.size();++i){
        LineSegment*l1 = new LineSegment({control[i-1],control[i]});
        controlPolygon.append(l1);
        man->addDrawable(l1);
    }
}

void BSpline::genBezierPolygon()
{
    clearPolygon();
    for(int i=0;i<segments.size();++i){
        PointCAM* p0 = segments[i]->children()[0];
        PointCAM* p1 = segments[i]->children()[1];
        PointCAM* p2 = segments[i]->children()[2];
        PointCAM* p3 = segments[i]->children()[3];
        LineSegment*l1= new LineSegment({p0,p1}),
                *l2 = new LineSegment({p1,p2}),
                *l3 = new LineSegment({p2,p3});
        controlPolygon.append({l1,l2,l3});
        man->addDrawable(l1);
        man->addDrawable(l2);
        man->addDrawable(l3);
    }
}

void BSpline::genBSplineSegments()
{
    clearSegments();
    if(control.size()<3){
        return;
    }
    for(int i=3;i<control.size();++i){
        BSplineBasis* spline = new BSplineBasis({control[i-3],control[i-2],
                                                 control[i-1],control[i]});
        man->addDrawable<BSplineBasis>(spline);
        segments_b.append(spline);
    }
}

void BSpline::genBezierSegments()
{
    clearSegments();
    if(control.size()<3){
        return;
    }
    PointCAM* p0 = new PointCAM(
                glm::vec3{(control[0]->pos()+4.0f*control[1]->pos()+control[2]->pos())/6.0f},
                man);
    man->addPoint(p0);
    man->addDrawable(p0);
    connect(p0,&PointCAM::pointChanged,
            [this](PointCAM*p){control[1]->setPos(
                    glm::vec3{p->pos()*1.5f-
                        (control[0]->pos()+control[2]->pos())/4.0f
                    });});

    for(int i=3;i<control.size();++i){
        PointCAM* p1 = new PointCAM(glm::vec3{(2.0f*control[i-2]->pos()+control[i-1]->pos())/3.0f},man);
        PointCAM* p2 = new PointCAM(glm::vec3{(control[i-2]->pos()+2.0f*control[i-1]->pos())/3.0f},man);
        PointCAM* p3 = new PointCAM(glm::vec3{(control[i-2]->pos()+4.0f*control[i-1]->pos()+control[i]->pos())/6.0f},man);
        man->addPoint(p1);
        man->addPoint(p2);
        man->addPoint(p3);
        Bezier3* bezier = new Bezier3({p0,p1,p2,p3});
        man->addDrawable<Bezier3>(bezier);
        segments.append(bezier);

        man->addDrawable(p1);
        man->addDrawable(p2);
        man->addDrawable(p3);
        connect(p1,&PointCAM::pointChanged,
                [this,i](PointCAM*p){control[i-2]->setPos(
                        glm::vec3{p->pos()*1.5f-control[i-1]->pos()*0.5f
                        });});
        connect(p2,&PointCAM::pointChanged,
                [this,i](PointCAM*p){control[i-1]->setPos(
                        glm::vec3{p->pos()*1.5f-control[i-2]->pos()*0.5f
                        });});
        connect(p3,&PointCAM::pointChanged,
                [this,i](PointCAM*p){control[i-1]->setPos(
                        glm::vec3{p->pos()*1.5f-
                            (control[i-2]->pos()+control[i]->pos())/4.0f
                        });});

        p0=p3;
    }
}

void BSpline::updateRemoved(PointCAM *p)
{
    //clear polygon and all segments before reset
    clearPolygon();
    clearSegments();

    control.removeAll(p);
    if(control.size()==0){
        //emit destroyMe(this);
        return;
    }

    bezierBasis?genBezierSegments():genBSplineSegments();
    bezierBasis?genBezierPolygon():genBSplinePolygon();
}

void BSpline::updateChanged(PointCAM *p)
{
    if(bezierBasis){
        for(int i=0;i<control.size();++i){
            if(p==control[i]){
                for(int j=0;j<4;++j){
                    int id = i+j-3;
                    if(id<0||id>=segments.size()){
                        continue;
                    }
                    auto list = segments[id]->children();
                    list[0]->blockSignals(true);
                    list[1]->blockSignals(true);
                    list[2]->blockSignals(true);
                    list[3]->blockSignals(true);
                    list[0]->setPos(glm::vec3{(control[id]->pos()+4.0f*control[id+1]->pos()+control[id+2]->pos())/6.0f});
                    list[1]->setPos(glm::vec3{(2.0f*control[id+1]->pos()+control[id+2]->pos())/3.0f});
                    list[2]->setPos(glm::vec3{(control[id+1]->pos()+2.0f*control[id+2]->pos())/3.0f});
                    list[3]->setPos(glm::vec3{(control[id+1]->pos()+4.0f*control[id+2]->pos()+control[id+3]->pos())/6.0f});
                    list[0]->blockSignals(false);
                    list[1]->blockSignals(false);
                    list[2]->blockSignals(false);
                    list[3]->blockSignals(false);
                }
            }
        }
    }else{//bspline basis

    }
}
