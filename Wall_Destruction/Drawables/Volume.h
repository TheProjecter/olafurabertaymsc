#ifndef VOLUME_H
#define VOLUME_H

#include "Structs.h"
#include "Surface.h"
#include "PhyxelGrid.h"
#include "ChangedPhyxels.h"

class Volume
{
public:
	Volume(void);
	~Volume(void){};

	void AddSurface(Surface *surface);

	void SetWorld(D3DXMATRIX world){this->World = world;}

	void And(Volume *volume);
	void Or(Volume *volume);

	void CleanChangedPhyxels();
	void Draw();
	void DrawFirstPass();

	void DrawDepth();
	void DrawAttributes(ID3D10ShaderResourceView* depthTexture);
	void DrawNormalization();

	int GetSurfaceCount(){return (int)surfaces.size();}
	Surface* GetSurface(int i){return surfaces[i];}

	unsigned int GetChangedPhyxelsSize(){return changedPhyxels->GetPhyxelCount();}
	ProjectStructs::PHYXEL_NODE* GetChangedPhyxel(int i){return changedPhyxels->GetPhyxel(i);}

	void CleanUp();
	void Update(float dt);
	void Init(ProjectStructs::MATERIAL_PROPERTIES materialProperties);
	ProjectStructs::MATERIAL_PROPERTIES GetMaterialProperties(){return materialProperties;}

private:
	std::vector<Surface*> surfaces;
	ChangedPhyxels *changedPhyxels;
	
	D3DXMATRIX World;
	D3DXVECTOR3 pos;

	D3DXMATRIX C;

	PhyxelGrid* phyxelGrid;
	//VertexBufferGrid* vertexBufferGrid;
	ProjectStructs::MATERIAL_PROPERTIES materialProperties;
};

#endif