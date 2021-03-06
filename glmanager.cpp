#include "glmanager.h"
#include "pointcam.h"
#include "bufferwrap.h"


void GLManager::extendBuffer(QOpenGLBuffer &buffer, int size)
{
    if(size<0){
        return;
    }
    if(buffer.size()<=0){
        buffer.allocate(size);
        return;
    }
    QByteArray data(buffer.size(),'0');
    buffer.read(0,data.data(),buffer.size());
    buffer.allocate(buffer.size()+size);
    buffer.write(0,data.data(),buffer.size());
}

GLManager::GLManager()
{
}

GLManager::~GLManager()
{
    for(ShaderBox*box:shBoxes){
        delete box;
    }
}

void GLManager::init()
{
    vBuffer = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    vBuffer.create();
    vBuffer.setUsagePattern(QOpenGLBuffer::DynamicDraw);
}

void GLManager::addPoint(PointCAM *p)
{
    vBuffer.bind();
    int oldbuffersize = vBuffer.size();
    int glmv4size = sizeof(glm::vec4);

    //use DELQ if not empty, instead of extending buffer
    extendBuffer(vBuffer,glmv4size);

    glm::vec4 pos = p->pos();
    vBuffer.write(vBuffer.size()-sizeof(glm::vec4),
                  &pos,sizeof(glm::vec4));


    int bufsize = vBuffer.size();
    v_offsets[p] = bufsize - sizeof(glm::vec4);
    vBuffer.release();
    //qWarning()<<"added vertex under"<< (v_offsets[p]/sizeof(glm::vec4));
}

void GLManager::removePoint(PointCAM *p)
{
    if(!v_offsets.contains(p)){
        return;
    }

    v_offsets.remove(p);
    //DO DELQ

}

bool GLManager::updateData(PointCAM *obj, glm::vec4 data, int offset)
{
    /*constexpr int ID = PointCAM::id();
    if(!drawables.contains(ID,obj)){
        return false;
    }*/
    if(!v_offsets.contains(obj)){
        return false;
    }
    vBuffer.bind();
    vBuffer.write(v_offsets[obj]+offset*sizeof(data),
                             &data,sizeof(data));
    vBuffer.release();
    return true;
}

QList<PointCAM *> GLManager::listPoints() const
{
    auto voidlist = drawables.values(PointCAM::id());
    QList<PointCAM*> ret;
    std::transform(voidlist.begin(),voidlist.end(),
                   std::back_inserter(ret),[](void*p){return (PointCAM*)p;});
    assert(ret.size()==voidlist.size());
    return ret;
}
