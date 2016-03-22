#ifndef POINTCAM_H
#define POINTCAM_H

#include <QObject>
#include <QString>
#include "glm.h"

class PointCAM : public QObject
{
    Q_OBJECT
public:
    explicit PointCAM(QObject *parent = 0);
    explicit PointCAM(glm::vec3 p, QObject *parent = 0);
    explicit PointCAM(const PointCAM& other);
    PointCAM &operator=(const PointCAM& other);

    void setPos(glm::vec3);
    glm::vec4 pos() const;


    glm::vec4 position;
    QString name;
signals:

public slots:

private:

};

#endif // POINTCAM_H
