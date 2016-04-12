#ifndef BUFFERWRAP_H
#include "bufferwrap.h"
#endif

template<class T>
BufferWrap<T>::BufferWrap():stored(0),tempOff(-1)
{
}

template<class T>
bool BufferWrap<T>::init(QOpenGLBuffer::Type type,
                      QOpenGLBuffer::UsagePattern use)
{
    buffer = QOpenGLBuffer(type);
    buffer.setUsagePattern(use);
    return buffer.create();
}

template<class T>
bool BufferWrap<T>::bind()
{
    return buffer.bind();
}

template<class T>
void BufferWrap<T>::release()
{
    buffer.release();
}

template<class T>
int BufferWrap<T>::count() const
{
    return stored;
}

template<class T>
int BufferWrap<T>::capacity() const
{
    return buffer.size()/sizeof(T);
}

template<class T>
int BufferWrap<T>::add(const T &t)
{
    if(!delQ.empty()){
        int i = delQ.dequeue();
        place(i,t);
        return i;
    }
    if(capacity()==count()){
        int i = capacity();
        extend(1);
        place(i,t);
        stored++;
        return i;
    }
    int i = count();
    place(i,t);
    stored++;
    return i;
}

template<class T>
void BufferWrap<T>::allocateTemporary(const QVector<T> &tmp)
{
    reloc(count()+tmp.size());
    buffer.write(count()*sizeof(T),tmp.data(),tmp.size()*sizeof(T));
    tempOff = count()*sizeof(T);
}

template<class T>
int BufferWrap<T>::tempOffest() const
{
    return tempOff;
}

template<class T>
void BufferWrap<T>::dropTemporary()
{
    tempOff = -1;
}

template<class T>
void BufferWrap<T>::extend(int count)
{
    if(count<0) return;
    if(buffer.size()<=0){
        buffer.allocate(count);
        return;
    }
    QByteArray data(buffer.size(),'0');
    buffer.read(0,data.data(),buffer.size());
    buffer.allocate(buffer.size()+count*sizeof(T));
    buffer.write(0,data.data(),data.size());
}

template<class T>
void BufferWrap<T>::reloc(int count)
{
    if(count<0) return;
    if(buffer.size()<=0){
        buffer.allocate(count*sizeof(T));
        return;
    }
    QByteArray data(buffer.size(),'0');
    buffer.read(0,data.data(),buffer.size());
    buffer.allocate(count*sizeof(T));
    buffer.write(0,data.data(),qMin(buffer.size(),data.size()));
}

template<class T>
void BufferWrap<T>::place(int i, const T&&t)
{
    buffer.write(i*sizeof(T),&t,sizeof(T));
}
