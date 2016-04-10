#ifndef BUFFERWRAP_H
#define BUFFERWRAP_H

#include <QOpenGLBuffer>
#include <QQueue>
#include <QVector>

template<class T>
class BufferWrap
{
    QOpenGLBuffer buffer;
    //elements stored (including deleted)
    int stored;
    //offset of temporary values
    //(or -1 if no temp were allocated or they were dropped)
    int tempOff;
    //indices of deleted elements
    QQueue<int> delQ;
public:
    BufferWrap();
    bool init(QOpenGLBuffer::Type,QOpenGLBuffer::UsagePattern);
    bool bind();
    void release();
    //count of elements stored (including deleted)
    int count() const;
    //count of allocated elements total
    int capacity() const;
    //returns index of added element
    int add(const T&t);

    //allocates and writes to temporary memory after stored elements
    //regular add will overwrite temporaries
    void allocateTemporary(const QVector<T>&tmp);
    int tempOffest() const;
    void dropTemporary();

    //adds capacity for count elements
    void extend(int count);
private:
    //reallocates buffer to store count elements
    void reloc(int count);
    //place element t under index i, used for safety
    void place(int i,const T&&t);
};

#include "bufferwrap.cpp"

#endif // BUFFERWRAP_H
