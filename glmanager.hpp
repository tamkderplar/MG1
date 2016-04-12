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
    shBoxes[ID]->iBuffer.bind();
    obj_offsets.insert(obj,shBoxes[ID]->iBuffer.size());
    extendBuffer(shBoxes[ID]->iBuffer,GLDrawable::ibufferSize());
    const QList<const PointCAM*> children = obj->children();
    for(int i=0;i<children.size();++i){
        const PointCAM*p = children[i];
        int index = v_offsets[p]/sizeof(glm::vec4);
        shBoxes[ID]->iBuffer.write(shBoxes[ID]->iBuffer.size()
                                        - GLDrawable::ibufferSize()
                                        + i*sizeof(int),
                                   &index,sizeof(int));
    }
    shBoxes[ID]->iBuffer.release();
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
                       box->iBuffer.size()/sizeof(int),
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
