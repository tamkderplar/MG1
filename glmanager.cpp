#include "glmanager.h"
#include "pointcam.h"


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

    extendBuffer(vBuffer,glmv4size);

    glm::vec4 pos = p->pos();
    vBuffer.write(vBuffer.size()-sizeof(glm::vec4),
                  &pos,sizeof(glm::vec4));


    int bufsize = vBuffer.size();
    v_offsets[p] = bufsize - sizeof(glm::vec4);
    vBuffer.release();
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
