#include "bandmatrix.h"

template<int W,int B>
BandMatrix<W,B>::BandMatrix(int size)
{
    data.resize(W*size);
}

template<int W,int B>
float BandMatrix<W,B>::at(int c, int r) const
{
    if(c<0||c>=N||r<0||r>=N){
        throw std::out_of_range("band matrix indices out of bounds");
    }
    if(c>=r-B && c<r-B+W){
        return data[(c-r+B)+W*(r)];
    }
    if(c<r-B)return 0;
    if(c>=r-B+W)return 0;
    throw std::logic_error("not considered situation, should not happen");
}

template<int W,int B>
void BandMatrix<W,B>::set(int c, int r, float f)
{
    if(c<0||c>=N||r<0||r>=N){
        throw std::out_of_range("band matrix indices out of bounds");
    }
    if(c<r-B || c>=r-B+W){
        throw std::logic_error("matrix access outside of band");
    }
    if(c>=r-B && c<r-B+W){
        data[(c-r+B)+W*(r)] = f;
    }
    throw std::logic_error("not considered situation, should not happen");
}
