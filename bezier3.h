#ifndef BEZIER3_H
#define BEZIER3_H

#include "pointcam.h"

class Bezier3
{
    QList<PointCAM*> control;
public:
    Bezier3(QVector<PointCAM*> controlPoints);

    const QList<PointCAM *> children() const;
    static constexpr GLDrawableID id(){return 1;}
    static constexpr int ibufferSize(){return 4*sizeof(int);}
    static constexpr GLenum glmode(){return GL_LINES_ADJACENCY;}
    //static constexpr GLenum glindexcount(){return 1;}
    static void setUniforms(QOpenGLShaderProgram&shader,SceneData sdata);
    static void setAttributes(QOpenGLShaderProgram&shader);
    static void setTextures(QOpenGLShaderProgram&shader);
    static void preprocessor(QOpenGLBuffer& vBuf,
                             QOpenGLBuffer& iBuf,
                             SceneData sdata);
    static int instances(){return instanceSplit;}

    static int origSize;
    static int instanceSplit;

    static const QString VSH;
    static const QString GSH;
    static const QString FSH;
};

#endif // BEZIER3_H
