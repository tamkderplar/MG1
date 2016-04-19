#ifndef BANDMATRIX_H
#define BANDMATRIX_H

#include <QVector>

//band square matrix of band width W and bias B
//(bias being number of non-zero diagonals under main diagonal)
template<int W,int B>
class BandMatrix
{
    QVector<float> data;
    int N;
public:
    BandMatrix(int size=0);
    float at(int c, int r) const;
    void set(int c, int r, float f);
};

#include "bandmatrix.cpp"

#endif // BANDMATRIX_H
