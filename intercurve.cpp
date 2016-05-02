#include "intercurve.h"
#include "glmanager.h"
#include "bandmatrix.h"

InterCurve::InterCurve(QVector<PointCAM*> controlPoints,
                       GLManager* manager,
                       QObject *parent):
    QObject(parent),
    man(manager)
{
    if(controlPoints.size()==0){
        throw std::logic_error("empty control points");
    }
    for(int i=0;i<controlPoints.size();++i){
        control.append(controlPoints[i]);
        connect(controlPoints[i],&PointCAM::pointChanged,
                this,&InterCurve::updateChanged);
        connect(controlPoints[i],&PointCAM::pointRemoved,
                this,&InterCurve::updateRemoved);
    }
    reset();
}


InterCurve::~InterCurve()
{
    clean();
}

void InterCurve::append(PointCAM *p)
{
    control.append(p);
    connect(p,&PointCAM::pointChanged,
            this,&InterCurve::updateChanged);
    connect(p,&PointCAM::pointRemoved,
            this,&InterCurve::updateRemoved);
    reset();
}

void InterCurve::updateRemoved(PointCAM *p)
{
    control.removeAll(p);
    reset();
}

void InterCurve::updateChanged(PointCAM *p)
{
    if(control.contains(p))
        update();
}

void InterCurve::reset()
{
    clean();
    int n = control.size();
    if(n<2){
        return;
    }
    /*//option 1
    QVector<PointCAM*> input;
    QVector<glm::vec4> v;
    for(int i=0;i<control.size();++i){
        if(input.size()==2 && input[0]->pos()==input[1]->pos()){
            v.append(input[0]->pos());
            v.append(input[0]->pos());
            input.clear();
            input.append(control[i-1]);
            input.append(control[i]);
        }else
        if(input.size()>1 && control[i]->pos()==input.last()->pos()){
            v.append(solveInterpolation(input));
            input.clear();
            input.append(control[i-1]);
            input.append(control[i]);
        }else{
            input.append(control[i]);
        }
    }
    v.append(solveInterpolation(input));
    //*/
    //option 2
    QVector<PointCAM*> input;
    QVector<glm::vec4> w;
    for(int i=0;i<control.size();++i){
        if(input.size() && input.last()->pos()==control[i]->pos()){
            continue;
        }
        input.append(control[i]);
    }
    w = solveInterpolation(input);
    QVector<glm::vec4> v;
    for(int i=0,j=0;i<n-1;++i){
        if(control[i]->pos()==control[i+1]->pos()){
            v.append(control[i]->pos());
            v.append(control[i]->pos());
        }else{
            v.append(w[j]);
            v.append(w[j+1]);
            j+=2;
        }
    }
    //option 3
    //v = solveInterpolation(control);
    //
    for(int i=0;i<n-1;++i){
        PointCAM*b1 = new PointCAM(glm::vec3{v[2*i]},man);
        PointCAM*b2 = new PointCAM(glm::vec3{v[2*i+1]},man);
        man->addPoint(b1);
        man->addPoint(b2);
        Bezier3*bezier = new Bezier3({control[i],b1,
                                     b2,control[i+1]});
        man->addDrawable(bezier);
        segments.append(bezier);

        /*LineSegment*l1 = new LineSegment({control[i],b1});
        LineSegment*l2 = new LineSegment({b1,b2});
        LineSegment*l3 = new LineSegment({b2,control[i+1]});
        man->addDrawable(l1);
        man->addDrawable(l2);
        man->addDrawable(l3);
        polygon.append(l1);
        polygon.append(l2);
        polygon.append(l3);*/
    }
}

void InterCurve::clean()
{
    for(int i=0;i<segments.size();++i){
        /*man->removeDrawable(polygon[3*i]);
        man->removeDrawable(polygon[3*i+1]);
        man->removeDrawable(polygon[3*i+2]);
        delete polygon[3*i];
        delete polygon[3*i+1];
        delete polygon[3*i+2];*/

        man->removePoint(segments[i]->children()[1]);
        //delete segments[i]->children()[1];
        man->removePoint(segments[i]->children()[2]);
        //delete segments[i]->children()[2];
        man->removeDrawable(segments[i]);
        delete segments[i];
    }
    segments.clear();
    //polygon.clear();
}

void InterCurve::update()
{
    int n = control.size();
    if(n<2){
        return;
    }
    /*//option 1
    QVector<PointCAM*> input;
    QVector<glm::vec4> v;
    for(int i=0;i<control.size();++i){
        if(input.size()==2 && input[0]->pos()==input[1]->pos()){
            v.append(input[0]->pos());
            v.append(input[0]->pos());
            input.clear();
            input.append(control[i-1]);
            input.append(control[i]);
        }else
        if(input.size()>1 && control[i]->pos()==input.last()->pos()){
            v.append(solveInterpolation(input));
            input.clear();
            input.append(control[i-1]);
            input.append(control[i]);
        }else{
            input.append(control[i]);
        }
    }
    v.append(solveInterpolation(input));
    //*/
    //option 2
    QVector<PointCAM*> input;
    QVector<glm::vec4> w;
    for(int i=0;i<control.size();++i){
        if(input.size() && 1e-10>glm::distance(input.last()->pos(),
                                               control[i]->pos())){
            continue;
        }
        input.append(control[i]);
    }
    w = solveInterpolation(input);
    QVector<glm::vec4> v;
    for(int i=0,j=0;i<n-1;++i){
        if(1e-10>glm::distance(control[i]->pos(),
                         control[i+1]->pos())){
            v.append(control[i]->pos());
            v.append(control[i]->pos());
        }else{
            v.append(w[j]);
            v.append(w[j+1]);
            j+=2;
        }
    }
    //
    //option 3
    //v = solveInterpolation(control);
    //
    for(int i=0;i<n-1;++i){
        auto b1 = segments[i]->children()[1];
        auto b2 = segments[i]->children()[2];
        //b1->blockSignals(true);
        //b2->blockSignals(true);
        b1->setPos(glm::vec3{v[2*i]});
        b2->setPos(glm::vec3{v[2*i+1]});
        //b1->blockSignals(false);
        //b2->blockSignals(false);
    }
}

QVector<glm::vec4> InterCurve::solveInterpolation(const QVector<PointCAM*>& control)
{
    int n = control.size();
    int m = n-1;
    QVector<float> d(m);
    for(int i=0;i<n-1;++i){
        d[i]=glm::distance(control[i]->pos(),
                           control[i+1]->pos());
    }
    QVector<glm::vec4> v(2*m);
    v.first() = control.first()->pos();
    for(int i=1;i<m;++i){
        v[2*i-1]=glm::vec4{0,0,0,0};
        v[2*i]=(d[i-1]+d[i])*control[i]->pos();
    }
    v.last() = control.last()->pos();
    //fill band matrix
    constexpr int W=5,B=1;
    BandMatrix<W,B> mat{2*m};
    mat.set(0,0,1.0f);
    for(int i=1;i<m;++i){
        mat.set(2*i-2, 2*i-1,  -(d[i  ]*d[i  ]));
        mat.set(2*i-1, 2*i-1,    d[i  ]*(d[i-1]+d[i]));
        mat.set(2*i  , 2*i-1,   -d[i-1]*(d[i-1]+d[i]));
        mat.set(2*i+1, 2*i-1,   (d[i-1]*d[i-1]));

        mat.set(2*i-1, 2*i,   d[i]);
        mat.set(2*i  , 2*i,   d[i-1]);
    }
    mat.set(2*n-3,2*n-3,1.0f);
    //debug matrix print
    /*qWarning()<<"band matrix";
    for(int i=0;i<2*m;++i){
        int c = i-B;
        qWarning()<<(c<0?QString("* "):(QString("%1 ").arg(mat.at(c,i))))
                <<QString("%1 ").arg(mat.at(c+1,i))
                <<(c+2>=2*m?QString("* "):(QString("%1 ").arg(mat.at(c+2,i))))
                <<(c+3>=2*m?QString("* "):(QString("%1 ").arg(mat.at(c+3,i))))
                <<(c+4>=2*m?QString("* "):(QString("%1 ").arg(mat.at(c+4,i))));
    }*/
    //solve band matrix with v
    //make it triangular
    for(int i=0;i<2*m-1;++i){
        float div = mat.at(i,i);
        //div row to get 1 on main diagonal
        {
            for(int j=0;j<W;++j){
                int c = i+j-B;
                if(c<0||c>=2*m)continue;
                mat.set(c,i,mat.at(c,i)/div);
            }
            v[i]=v[i]/div;
        }
        float under = mat.at(i,i+1);
        //make zero under main diagonal
        {
            for(int j=0;j<W-B;++j){
                int c = i+j;
                if(c<0||c>=2*m)continue;
                mat.set(c,i+1,mat.at(c,i+1)-mat.at(c,i)*under);
            }
            v[i+1]=v[i+1]-v[i]*under;
        }
    }
    //debug matrix print
    /*qWarning()<<"band triangular matrix";
    for(int i=0;i<2*m;++i){
        int c = i-B;
        qWarning()<<(c<0?QString("* "):(QString("%1 ").arg(mat.at(c,i))))
                <<QString("%1 ").arg(mat.at(c+1,i))
                <<(c+2>=2*m?QString("* "):(QString("%1 ").arg(mat.at(c+2,i))))
                <<(c+3>=2*m?QString("* "):(QString("%1 ").arg(mat.at(c+3,i))))
                <<(c+4>=2*m?QString("* "):(QString("%1 ").arg(mat.at(c+4,i))));
    }*/
    //solve triangular
    for(int i=2*m-2;i>=0;--i){
        glm::vec4 sum={0,0,0,0};
        for(int j=1;j<W-B;++j){
            int c = i+j;
            if(c>=2*m) continue;
            sum += mat.at(c,i)*v[c];
        }
        v[i]=(v[i]-sum)/mat.at(i,i);
        //assert(v[i].w >0.99f && v[i].w <1.01f);
    }
    return v;
}
