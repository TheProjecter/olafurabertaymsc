#include "Octree.h"
#include "MathHelper.h"
#include "Structs.h"


Octree::Octree(D3DXVECTOR3 position, float halfWidth, int depthLimit)
{
	Position = position;
	HalfWidth = halfWidth;
	DepthLimit = depthLimit;
/*
	surfels = std::vector<std::vector<Structs::SurfelNode*>>(depthLimit);

	for (int i = 0; i < depthLimit ; i++)
	{
		surfels.push_back(std::vector<Structs::SurfelNode*>());
	}
*/	
	ParentNode = BuildOctree(position, halfWidth, depthLimit, true);

	DrawSetUp = false;

	this->sphere = Sphere();
	this->sphere.init(0.1f, 3, 3);
}

Structs::OctreeNode* Octree::BuildOctree(D3DXVECTOR3 position, float halfWidth, int depthLimit, bool isParent){
	if (depthLimit < 0) 
		return NULL;
	
	// Construct and fill in root of this subtree
	Structs::OctreeNode *pNode = new Structs::OctreeNode;

	if(isParent){
		pNode->parent = NULL;
	}

	pNode->center = position;
	pNode->halfWidth = halfWidth;
	pNode->pObjList = NULL;
	pNode->checked = false;
	pNode->childrenContainObjects = false;
	
	// Recursively construct the eight children of the subtree
	D3DXVECTOR3 offset;
	float step = halfWidth * 0.5f;
	for (int i = 0; i < 8; i++) {
		offset.x = ((i & 1) ? step : -step);
		offset.y = ((i & 2) ? step : -step);
		offset.z = ((i & 4) ? step : -step);
		pNode->pChild[i] = BuildOctree(position + offset, step, depthLimit - 1, false);
		if(pNode->pChild[i])
			pNode->pChild[i]->parent = pNode;
	}

	return pNode;
}

void Octree::InsertSurface(Drawables::Surface *surface){

	for (int i = 0; i < surface->GetSurfaceSurfelCount() ; i++)
	{
		Structs::SurfelNode* sn = new Structs::SurfelNode;
		sn->surfel = surface->GetSurfaceSurfel(i);
		sn->surfel.pos += Position;
		InsertObject(sn);
	}

	/*for (int i = 0; i < surface->GetEdgeSurfelCount() ; i++)
	{
		Structs::SurfelNode* sn = new Structs::SurfelNode;
		sn->surfel = surface->GetEdgeSurfel(i);
		sn->surfel.pos += Position;
		InsertObject(sn);
	}*/
}

void Octree::InsertObject(Structs::SurfelNode* surfelNode){
	int index = 0, straddle = 0;

	for (int i = 0; i < 3; i++) {
		float delta = surfelNode->surfel.pos[i] - ParentNode->center[i];

		// this method was changed from the original
		if (abs(delta) >= ParentNode->halfWidth) {
			straddle = 1;
			break;
		}

		if (delta > 0.0f) index |= (1 << i); // ZYX
	}
	if (!straddle && ParentNode->pChild[index]) {
		// Fully contained in existing child node; insert in that subtree
		ParentNode->childrenContainObjects = InsertObject(ParentNode->pChild[index], surfelNode, ParentNode, 0);
	} else {
		// Straddling, or no child node to descend into, so
		// link object into linked list at this node
		
		//surfels[0].[0].push_back(surfelNode);
		surfelNode->nextSurfel = ParentNode->pObjList;
		ParentNode->pObjList = surfelNode;
	}
}

bool Octree::InsertObject(Structs::OctreeNode *pTree, Structs::SurfelNode* surfelNode, Structs::OctreeNode* parent, int depth)
{
	int index = 0, straddle = 0;

	for (int i = 0; i < 3; i++) {
		float delta = surfelNode->surfel.pos[i] - pTree->center[i];

		if (abs(delta) >= ParentNode->halfWidth) {
			straddle = 1;
			break;
		}

		if (delta > 0.0f) index |= (1 << i); // ZYX
	}

	if (!straddle && pTree->pChild[index]) {
		// Fully contained in existing child node; insert in that subtree
		pTree->childrenContainObjects = InsertObject(pTree->pChild[index], surfelNode, pTree, ++depth);
	} else {
		// Straddling, or no child node to descend into, so
		// link object into linked list at this node
		//surfels[depth].push_back(surfelNode);
		surfelNode->nextSurfel = parent->pObjList;
		parent->pObjList = surfelNode;
		return true;
	}
	return pTree->childrenContainObjects;
}

void Octree::CleanUpDrawables(){
	if(DrawSetUp){
		effect.CleanUp();
		vertexBuffer->Release();
		vertexBuffer = NULL;

		octreeCubes.clear();

		DrawSetUp = false;	
	}
}

void Octree::CleanUp(){
	CleanUpNode(ParentNode);
	CleanUpDrawables();
}

void Octree::CleanUpNode(Structs::OctreeNode* node){

	if(!node)
		return;

	for(int i = 0; i<8; i++){
		CleanUpNode(node->pChild[i]);
	}

	if(node->pObjList){
		Structs::SurfelNode *sn = node->pObjList;
		while(node->pObjList != NULL){
			sn = node->pObjList->nextSurfel;
			
			delete node->pObjList;
			node->pObjList = NULL;

			node->pObjList = sn;
		}
	}
	
	delete node;
}

void Octree::Draw(){
	if(!DrawSetUp){
		SetUpDraw();
	}

	effect.SetMatrix("View", Helpers::Globals::AppCamera.View());
	effect.SetMatrix("Projection", Helpers::Globals::AppCamera.Projection());

	effect.PreDraw();

	UINT stride = sizeof( Structs::SIMPLE_VERTEX);
	UINT offset = 0;

	Helpers::Globals::Device->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_LINELIST);
	Helpers::Globals::Device->IASetVertexBuffers( 0, 1, &vertexBuffer, &stride, &offset );
	Helpers::Globals::Device->IASetIndexBuffer( indexBuffer, DXGI_FORMAT_R32_UINT, 0 );

	// draw cubes
	for(unsigned int i = 0; i<octreeCubes.size(); i++){
		effect.SetMatrix("World", octreeCubes[i]);

		effect.DrawIndexed(24);
	}
}

void Octree::SetUpDrawables(Structs::OctreeNode *node){
	if(!node)
		return;

	if(node->pObjList){
		D3DXMATRIX m, scale;

		D3DXMatrixScaling(&scale, node->halfWidth / HalfWidth, node->halfWidth / HalfWidth, node->halfWidth / HalfWidth);
		D3DXMatrixTranslation(&m, node->center.x, node->center.y, node->center.z);
		octreeCubes.push_back(scale * m);
	}

	if(node->pChild){
		for(int i = 0; i<8; i++){
			SetUpDrawables(node->pChild[i]);
		}
	}
}

void Octree::SetUpDraw(){

	D3D10_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 }, // pos
	};
	
	effect = Helpers::CustomEffect("Simple.fx", "SimpleTechnique", CUSTOM_EFFECT_TYPE_PIXEL | CUSTOM_EFFECT_TYPE_VERTEX, layout, 1);
	effect.AddVariable("World");
	effect.AddVariable("View");
	effect.AddVariable("Projection");

	Structs::SIMPLE_VERTEX vertices[8];

	vertices[0].pos = D3DXVECTOR3(-HalfWidth, -HalfWidth, -HalfWidth);
	vertices[1].pos = D3DXVECTOR3(-HalfWidth, -HalfWidth, HalfWidth);
	vertices[2].pos = D3DXVECTOR3(HalfWidth, -HalfWidth, HalfWidth);
	vertices[3].pos = D3DXVECTOR3(HalfWidth, -HalfWidth, -HalfWidth);

	vertices[4].pos = D3DXVECTOR3(-HalfWidth, HalfWidth, -HalfWidth);
	vertices[5].pos = D3DXVECTOR3(-HalfWidth, HalfWidth, HalfWidth);
	vertices[6].pos = D3DXVECTOR3(HalfWidth, HalfWidth, HalfWidth);
	vertices[7].pos = D3DXVECTOR3(HalfWidth, HalfWidth, -HalfWidth);

	D3D10_BUFFER_DESC bd;
	bd.Usage = D3D10_USAGE_DEFAULT;
	bd.ByteWidth = 8 * sizeof( Structs::SIMPLE_VERTEX);
	bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	D3D10_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = vertices;

	HR(Helpers::Globals::Device->CreateBuffer( &bd, &InitData, &vertexBuffer));	

	// Create indices
	unsigned int indices[] = { 
		0, 1, 
		1, 2,
		2, 3, 
		3, 0, 
		4, 5, 
		5, 6, 
		6, 7, 
		7, 4,
		0, 4, 
		1, 5, 
		2, 6, 
		3, 7, 
	};

	D3D10_BUFFER_DESC bufferDesc;
	bufferDesc.Usage           = D3D10_USAGE_DEFAULT;
	bufferDesc.ByteWidth       = 24* sizeof( unsigned int );
	bufferDesc.BindFlags       = D3D10_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags  = 0;
	bufferDesc.MiscFlags       = 0;

	D3D10_SUBRESOURCE_DATA indexInitData;
	indexInitData.pSysMem = indices;
	indexInitData.SysMemPitch = 0;
	indexInitData.SysMemSlicePitch = 0;
	HR(Helpers::Globals::Device->CreateBuffer( &bufferDesc, &indexInitData, &indexBuffer ));

	DrawSetUp = true;
	
	SetUpDrawables(ParentNode);
}