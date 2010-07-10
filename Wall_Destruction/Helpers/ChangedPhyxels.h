#ifndef CHANGED_PHYXELS_H
#define CHANGED_PHYXELS_H

#include <vector>
#include "Structs.h"
#include "CustomEffect.h"
#include "Sphere.h"

class ChangedPhyxels
{
public:
	static void AddPhyxel(ProjectStructs::PHYXEL_NODE* node);
	
	static void Init();
	static void Update(float dt);
	static void Draw();
	static void CleanUp();
	
private:
	static std::vector<ProjectStructs::PHYXEL_NODE*> phyxels;
	static Helpers::CustomEffect phyxelEffect;
	static Sphere phyxelSphere;
	static D3DXMATRIX world;
};

#endif
