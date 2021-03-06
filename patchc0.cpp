#include "patchc0.h"
#include "glmanager.h"

PatchC0::PatchC0(glm::vec4 center,
                 int n,int m,bool wrap,
                 float width, float height,
                 GLManager*manager,
                 QObject *parent) :
    QObject(parent),
    points_u(3*n+1),
    points_v(3*m+1),
    patches_u(n),
    patches_v(m),
    wrap(wrap),
    man(manager)
{
    //generate points
    float tau = glm::two_pi<float>();
    float y = center.y;
    for(int i=0;i<points_u;++i){
        float u = i/(3.0*patches_u);
        float x = center.x + (2*u-1)*width;
        if(wrap){
            x = center.x + width*cos(u*tau);
            y = center.y + width*sin(u*tau);
        }
        for(int j=0;j<points_v;++j){
            if(i == 3*patches_u && wrap){
                control.append(control[j]);
            }else{
                float v = j/(3.0*patches_v);
                float z = center.z + (2*v-1)*height;
                PointCAM*p = new PointCAM(glm::vec3{x,y,z},man);
                man->addPoint(p);
                man->addDrawable(p);
                control.append(p);
            }
        }
    }
    //generate control polygon
    for(int i=0;i<(points_u-wrap);++i){
        for(int j=0;j<(points_v-1);++j){
            LineSegment* l1 = new LineSegment({control[i*points_v+j],
                                               control[i*points_v+j+1]});
            man->addDrawable(l1);
            controlPolygon.append(l1);
        }
    }
    for(int i=0;i<(points_u-1);++i){
        for(int j=0;j<(points_v);++j){
            LineSegment* l1 = new LineSegment({control[i*points_v+j],
                                               control[(i+1)*points_v+j]});
            man->addDrawable(l1);
            controlPolygon.append(l1);
        }
    }
    //generate obligatory ribs
    for(int i=0;i<(patches_u+!wrap);++i){
        for(int j=0;j<patches_v;++j){
            Bezier3* bezier = new Bezier3({control[3*i*points_v+3*j],
                                          control[3*i*points_v+3*j+1],
                                          control[3*i*points_v+3*j+2],
                                          control[3*i*points_v+3*j+3]});
            man->addDrawable(bezier);
            segments.append(bezier);
        }
    }
    for(int i=0;i<patches_u;++i){
        for(int j=0;j<patches_v+1;++j){
            Bezier3* bezier = new Bezier3({control[3*i*points_v+3*j],
                                          control[(3*i+1)*points_v+3*j],
                                          control[(3*i+2)*points_v+3*j],
                                          control[(3*i+3)*points_v+3*j]});
            man->addDrawable(bezier);
            segments.append(bezier);
        }
    }
    const_ribs=segments.size();
    //?generate internal ribs?
    ribs_u = ribs_v = 0;//zero until decided default
}


PatchC0::~PatchC0()
{
    for(int i=0;i<controlPolygon.size();++i){
        man->removeDrawable(controlPolygon[i]);
        delete controlPolygon[i];
    }
    controlPolygon.clear();

    //safely remove internal ribs
    for(int i=0;i<control.size();++i){
        disconnect(qobject_cast<PointCAM*>(control[i]),0,0,0);
        qobject_cast<PointCAM*>(control[i])->disconnect();
    }
    for(int i=const_ribs;i<segments.size();++i){
        man->removeDrawable(segments[i]);
        //internal ribs own their points
        for(int j=0;j<4;++j){
            PointCAM*p = segments[i]->children()[j];
            man->removePoint(p);
            delete p;
        }
        delete segments[i];
    }
    segments.erase(segments.begin()+const_ribs,segments.end());
    //remove obligatory ribs
    for(int i=0;i<segments.size();++i){
        man->removeDrawable(segments[i]);
        delete segments[i];
    }
    segments.clear();
    //we own all points
    for(int i=0;i<control.size()-(wrap*points_v);++i){
        man->removeDrawable(control[i]);
        man->removePoint(control[i]);
        delete control[i];
    }
    control.clear();
}

void PatchC0::updateChanged(PointCAM *)
{
}

void PatchC0::updateRibsChanged(int udiv, int vdiv)
{
    for(int i=0;i<control.size();++i){
        disconnect(qobject_cast<PointCAM*>(control[i]),0,0,0);
        qobject_cast<PointCAM*>(control[i])->disconnect();
    }
    for(int i=const_ribs;i<segments.size();++i){
        man->removeDrawable(segments[i]);
    }
    //make more beziers if necessary
    {
        int oldsize = segments.size();
        int newsize = const_ribs+patches_u*patches_v*(udiv+vdiv);
        if(newsize > oldsize){
            segments.reserve(newsize);
        }
        for(int i = oldsize; i<newsize; ++i){
            PointCAM*ps[4];
            for(int j=0; j<4; ++j){
                ps[j]=new PointCAM(glm::vec3{0,0,0},man);
            }
            Bezier3*bezier=new Bezier3({ps[0],ps[1],ps[2],ps[3]});
            segments.append(bezier);
        }
    }
    //update internal positions and connections
    QVector<Bezier3*>::iterator bez_it=segments.begin();
    bez_it+=const_ribs;
    for(int i=0;i<patches_u;++i){
        for(int j=0;j<patches_v;++j){
            for(int k=0;k<udiv;++k){
                float u=(k+1.0)/(udiv+1.0);
                float t = 1-u;
                float b0=t*t*t,b1=3*t*t*u,b2=3*t*u*u,b3=u*u*u;
                Bezier3*bezier = *bez_it;
                QList<PointCAM*> children = bezier->children();
                for(int l=0;l<4;++l){
                    children[l]->setPos(glm::vec3{
                                           control[3*i*points_v+3*j+l]->pos()*b0+
                                           control[(3*i+1)*points_v+3*j+l]->pos()*b1+
                                           control[(3*i+2)*points_v+3*j+l]->pos()*b2+
                                           control[(3*i+3)*points_v+3*j+l]->pos()*b3
                                       });
                    auto p =children[l];
                    auto update=[this,b0,b1,b2,b3,i,j,l,p](PointCAM*){
                        p->setPos(glm::vec3{
                                      control[3*i*points_v+3*j+l]->pos()*b0+
                                      control[(3*i+1)*points_v+3*j+l]->pos()*b1+
                                      control[(3*i+2)*points_v+3*j+l]->pos()*b2+
                                      control[(3*i+3)*points_v+3*j+l]->pos()*b3
                                  });
                    };
                    man->addPoint(p);
                    connect(control[3*i*points_v+3*j+l],&PointCAM::pointChanged,
                            update);
                    connect(control[(3*i+1)*points_v+3*j+l],&PointCAM::pointChanged,
                            update);
                    connect(control[(3*i+2)*points_v+3*j+l],&PointCAM::pointChanged,
                            update);
                    connect(control[(3*i+3)*points_v+3*j+l],&PointCAM::pointChanged,
                            update);
                }
                man->addDrawable(bezier);
                ++bez_it;
            }
            for(int k=0;k<vdiv;++k){
                float v=(k+1.0)/(vdiv+1.0);
                float t = 1-v;
                float b0=t*t*t,b1=3*t*t*v,b2=3*t*v*v,b3=v*v*v;
                Bezier3*bezier = *bez_it;
                QList<PointCAM*> children = bezier->children();
                for(int l=0;l<4;++l){
                    children[l]->setPos(glm::vec3{
                                           control[(3*i+l)*points_v+3*j]->pos()*b0+
                                           control[(3*i+l)*points_v+3*j+1]->pos()*b1+
                                           control[(3*i+l)*points_v+3*j+2]->pos()*b2+
                                           control[(3*i+l)*points_v+3*j+3]->pos()*b3
                                       });
                    auto p =children[l];
                    auto update=[this,b0,b1,b2,b3,i,j,l,p](PointCAM*){
                        p->setPos(glm::vec3{
                                      control[(3*i+l)*points_v+3*j]->pos()*b0+
                                      control[(3*i+l)*points_v+3*j+1]->pos()*b1+
                                      control[(3*i+l)*points_v+3*j+2]->pos()*b2+
                                      control[(3*i+l)*points_v+3*j+3]->pos()*b3
                                  });
                    };
                    man->addPoint(p);
                    connect(control[(3*i+l)*points_v+3*j],&PointCAM::pointChanged,
                            update);
                    connect(control[(3*i+l)*points_v+3*j+1],&PointCAM::pointChanged,
                            update);
                    connect(control[(3*i+l)*points_v+3*j+2],&PointCAM::pointChanged,
                            update);
                    connect(control[(3*i+l)*points_v+3*j+3],&PointCAM::pointChanged,
                            update);
                }
                man->addDrawable(bezier);
                ++bez_it;
            }

        }
    }
    ribs_u=udiv;
    ribs_v=vdiv;
}
