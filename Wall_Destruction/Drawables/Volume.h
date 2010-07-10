#ifndef VOLUME_H
#define VOLUME_H

#include "Structs.h"
#include "Surface.h"
#include "Octree.h"
#include "PhyxelObject.h"
#include "PhyxelGrid.h"

class Volume
{
public:
	Volume(void);
	~Volume(void){};

	void AddSurface(Surface *surface);

	void SetWorld(D3DXMATRIX world){this->World = world;}

	void And(Volume *volume);
	void Or(Volume *volume);

	void Draw();
	void DrawFirstPass();

	void DrawDepth();
	void DrawAttributes(ID3D10ShaderResourceView* depthTexture);
	void DrawNormalization();

	int GetSurfaceCount(){return (int)surfaces.size();}
	Surface* GetSurface(int i){return surfaces[i];}

	void CleanUp();
	void Update(float dt);
	void Init(bool deformable);

private:
	std::vector<Surface*> surfaces;
	
	D3DXMATRIX World;
	D3DXVECTOR3 pos;

	Octree *octree;
	bool deformable;
	PhyxelObject phyxelObject;

	PhyxelGrid *grid;
};

#endif