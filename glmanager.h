#ifndef GLMANAGER_H
#define GLMANAGER_H

#include <QMap>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions_3_3_Core>
#include "glm.h"
#include "pointcam.h"

class PointCAM;

class GLManager
{
    //set of drawable objects for each ID
    QMultiMap<GLDrawableID,void*> drawables;
    //vertex buffer to store all PointCAM vertices
    //?draw PointCAM directly from these or as drawables?
    QOpenGLBuffer vBuffer;
    //QQueue<int> vBufDelQ;//queue to store free(removed) vbuffer indices
    //map of vertex offsets (in bytes) for each PointCAM
    QMap<const PointCAM*,int> v_offsets;
    //QMap<int,QVector<void*>>

    //struct for storing index buffer, usable count and shader program
    //  for each drawable type
    //all drawable objects of same type share index buffer
    //  ,because they're all drawn by the same shader program
    struct ShaderBox{
        QOpenGLBuffer iBuffer;
        int iBufCount;
        QOpenGLShaderProgram shader;
    };
    QMap<GLDrawableID,ShaderBox*> shBoxes;
    //map of index offsets for each drawable object
    QMap<void*,int> obj_offsets;

    template<class GLDrawable>
    void addType();
    void extendBuffer(QOpenGLBuffer&buffer,int size);
public:
    GLManager();
    ~GLManager();
    void init();
    template<class GLDrawable>
    void addDrawable(GLDrawable*);
    template<class GLDrawable>
    void removeDrawable(GLDrawable*);
    //template<class GLDrawable>
    //void updateDrawable(GLDrawable*);
    void addPoint(PointCAM*);
    void removePoint(PointCAM*);
    bool updateData(PointCAM*obj,glm::vec4 data,int offset);

    //?extend to template listDrawables<GLDrawable>?
    QList<PointCAM *> listPoints() const;

    template<class GLDrawable>
    void drawAll(SceneData sdata,QOpenGLFunctions_3_3_Core*);
};

#include "glmanager.hpp"

#endif // GLMANAGER_H
