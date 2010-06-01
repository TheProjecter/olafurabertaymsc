#ifndef SURFEL_OBJECT_H
#define SURFEL_OBJECT_H

#include <D3DX10.h>

struct Surfel{
	// position
	D3DXVECTOR3 pos;
	// normal
	D3DXVECTOR3 normal;
	// dimensions.x : width
	// dimensions.y : height
	D3DXVECTOR2 dimensions;
};

class SurfelObject
{
public:
	SurfelObject(void);

	~SurfelObject(void);
private:

};

#endif