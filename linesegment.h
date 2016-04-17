#ifndef LINESEGMENT_H
#define LINESEGMENT_H

#include "pointcam.h"

class LineSegment
{
    QList<PointCAM*> control;
public:
    LineSegment(QVector<PointCAM*> controlPoints);

    const QList<PointCAM *> children() const;
    static constexpr GLDrawableID id(){return 2;}
    static constexpr int ibufferSize(){return 2*sizeof(int);}
    static constexpr GLenum glmode(){return GL_LINES;}
    //static constexpr GLenum glindexcount(){return 1;}
    static void setUniforms(QOpenGLShaderProgram&shader,SceneData sdata);
    static void setAttributes(QOpenGLShaderProgram&shader);
    static void setTextures(QOpenGLShaderProgram&shader);
    static void preprocessor(QOpenGLBuffer& vBuf,
                             QOpenGLBuffer& iBuf,
                             SceneData sdata);
    static int instances(){return 1;}


    static const QString VSH;
    static const QString GSH;
    static const QString FSH;
};

#endif // LINESEGMENT_H
