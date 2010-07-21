#ifndef CHANGED_PHYXELS_H
#define CHANGED_PHYXELS_H

#include <vector>
#include "Structs.h"
#include "CustomEffect.h"
#include "Sphere.h"

class ChangedPhyxels
{
public:
	void AddPhyxel(ProjectStructs::PHYXEL_NODE* node);
	void AddDrawablePhyxel(ProjectStructs::PHYXEL_NODE* node);
	
	void Init();
	void Update(float dt);
	void Emptylist();
	void Draw();
	void CleanUp();

	unsigned int GetPhyxelCount(){return phyxels.size();}
	ProjectStructs::PHYXEL_NODE* GetPhyxel(int i){return phyxels[i];}

private:
	bool SetupPhyxels();

	std::vector<ProjectStructs::PHYXEL_NODE*> phyxels, drawablePhyxels;
	Helpers::CustomEffect phyxelEffect;
	
	ID3D10Buffer *mVB;
	bool hasBeenSetup;
	int phyxelsToDraw;

	D3DXMATRIX world;
};

#endif
