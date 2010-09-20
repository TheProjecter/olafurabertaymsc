#ifndef IMPACT_LIST_H
#define IMPACT_LIST_H

#include <vector>
#include <algorithm>
#include "Structs.h"
#include "CustomEffect.h"
#include "Sphere.h"
#include "MathHelper.h"

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
// 
// 	static unsigned int GetImpactCount(){return impacts.size();}
// 	static ProjectStructs::IMPACT* GetImpact(int i){
// 		return impacts[i];
// 	}

/*	static std::map<ProjectStructs::PHYXEL_NODE*, std::vector<ProjectStructs::SURFEL*>> GetPhyxelToSurfels(){
		return phyxelToSurfels;
	}

	static std::map<ProjectStructs::SURFEL*, std::vector<ProjectStructs::PHYXEL_NODE*>> GetSurfelsToPhyxels(){
		return surfelsToPhyxel;
	}
*/
	static std::map<ProjectStructs::PHYXEL_NODE*, bool> GetAffectedPhyxels(){
		return affectedPhyxels;
	}

	static std::map<ProjectStructs::SURFEL*, bool> GetAffectedSurfels(){
		return affectedSurfels;
	}

	static std::vector<ProjectStructs::SURFEL*> GetAffectedSurfelVector(){
		if(affectedSurfelsVector.size() == 0){

			for(std::map<ProjectStructs::SURFEL*, bool>::iterator it = affectedSurfels.begin(); it != affectedSurfels.end(); it++){
				affectedSurfelsVector.push_back(it->first);
			}
		}
		return affectedSurfelsVector;
	}

	static std::vector<ProjectStructs::PHYXEL_NODE*> GetAffectedPhyxelVector(){
		if(affectedPhyxelsVector.size() == 0){

			for(std::map<ProjectStructs::PHYXEL_NODE*, bool>::iterator it = affectedPhyxels.begin(); it != affectedPhyxels.end(); it++){
				affectedPhyxelsVector.push_back(it->first);
			}
		}
		return affectedPhyxelsVector;
	}

	static void CalculateImpacts();
	static void AddAffectedPhyxel( ProjectStructs::PHYXEL_NODE * phyxel );
	
	static void AddNewlyAffectedPhyxel( ProjectStructs::PHYXEL_NODE * phyxel ){
		if(phyxel->bodyForce != MathHelper::GetZeroVector()){
			newlyAffectedPhyxels[phyxel] = true;;
		}
	}

	static void InsertNewIntoOld(){
		
		for(std::map<ProjectStructs::PHYXEL_NODE*, bool>::iterator newPhyxels = newlyAffectedPhyxels.begin(); newPhyxels != newlyAffectedPhyxels.end(); newPhyxels++){
			AddAffectedPhyxel(newPhyxels->first);
		}

		newlyAffectedPhyxels.clear();
	}

	static void AddAffectedSurfel( ProjectStructs::SURFEL* surfel );

	static std::map<ProjectStructs::PHYXEL_NODE*, std::vector<ProjectStructs::SURFEL*>> phyxelToSurfels;
	static std::map<ProjectStructs::SURFEL*, std::vector<ProjectStructs::PHYXEL_NODE*>> surfelsToPhyxel;

private:
	static bool SetupImpacts();

	static bool AddForceToPhyxel(D3DXVECTOR3 force, D3DXVECTOR3 pos, D3DXVECTOR3 direction, ProjectStructs::PHYXEL_NODE *phyxel, ProjectStructs::SURFEL* surfel);

	static void AddForceToPhyxels(D3DXVECTOR3 force, D3DXVECTOR3 pos, D3DXVECTOR3 direction, ProjectStructs::PHYXEL_NODE *phyxel, ProjectStructs::SURFEL* surfel);
	static void AddForce(D3DXVECTOR3 force, D3DXVECTOR3 pos, ProjectStructs::SURFEL* surfel);
	

	static std::vector<ProjectStructs::PHYXEL_NODE*> drawablePhyxels;
	static std::map<ProjectStructs::PHYXEL_NODE*, bool> affectedPhyxels, newlyAffectedPhyxels;
	static std::map<ProjectStructs::SURFEL*, bool> affectedSurfels;

	static std::vector<ProjectStructs::PHYXEL_NODE*> affectedPhyxelsVector;
	static std::vector<ProjectStructs::SURFEL*> affectedSurfelsVector;

	static std::map<ProjectStructs::SURFEL*, PreImpactStruct> preImpacts;
	static Helpers::CustomEffect impactEffect;
	
	static ID3D10Buffer *mVB;
	static bool hasBeenSetup, hasBeenInitedOrSomething ;
	static int impactsToDraw;

	static D3DXMATRIX world;
};

#endif
