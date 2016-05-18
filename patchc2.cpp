#include "patchc2.h"
#include "glmanager.h"

PatchC2::PatchC2(glm::vec4 center,
                 int n,int m,bool wrap,
                 float width, float height,
                 GLManager*manager,
                 QObject *parent) :
    QObject(parent),
    points_u(n+3),
    points_v(m+3),
    patches_u(n),
    patches_v(m),
    wrap(wrap),
    man(manager)
{
    //generate points
    float tau = glm::two_pi<float>();
    float y = center.y;
    for(int i=0;i<points_u;++i){
        float u = i/(2.0+patches_u);
        float x = center.x + (2*u-1)*width;
        if(wrap){
            u = i/(float)patches_u;
            x = center.x + width*cos(u*tau);
            y = center.y + width*sin(u*tau);
        }
        for(int j=0;j<points_v;++j){
            if(i >= patches_u && wrap){
                control.append(control[(i-patches_u)*points_v+j]);
            }else{
                float v = j/(2.0+patches_v);
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
            PointCAM*ps[4];
            for(int l=0;l<4;++l){
                ps[l] = new PointCAM(glm::vec3{
                                         control[i*points_v+j+l]->pos()/6.0f+
                                         control[(i+1)*points_v+j+l]->pos()/1.5f+
                                         control[(i+2)*points_v+j+l]->pos()/6.0f
                                     },man);
                auto p = ps[l];
                auto update=[this,i,j,l,p](PointCAM*){
                    p->setPos(glm::vec3{
                                  control[i*points_v+j+l]->pos()/6.0f+
                                  control[(i+1)*points_v+j+l]->pos()/1.5f+
                                  control[(i+2)*points_v+j+l]->pos()/6.0f
                              });
                };
                connect(control[i*points_v+j+l],&PointCAM::pointChanged,
                        update);
                connect(control[(i+1)*points_v+j+l],&PointCAM::pointChanged,
                        update);
                connect(control[(i+2)*points_v+j+l],&PointCAM::pointChanged,
                        update);
                man->addPoint(ps[l]);
            }
            BSplineBasis* bspline = new BSplineBasis({ps[0],ps[1],ps[2],ps[3]});
            man->addDrawable(bspline);
            segments.append(bspline);
        }
    }
    for(int i=0;i<patches_u;++i){
        for(int j=0;j<patches_v+1;++j){
            PointCAM*ps[4];
            for(int l=0;l<4;++l){
                ps[l] = new PointCAM(glm::vec3{
                                         control[(i+l)*points_v+j]->pos()/6.0f+
                                         control[(i+l)*points_v+j+1]->pos()/1.5f+
                                         control[(i+l)*points_v+j+2]->pos()/6.0f
                                     },man);
                auto p = ps[l];
                auto update=[this,i,j,l,p](PointCAM*){
                    p->setPos(glm::vec3{
                                  control[(i+l)*points_v+j]->pos()/6.0f+
                                  control[(i+l)*points_v+j+1]->pos()/1.5f+
                                  control[(i+l)*points_v+j+2]->pos()/6.0f
                              });
                };
                connect(control[(i+l)*points_v+j],&PointCAM::pointChanged,
                        update);
                connect(control[(i+l)*points_v+j+1],&PointCAM::pointChanged,
                        update);
                connect(control[(i+l)*points_v+j+2],&PointCAM::pointChanged,
                        update);
                man->addPoint(ps[l]);
            }
            BSplineBasis* bspline = new BSplineBasis({ps[0],ps[1],ps[2],ps[3]});
            man->addDrawable(bspline);
            segments.append(bspline);
        }
    }
    const_ribs=segments.size();
    //?generate internal ribs?
    ribs_u = ribs_v = 0;//zero until decided default
}


PatchC2::~PatchC2()
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
        for(int j=0;j<4;++j){
            PointCAM*p = segments[i]->children()[j];
            man->removePoint(p);
            delete p;
        }
        delete segments[i];
    }
    segments.clear();
    //we own all points
    for(int i=0;i<control.size()-(wrap*3*points_v);++i){
        man->removeDrawable(control[i]);
        man->removePoint(control[i]);
        delete control[i];
    }
    control.clear();
}

void PatchC2::updateChanged(PointCAM *)
{
}

void PatchC2::updateRibsChanged(int udiv, int vdiv)
{
    for(int i=0;i<control.size();++i){
        disconnect(qobject_cast<PointCAM*>(control[i]),0,0,0);
        qobject_cast<PointCAM*>(control[i])->disconnect();
    }
    for(int i=const_ribs;i<segments.size();++i){
        man->removeDrawable(segments[i]);
    }
    //make more bsplines if necessary
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
            BSplineBasis*bspline=new BSplineBasis({ps[0],ps[1],ps[2],ps[3]});
            segments.append(bspline);
        }
    }
    //update obligatory connections
    QVector<BSplineBasis*>::iterator bez_it=segments.begin();
    for(int i=0;i<(patches_u+!wrap);++i){
        for(int j=0;j<patches_v;++j){
            auto bspline = *bez_it;
            auto children = bspline->children();
            for(int l=0;l<4;++l){
                auto p = children[l];
                auto update=[this,i,j,l,p](PointCAM*){
                    p->setPos(glm::vec3{
                                  control[i*points_v+j+l]->pos()/6.0f+
                                  control[(i+1)*points_v+j+l]->pos()/1.5f+
                                  control[(i+2)*points_v+j+l]->pos()/6.0f
                              });
                };
                connect(control[i*points_v+j+l],&PointCAM::pointChanged,
                        update);
                connect(control[(i+1)*points_v+j+l],&PointCAM::pointChanged,
                        update);
                connect(control[(i+2)*points_v+j+l],&PointCAM::pointChanged,
                        update);
            }
            ++bez_it;
        }
    }
    for(int i=0;i<patches_u;++i){
        for(int j=0;j<patches_v+1;++j){
            auto bspline = *bez_it;
            auto children = bspline->children();
            for(int l=0;l<4;++l){
                auto p = children[l];
                auto update=[this,i,j,l,p](PointCAM*){
                    p->setPos(glm::vec3{
                                  control[(i+l)*points_v+j]->pos()/6.0f+
                                  control[(i+l)*points_v+j+1]->pos()/1.5f+
                                  control[(i+l)*points_v+j+2]->pos()/6.0f
                              });
                };
                connect(control[(i+l)*points_v+j],&PointCAM::pointChanged,
                        update);
                connect(control[(i+l)*points_v+j+1],&PointCAM::pointChanged,
                        update);
                connect(control[(i+l)*points_v+j+2],&PointCAM::pointChanged,
                        update);
            }
            ++bez_it;
        }
    }
    //update internal positions and connections
    for(int i=0;i<patches_u;++i){
        for(int j=0;j<patches_v;++j){
            for(int k=0;k<udiv;++k){
                float u=(k+1.0)/(udiv+1.0);
                float t = 1-u;
                float b0=t*t*t/6,b1=2.0/3-t*u*u-0.5*u*u*u,
                        b2=2.0/3-0.5*t*t*t-t*t*u,b3=u*u*u/6;
                BSplineBasis*bspline = *bez_it;
                QList<PointCAM*> children = bspline->children();
                for(int l=0;l<4;++l){
                    children[l]->setPos(glm::vec3{
                                           control[i*points_v+j+l]->pos()*b0+
                                           control[(i+1)*points_v+j+l]->pos()*b1+
                                           control[(i+2)*points_v+j+l]->pos()*b2+
                                           control[(i+3)*points_v+j+l]->pos()*b3
                                       });
                    auto p =children[l];
                    auto update=[this,b0,b1,b2,b3,i,j,l,p](PointCAM*){
                        p->setPos(glm::vec3{
                                      control[i*points_v+j+l]->pos()*b0+
                                      control[(i+1)*points_v+j+l]->pos()*b1+
                                      control[(i+2)*points_v+j+l]->pos()*b2+
                                      control[(i+3)*points_v+j+l]->pos()*b3
                                  });
                    };
                    man->addPoint(p);
                    connect(control[i*points_v+j+l],&PointCAM::pointChanged,
                            update);
                    connect(control[(i+1)*points_v+j+l],&PointCAM::pointChanged,
                            update);
                    connect(control[(i+2)*points_v+j+l],&PointCAM::pointChanged,
                            update);
                    connect(control[(i+3)*points_v+j+l],&PointCAM::pointChanged,
                            update);
                }
                man->addDrawable(bspline);
                ++bez_it;
            }
            for(int k=0;k<vdiv;++k){
                float v=(k+1.0)/(vdiv+1.0);
                float t = 1-v;
                float b0=t*t*t/6,b1=2.0/3-t*v*v-0.5*v*v*v,
                        b2=2.0/3-0.5*t*t*t-t*t*v,b3=v*v*v/6;
                BSplineBasis*bspline = *bez_it;
                QList<PointCAM*> children = bspline->children();
                for(int l=0;l<4;++l){
                    children[l]->setPos(glm::vec3{
                                           control[(i+l)*points_v+j]->pos()*b0+
                                           control[(i+l)*points_v+j+1]->pos()*b1+
                                           control[(i+l)*points_v+j+2]->pos()*b2+
                                           control[(i+l)*points_v+j+3]->pos()*b3
                                       });
                    auto p =children[l];
                    auto update=[this,b0,b1,b2,b3,i,j,l,p](PointCAM*){
                        p->setPos(glm::vec3{
                                      control[(i+l)*points_v+j]->pos()*b0+
                                      control[(i+l)*points_v+j+1]->pos()*b1+
                                      control[(i+l)*points_v+j+2]->pos()*b2+
                                      control[(i+l)*points_v+j+3]->pos()*b3
                                  });
                    };
                    man->addPoint(p);
                    connect(control[(i+l)*points_v+j],&PointCAM::pointChanged,
                            update);
                    connect(control[(i+l)*points_v+j+1],&PointCAM::pointChanged,
                            update);
                    connect(control[(i+l)*points_v+j+2],&PointCAM::pointChanged,
                            update);
                    connect(control[(i+l)*points_v+j+3],&PointCAM::pointChanged,
                            update);
                }
                man->addDrawable(bspline);
                ++bez_it;
            }

        }
    }
    ribs_u=udiv;
    ribs_v=vdiv;
}

