#ifndef VOLUME_H
#define VOLUME_H

#include "Structs.h"
//#include "Surface.h"
#include "VertexBufferGrid.h"
#include "PhyxelGrid.h"
#include "ImpactList.h"

class Volume
{
public:
	Volume::Volume(ProjectStructs::MATERIAL_PROPERTIES materialProperties);
	~Volume(void){};

	//void AddSurface(Surface *surface);
	void AddSurfel(ProjectStructs::SURFEL* surfel);
	void AddCrack(ProjectStructs::CRACK_NODE* crack);

	void SetWorld(D3DXMATRIX world){this->World = world;}
	D3DXMATRIX GetWorld(){return this->World;}

	void And(Volume *volume);
	void Or(Volume *volume);

	void ResetSurfaces();
	void ClearImpacts();
	void Draw();
	void DrawFirstPass();

	void DrawDepth();
	void DrawAttributes(ID3D10ShaderResourceView* depthTexture);
	void DrawNormalization();

/*	int GetSurfaceCount(){return (int)surfaces.size();}
	Surface* GetSurface(int i){return surfaces[i];}
*/
/*	unsigned int GetImpactSize(){return impactList->GetImpactCount();}
	ProjectStructs::IMPACT* GetImpact(int i){return impactList->GetImpact(i);}
	ImpactList* GetImpactList(){return impactList;}
*/

	void CleanUp();
	void Update(float dt);
	void Init();
	D3DXVECTOR3 GetPosition(){return pos;}
	PhyxelGrid* GetPhyxelGrid(){return phyxelGrid;};
	
/*	int GetCrackedPhyxelSize(){return (int)crackedPhyxels.size();}
	ProjectStructs::PHYXEL_NODE* GetCrackedPhyxel(int i){ return crackedPhyxels[i]; }
*/
	ProjectStructs::MATERIAL_PROPERTIES GetMaterialProperties(){return materialProperties;}
	void StepResampleAlgorithm();
	static std::map<ProjectStructs::PHYXEL_NODE*, float> weights;

private:
	//std::vector<Surface*> surfaces;
	VertexBufferGrid* vertexBufferGrid;
	std::vector<ProjectStructs::SURFEL*> newSurfels;
	
	/*std::vector<ProjectStructs::PHYXEL_NODE*> crackedPhyxels;
	ImpactList* impactList;
	
	bool cracked;
	*/
	D3DXMATRIX World;
	D3DXVECTOR3 pos;

	PhyxelGrid* phyxelGrid;
	//VertexBufferGrid* vertexBufferGrid;
	ProjectStructs::MATERIAL_PROPERTIES materialProperties;
};

#endif