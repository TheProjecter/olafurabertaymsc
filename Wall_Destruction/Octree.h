#ifndef OCTREE_H
#define OCTREE_H

#include <D3DX10.h>
#include <D3D10.h>
#include "Structs.h"
#include "CustomEffect.h"
#include "Sphere.h"
#include "Surface.h"
#include "Drawable.h"
#include <vector>
/*
	Implementation based the octree implementation from the Real Time Collision Detection book by Crister Ericson
*/

class Octree : public Drawable
{
public:
	Octree(void){};
	Octree(D3DXVECTOR3 position, float halfWidth, int depthLimit);
	~Octree(void){};
	void InsertObject(Structs::SurfelNode* surfelNode);
	void InsertSurface(Drawables::Surface *surface);
	Structs::OctreeNode* GetOctreeNode(){return ParentNode;}

	void Update(float dt);
	void Draw();
	void CleanUp();
	void CleanUpDrawables();

private:
	void CleanUpNode(Structs::OctreeNode *node);
	void SetUpDrawables(Structs::OctreeNode *node);
	Structs::OctreeNode* BuildOctree(D3DXVECTOR3 position, float halfWidth, int depthLimit, bool isParent);
	bool InsertObject(Structs::OctreeNode *pTree, Structs::SurfelNode* surfelNode, Structs::OctreeNode *parent, int depth);
	void SetUpDraw();

	Structs::OctreeNode *ParentNode;

	int DepthLimit;
	D3DXVECTOR3 Position;
	float HalfWidth;
	ID3D10Buffer *vertexBuffer, *indexBuffer;
	Helpers::CustomEffect effect, sphereEffect;
	bool DrawSetUp;

	D3DXMATRIX tmpWorld, tmpScale;
	Sphere sphere;
	std::vector<D3DXMATRIX> octreeCubes;
	//std::vector<std::vector<Structs::SurfelNode*>> surfels;

};

#endif