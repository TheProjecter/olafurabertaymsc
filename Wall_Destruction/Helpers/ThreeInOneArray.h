/*
	A class that represents a three dimentional array in one dimension

	Array overload implementation taken from http://www.anyexample.com/programming/cplusplus/cplusplus_dynamic_array_template_class.xml

*/

#ifndef THREE_IN_ONE_ARRAY_H
#define THREE_IN_ONE_ARRAY_H

#include <cstdlib>
#include <exception>
#include <D3DX10.h>
 
struct INDEX{
	int x;
	int y;
	int z;
};

template<class T>
class ThreeInOneArray
{
public:
	// constructors
    ThreeInOneArray(void);
	ThreeInOneArray(int width, int height, int depth);
    ThreeInOneArray(const ThreeInOneArray &a); 

    ~ThreeInOneArray(); // destructor 
    ThreeInOneArray& operator = (const ThreeInOneArray &a); // assignment operator 

	INDEX GetIndices(int index); // one to three
	void GetIndices(int index, int* &indices ); // one to three
	int GetIndex(int i, int j, int k); // three to one
 
    T& operator [] (int index); // get array item 
	T& operator () (int i, int j, int k); // get array item 
	T& operator () (D3DXVECTOR3 index); // get array item 
    
    unsigned int GetSize(); // get size of array (elements)
	unsigned int GetWidth(); // get width of array 
	unsigned int GetDepth(); // get depth of array 
	unsigned int GetHeight(); // get height of array 
	bool ValidIndex(int i, int j, int k);
	bool ValidIndex(D3DXVECTOR3 index);
    void Clear(); // clear array
    
private:
    T *oneDimArray; // pointer for array's memory 
    unsigned int size, width, height, depth; 
};

// here comes the implementation

template <class T>
ThreeInOneArray<T>::ThreeInOneArray()
{
	// do almost nothing...
	this->oneDimArray = NULL;
}
 
template <class T>
ThreeInOneArray<T>::ThreeInOneArray(int width, int height, int depth)
{
	this->size = width * height * depth;
	this->width = width;
	this->height = height;
	this->depth = depth;

	this->oneDimArray = NULL;
	this->oneDimArray = new T[size];
}
 
template <class T>
ThreeInOneArray<T>::~ThreeInOneArray()
{
    if (oneDimArray)
    {
    	free(oneDimArray); // Freeing memory 
    	oneDimArray = NULL;
    }
} 
 
template <class T>
ThreeInOneArray<T>::ThreeInOneArray(const ThreeInOneArray &a)
{
	this->size = a.size;
	this->width = a.width;
	this->depth = a.depth;
	this->height = a.height;

	this->oneDimArray = new T[size];
	for(unsigned int i = 0; i<size; i++){
		this->oneDimArray[i] = a.oneDimArray[i];
	}
}

template <class T>
bool ThreeInOneArray<T>::ValidIndex(int i, int j, int k){
	return i >= 0 && j >= 0 && k >= 0 && (unsigned int)i <width && (unsigned int)j < height && (unsigned int)k < depth;
}

template <class T>
bool ThreeInOneArray<T>::ValidIndex(D3DXVECTOR3 index){

	// don't know why the indices are positive when they are passed over to ValidIndex(i, j, k)
	return (int)index.x >= 0 && (int)index.y >= 0 && (int)index.z >= 0 && ValidIndex((int)index.x, (int)index.y, (int)index.z);
}
 
template <class T>
ThreeInOneArray<T>& ThreeInOneArray<T>::operator = (const ThreeInOneArray &a)
{
    if (this == &a) // in case somebody tries assign array to itself 
		return *this;

	if(oneDimArray != NULL){
		delete [] oneDimArray;
	}

	this->size = a.size;
	this->width = a.width;
	this->depth = a.depth;
	this->height = a.height;

	oneDimArray = new T[size];

    return *this;
}
 
template <class T>
unsigned int ThreeInOneArray<T>::GetSize()
{
    return size; // simply return size
}

template <class T>
unsigned int ThreeInOneArray<T>::GetDepth()
{
    return depth; 
}

template <class T>
unsigned int ThreeInOneArray<T>::GetHeight()
{
    return height;
}
 
template <class T>
unsigned int ThreeInOneArray<T>::GetWidth()
{
    return width; // simply return size
}
template <class T>
T& ThreeInOneArray<T>::operator [] (int index)
{
    return oneDimArray[index]; // return array element 
}

template <class T>
T& ThreeInOneArray<T>::operator () (int i, int j, int k)
{
	int index = GetIndex(i, j, k);
	if(index == -1)
		throw new std::exception("Invalid index");

	return oneDimArray[index]; // return array element 
}

template <class T>
T& ThreeInOneArray<T>::operator () (D3DXVECTOR3 index)
{
	int indx = GetIndex((int)index.x, (int)index.y, (int)index.z);
	if(indx == -1)
		throw new std::exception("Invalid index");

	return oneDimArray[indx]; // return array element 
}

template <class T>
INDEX ThreeInOneArray<T>::GetIndices(int index){
	INDEX ret;

	ret.z = index % depth;	
	index -= ret.z;
	index /= height;
	ret.y = index % height;
	index -= ret.y;
	index /= depth;
	ret.x = index % width;		

	return ret;
}

template <class T>
int ThreeInOneArray<T>::GetIndex(int i, int j, int k){
	if(i < 0 || j < 0 || k < 0 || (unsigned int)i > width-1 || (unsigned int)j > height-1 || (unsigned int)k > depth -1)
		return -1;

	return i * depth * height + j * depth + k;
}

template <class T>
void ThreeInOneArray<T>::Clear(){
	delete [] oneDimArray;
	this->oneDimArray = NULL;
}

#endif