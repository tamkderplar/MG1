#ifndef GLMANAGER_HPP
#define GLMANAGER_HPP

#include <QOpenGLShaderProgram>

#ifndef GLMANAGER_H
//these are for highlight purposes
#include "glmanager.h"
//highlight purposes end here
#endif

template<class GLDrawable>
void GLManager::addType()
{
    ShaderBox* box = *(shBoxes.insert(GLDrawable::id(),new ShaderBox()));

    box->iBuffer = QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
    box->iBuffer.create();
    box->iBuffer.setUsagePattern(QOpenGLBuffer::DynamicDraw);
    box->iBufCount = 0;

    bool result = false;
    result = box->shader.addShaderFromSourceCode(QOpenGLShader::Vertex,
                                                    GLDrawable::VSH);
    if(!result) qWarning() << box->shader.log();
    result = box->shader.addShaderFromSourceCode(QOpenGLShader::Geometry,
                                                    GLDrawable::GSH);
    if(!result) qWarning() << box->shader.log();
    result = box->shader.addShaderFromSourceCode(QOpenGLShader::Fragment,
                                                    GLDrawable::FSH);
    if(!result) qWarning() << box->shader.log();
    result =
    box->shader.link();
    if(!result) qWarning() << box->shader.log();
}

template<class GLDrawable>
void GLManager::addDrawable(GLDrawable *obj)
{
    constexpr int ID = GLDrawable::id();
    if(drawables.contains(ID,obj)){
        return;
    }
    drawables.insert(ID,obj);

    if(!shBoxes.contains(ID)){
        addType<GLDrawable>();
    }
    QOpenGLBuffer&iBuf = shBoxes[ID]->iBuffer;
    iBuf.bind();

    int writeOffset = shBoxes[ID]->iBufCount*GLDrawable::ibufferSize();
    if(writeOffset==iBuf.size()){
        extendBuffer(iBuf,GLDrawable::ibufferSize());
    }
    obj_offsets.insert(obj,writeOffset);

    const QList<PointCAM*> children = obj->children();
    for(int i=0;i<children.size();++i){
        const PointCAM*p = children[i];
        int index = v_offsets[p]/sizeof(glm::vec4);
        iBuf.write(writeOffset + i*sizeof(int), &index, sizeof(int));
    }
    iBuf.release();
    shBoxes[ID]->iBufCount++;
}

template<class GLDrawable>
void GLManager::removeDrawable(GLDrawable *obj)
{
    constexpr int ID = GLDrawable::id();
    if(!drawables.contains(ID,obj)){
        throw std::logic_error("removing unexisting drawable");
        //?return;
    }

    int removed_offset = obj_offsets[obj];
    int removedID = removed_offset/GLDrawable::ibufferSize();

    for(void*object:drawables.values(GLDrawable::id())){
        auto pair= obj_offsets.find(object);
        if(pair.value()>removed_offset){
            pair.value()-=sizeof(int);
        }
    }
    obj_offsets.remove(obj);
    int idxCount = GLDrawable::ibufferSize()/sizeof(int);

    QOpenGLBuffer&buf = shBoxes[GLDrawable::id()]->iBuffer;
    buf.bind();
    int*indices = (int*)buf.map(QOpenGLBuffer::ReadWrite);
    int bufSize = buf.size()/sizeof(int);
    //?;i<iBufCount-idxCount;?
    for(int i=removedID;i<bufSize-idxCount;++i){
        indices[i] = indices[i+idxCount];
    }
    //indices[bufSize-1]=indices[0];
    buf.unmap();
    buf.release();
    shBoxes[GLDrawable::id()]->iBufCount--;

    drawables.remove(GLDrawable::id(),obj);
}

template<class GLDrawable>
void GLManager::drawAll(SceneData sdata,QOpenGLFunctions_3_3_Core*glfunc)
{
    ShaderBox*box = shBoxes[GLDrawable::id()];
    {
        box->shader.bind();
        vBuffer.bind();
        box->iBuffer.bind();

        GLDrawable::preprocessor(vBuffer,box->iBuffer,sdata);
        GLDrawable::setUniforms(box->shader,sdata);
        GLDrawable::setAttributes(box->shader);
        glfunc->glDrawElementsInstanced(GLDrawable::glmode(),
                       box->iBufCount*GLDrawable::ibufferSize()/sizeof(int),
                       GL_UNSIGNED_INT,0,GLDrawable::instances());

        //box->shader.disableAttributeArray("vertex");
        //vBuffer.release();
        //box->iBuffer.release();
        //QOpenGLBuffer::release(QOpenGLBuffer::VertexBuffer);
        //QOpenGLBuffer::release(QOpenGLBuffer::IndexBuffer);
        box->shader.release();
    }
}

#endif // GLMANAGER_HPP
