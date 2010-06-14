/*
	A class that represents a three dimentional array in one dimension

	Array overload implementation taken from http://www.anyexample.com/programming/cplusplus/cplusplus_dynamic_array_template_class.xml

*/

#ifndef THREE_IN_ONE_ARRAY_H
#define THREE_IN_ONE_ARRAY_H

#include <cstdlib>
#include <exception>
 
template<class T>
class ThreeInOneArray
{
public:
	// constructors
    ThreeInOneArray(void);
	ThreeInOneArray(int width, int height, int depth);
    ThreeInOneArray(const ThreeInOneArray &a); 

    ~ThreeInOneArray(); // distructor 
    ThreeInOneArray& operator = (const ThreeInOneArray &a); // assignment operator 

	int* GetIndices(unsigned int index); // one to three
	int GetIndex(unsigned int i, unsigned int j, unsigned int k); // three to one
 
    T& operator [] (unsigned int index); // get array item 
	T& operator () (unsigned int i, unsigned int j, unsigned int k); // get array item 
    
    unsigned int GetSize(); // get size of array (elements)
	unsigned int GetWidth(); // get width of array 
	unsigned int GetDepth(); // get depth of array 
	unsigned int GetHeight(); // get height of array 
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
ThreeInOneArray<T>& ThreeInOneArray<T>::operator = (const ThreeInOneArray &a)
{
    if (this == &a) // in case somebody tries assign array to itself 
		return *this;

	if(oneDimArray != NULL)
		delete [] oneDimArray;

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
T& ThreeInOneArray<T>::operator [] (unsigned int index)
{
    return oneDimArray[index]; // return array element 
}

template <class T>
T& ThreeInOneArray<T>::operator () (unsigned int i, unsigned int j, unsigned int k)
{
	int index = GetIndex(i, j, k);
	if(index == -1)
		throw new std::exception("Invalid index");

	return oneDimArray[index]; // return array element 
}

template <class T>
int* ThreeInOneArray<T>::GetIndices(unsigned int index){
	int *ret = new int[3];

	ret[2] = index % depth;	
	index -= ret[2];
	index /= height;
	ret[1] = index % height;
	index -= ret[1];
	index /= depth;
	ret[0] = index % width;		

	return ret;
}

template <class T>
int ThreeInOneArray<T>::GetIndex(unsigned int i, unsigned int j, unsigned int k){
	if(i < 0 || j < 0 || k < 0 || i > width-1 || j > height-1 || k > depth -1)
		return -1;

	return i * depth * height + j * depth + k;
}

template <class T>
void ThreeInOneArray<T>::Clear(){
	this->oneDimArray = NULL;
}

#endif