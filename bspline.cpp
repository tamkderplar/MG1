#include "bspline.h"
#include "glmanager.h"

BSpline::BSpline(QVector<PointCAM*> controlPoints,
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
        connect(controlPoints[i],&PointCAM::pointChanged,this,&BSpline::updateChanged);
    }
    if(control.size()<3){
        return;
    }
    PointCAM* p0 = new PointCAM(
                glm::vec3{(control[0]->pos()+4.0f*control[1]->pos()+control[2]->pos())/6.0f},
                man);
    man->addPoint(p0);
    for(int i=3;i<controlPoints.size();++i){
        PointCAM* p1 = new PointCAM(glm::vec3{(2.0f*control[i-2]->pos()+control[i-1]->pos())/3.0f},man);
        PointCAM* p2 = new PointCAM(glm::vec3{(control[i-2]->pos()+2.0f*control[i-1]->pos())/3.0f},man);
        PointCAM* p3 = new PointCAM(glm::vec3{(control[i-2]->pos()+4.0f*control[i-1]->pos()+control[i]->pos())/6.0f},man);
        man->addPoint(p1);
        man->addPoint(p2);
        man->addPoint(p3);
        Bezier3* bezier = new Bezier3({p0,p1,p2,p3});
        man->addDrawable<Bezier3>(bezier);
        segments.append(bezier);
        p0=p3;
    }
}

void BSpline::updateRemoved(PointCAM *p)
{
    /*for(int i=0;i<control.size();++i){
        if(p==control[i]){
            for(int j=0;j<4;++j){
                int id = i+j-3;
                if(id<0||id>=segments.size()){
                    continue;
                }
                segments[id]->children()[0]->setPos(glm::vec3{(control[id]->pos()+4.0f*control[id+1]->pos()+control[id+2]->pos())/6.0f});
                segments[id]->children()[1]->setPos(glm::vec3{(2.0f*control[id+1]->pos()+control[id+2]->pos())/3.0f});
                segments[id]->children()[2]->setPos(glm::vec3{(control[id+1]->pos()+2.0f*control[id+2]->pos())/3.0f});
                segments[id]->children()[3]->setPos(glm::vec3{(control[id+1]->pos()+4.0f*control[id+2]->pos()+control[id+3]->pos())/6.0f});
            }
        }
    }*/
}

void BSpline::updateChanged(PointCAM *p)
{
    for(int i=0;i<control.size();++i){
        if(p==control[i]){
            for(int j=0;j<4;++j){
                int id = i+j-3;
                if(id<0||id>=segments.size()){
                    continue;
                }
                segments[id]->children()[0]->setPos(glm::vec3{(control[id]->pos()+4.0f*control[id+1]->pos()+control[id+2]->pos())/6.0f});
                segments[id]->children()[1]->setPos(glm::vec3{(2.0f*control[id+1]->pos()+control[id+2]->pos())/3.0f});
                segments[id]->children()[2]->setPos(glm::vec3{(control[id+1]->pos()+2.0f*control[id+2]->pos())/3.0f});
                segments[id]->children()[3]->setPos(glm::vec3{(control[id+1]->pos()+4.0f*control[id+2]->pos()+control[id+3]->pos())/6.0f});
            }
        }
    }
}
