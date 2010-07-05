#ifndef PHYXEL_OBJECT_H
#define PHYXEL_OBJECT_H

#include <vector>
#include "Structs.h"
#include "Octree.h"
#include "Sphere.h"
#include "Drawable.h"

class PhyxelObject : Drawable
{
public:
	PhyxelObject(void){
		drawBeenSetup = false;
	};

	PhyxelObject(float density){
		drawBeenSetup = false;
		this->density = density;
	};

	~PhyxelObject(void){};
	void Init(Octree *octree);

	void Draw();
	void Update(float dt);
	void CleanUpDrawables();
	void CleanUp();

private:
	void SetUpDraw();

	bool drawBeenSetup;
	float density;
	std::vector<Structs::PHYXEL_NODE> phyxels;
	Helpers::CustomEffect sphereEffect;
	Sphere phyxelSphere;
	D3DXMATRIX world;

};

#endif

