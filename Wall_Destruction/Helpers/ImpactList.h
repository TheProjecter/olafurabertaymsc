#ifndef IMPACT_LIST_H
#define IMPACT_LIST_H

#include <vector>
#include "Structs.h"
#include "CustomEffect.h"
#include "Sphere.h"

struct PreImpactStruct{
	D3DXVECTOR3 pos;
	D3DXVECTOR3 force;
	int contactPoints;
};

class ImpactList
{
public:
	static void AddPreImpact(ProjectStructs::SURFEL* surfel, D3DXVECTOR3 force, D3DXVECTOR3 pos);

	static void AddImpact(ProjectStructs::IMPACT* impact);
	static void AddImpactForNextIteration(ProjectStructs::IMPACT* impact);
	
	static void Init();
	static void Update(float dt);
	static void Emptylist();
	static void Draw();
	static void CleanUp();

	static unsigned int GetImpactCount(){return impacts.size();}
	static ProjectStructs::IMPACT* GetImpact(int i){
		return impacts[i];
	}
	static void CalculateImpacts();

private:
	static bool SetupImpacts();

	static bool AddForceToPhyxel(D3DXVECTOR3 force, D3DXVECTOR3 pos, D3DXVECTOR3 direction, ProjectStructs::PHYXEL_NODE *phyxel, ProjectStructs::SURFEL* surfel);
	static void AddForceToPhyxels(D3DXVECTOR3 force, D3DXVECTOR3 pos, D3DXVECTOR3 direction, ProjectStructs::PHYXEL_NODE *phyxel, ProjectStructs::SURFEL* surfel);
	static void AddForce(D3DXVECTOR3 force, D3DXVECTOR3 pos, ProjectStructs::SURFEL* surfel);
	
	static std::vector<ProjectStructs::IMPACT*> impacts, drawableImpacts;
	static std::map<ProjectStructs::SURFEL*, PreImpactStruct> preImpacts;
	static Helpers::CustomEffect impactEffect;
	
	static ID3D10Buffer *mVB;
	static bool hasBeenSetup, hasBeenInitedOrSomething ;
	static int impactsToDraw;

	static D3DXMATRIX world;
};

#endif
