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

    extendBuffer(vBuffer,glmv4size);

    glm::vec4 pos = p->pos();
    vBuffer.write(vBuffer.size()-sizeof(glm::vec4),
                  &pos,sizeof(glm::vec4));


    int bufsize = vBuffer.size();
    v_offsets[p] = bufsize - sizeof(glm::vec4);
    vBuffer.release();
}

void GLManager::removePoint(PointCAM *p)
{
    if(!v_offsets.contains(p)){
        return;
    }

    //int removed_offset = v_offsets[p];
    int removedID = v_offsets[p]/sizeof(glm::vec4);
    int pOffset = obj_offsets[p];
    qWarning()<<"remID:"<<removedID<<"  pOff:"<<pOffset;
    assert(removedID*4 == pOffset);
    v_offsets.remove(p);

    /*for(QMap<const PointCAM*,int>::iterator&pair:v_offsets){
        if(pair.value()>removed_offset){
            pair.value()-=sizeof(glm::vec4);
        }
    }*/
    QOpenGLBuffer&buf = shBoxes[PointCAM::id()]->iBuffer;
    buf.bind();
    int*indices = (int*)buf.map(QOpenGLBuffer::ReadWrite);
    int bufSize = buf.size()/sizeof(int);
    for(int i=removedID;i<bufSize-1;++i){
        indices[i] = indices[i+1];
    }
    indices[bufSize-1]=indices[0];
    buf.unmap();
    buf.release();

    drawables.remove(PointCAM::id(),p);
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
