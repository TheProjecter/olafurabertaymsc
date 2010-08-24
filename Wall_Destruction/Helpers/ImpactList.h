#ifndef IMPACT_LIST_H
#define IMPACT_LIST_H

#include <vector>
#include "Structs.h"
#include "CustomEffect.h"
#include "Sphere.h"

class ImpactList
{
public:
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

private:
	static bool SetupImpacts();

	static std::vector<ProjectStructs::IMPACT*> impacts, drawableImpacts;
	static Helpers::CustomEffect impactEffect;
	
	static ID3D10Buffer *mVB;
	static bool hasBeenSetup, hasBeenInitedOrSomething ;
	static int impactsToDraw;

	static D3DXMATRIX world;
};

#endif
