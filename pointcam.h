#ifndef POINTCAM_H
#define POINTCAM_H

#include <QColor>
#include <QObject>
#include <QString>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include "glm.h"

class GLManager;

struct SceneData{
    glm::mat4 worldMat;
    glm::mat4 viewMat;
    QColor color;
    float cameraPosZ;
};

typedef int GLDrawableID;

class PointCAM : public QObject
{
    Q_OBJECT
    GLManager* man;
public:
    explicit PointCAM(GLManager *manager=0, QObject *parent=0);
    explicit PointCAM(glm::vec3 p,GLManager *manager=0, QObject *parent=0);

    void setPos(glm::vec3);
    glm::vec4 pos() const;

    QList<const PointCAM *> children() const;
    static constexpr GLDrawableID id(){return 0;}
    static constexpr int ibufferSize(){return sizeof(int);}
    static constexpr GLenum glmode(){return GL_POINTS;}
    //static constexpr GLenum glindexcount(){return 1;}
    static void setUniforms(QOpenGLShaderProgram&shader,SceneData sdata);
    static void setAttributes(QOpenGLShaderProgram&shader);
    static void setTextures(QOpenGLShaderProgram&shader);
    static void preprocessor(QOpenGLBuffer& vBuf,
                             QOpenGLBuffer& iBuf,
                             SceneData sdata);
    static constexpr int instances(){return 1;}


    glm::vec4 position;

    static const QString VSH;
    static const QString GSH;
    static const QString FSH;
signals:

public slots:

private:

};

#endif // POINTCAM_H
