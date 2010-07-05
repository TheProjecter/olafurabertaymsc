#include "PhyxelObject.h"

void PhyxelObject::Init(Octree *octree)
{
	Structs::OctreeNode *node = octree->GetOctreeNode();

	int index = 0;
	drawBeenSetup = false;
	bool done = false;

	do{
		
		index = -1;
		while(index == -1){
			// go down to the bottom
			for(int i = 0; i<8; i++){
				if(node->pChild[i] && !node->pChild[i]->checked){
					node = node->pChild[i];
					index = i;
					break;   
				}
			}

			if(index == -1){
				if(node->pChild[0]){
					node->pChild[0]->checked = true;
					node->pChild[1]->checked = true;
					node->pChild[2]->checked = true;
					node->pChild[3]->checked = true;
					node->pChild[4]->checked = true;
					node->pChild[5]->checked = true;
					node->pChild[6]->checked = true;
					node->pChild[7]->checked = true;
				}

				if(node->parent && (!node->parent->pChild[0]->checked || !node->parent->pChild[1]->checked || 
					!node->parent->pChild[2]->checked || !node->parent->pChild[3]->checked || !node->parent->pChild[4]->checked || 
					!node->parent->pChild[5]->checked || !node->parent->pChild[6]->checked || !node->parent->pChild[7]->checked)){
					node = node->parent;
					index = -1;
				}
				else{	
					node = node->parent;
					index = 1;
				}
			}
		}

		if(node->pObjList)
		{
			Structs::SurfelNode *tmpnode = node->pObjList;
			bool foundASurfel = false;
			
			D3DXVECTOR3 normal;
			while(tmpnode && !foundASurfel ){
				// check if the surfel has a normal that faces away from the center		
				foundASurfel = D3DXVec3Dot(&(tmpnode->surfel.pos - node->center), &tmpnode->surfel.normal) > 0.0f;

				normal = tmpnode->surfel.normal;
				tmpnode = tmpnode->nextSurfel;
			}


			Structs::PHYXEL_NODE phyxel;
			if(foundASurfel){
				phyxel.pos = node->center;
			}
			else{
				phyxel.pos = node->center - 2*node->halfWidth*normal;
			}

			phyxel.radius = 2*node->halfWidth;
			phyxel.mass = 4.0f/3.0f * Helpers::Globals::PI * pow(phyxel.radius, 3) * density;

			phyxels.push_back(phyxel);
		}

		node->checked = true;

		done = !node->parent && node->pChild[0]->checked && node->pChild[1]->checked && node->pChild[2]->checked && node->pChild[3]->checked
			&& node->pChild[4]->checked && node->pChild[5]->checked && node->pChild[6]->checked && node->pChild[7]->checked;
	}while(!done);


}

void PhyxelObject::CleanUpDrawables(){
	if(drawBeenSetup){
		sphereEffect.CleanUp();
		phyxelSphere.CleanUp();

		drawBeenSetup = false;			
	}
}

void PhyxelObject::CleanUp(){
	CleanUpDrawables();
	phyxelSphere.CleanUp();
	phyxels.clear();
}

void PhyxelObject::SetUpDraw(){

	D3D10_INPUT_ELEMENT_DESC sphereLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 }, // pos
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(D3DXVECTOR3), D3D10_INPUT_PER_VERTEX_DATA, 0 }, // normal
	};

	this->SetMaterialInfo(0.600672f, 0.668533f);

	sphereEffect = Helpers::CustomEffect("SphereEffect.fx", "PhyxelTechnique", CUSTOM_EFFECT_TYPE_PIXEL | CUSTOM_EFFECT_TYPE_VERTEX, sphereLayout, 2);
	sphereEffect.AddVariable("World");
	sphereEffect.AddVariable("View");
	sphereEffect.AddVariable("Projection");
	// image taken from http://www.cgtextures.com/getfile.php/ConcreteRough0019_2_S.jpg?id=38615&s=s&PHPSESSID=fb96f672ea0d7aff54fd54fe3f539e00
	sphereEffect.AddTexture("tex", "Textures\\Projectile.jpg");

	sphereEffect.AddVariable("Color");
	sphereEffect.AddVariable("LightPos");
	sphereEffect.AddVariable("AmbientColor");
	sphereEffect.AddVariable("CameraPos");
	sphereEffect.AddVariable("LightDirection");
	sphereEffect.AddVariable("A");
	sphereEffect.AddVariable("B");
	sphereEffect.AddVariable("rhoOverPi");
	sphereEffect.AddVariable("LightColor");		

	sphereEffect.SetFloatVector("Color", D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));
	sphereEffect.SetFloatVector("AmbientColor", Helpers::Globals::AppLight.GetAmbientColor());
	sphereEffect.SetFloatVector("LightPos", Helpers::Globals::AppLight.GetPosition());
	sphereEffect.SetFloatVector("LightDirection", Helpers::Globals::AppLight.GetDirection());
	sphereEffect.SetFloat("A", this->A);
	sphereEffect.SetFloat("B", this->B);
	sphereEffect.SetFloat("rhoOverPi", this->rhoOverPi);
	sphereEffect.SetFloatVector("LightColor", Helpers::Globals::AppLight.GetColor());

	phyxelSphere = Sphere();
	phyxelSphere.init(0.1f, 3, 3);

	this->drawBeenSetup = true;
}

void PhyxelObject::Draw(){

	if(!drawBeenSetup)
		SetUpDraw();

	sphereEffect.SetMatrix("View", Helpers::Globals::AppCamera.View());
	sphereEffect.SetMatrix("Projection", Helpers::Globals::AppCamera.Projection());
	sphereEffect.SetFloatVector("CameraPos", Helpers::Globals::AppCamera.Position());
	sphereEffect.PreDraw();

	// draw phyxels
	for(unsigned int i = 0; i< phyxels.size(); i++){
		D3DXMatrixTranslation(&world, phyxels[i].pos.x, phyxels[i].pos.y, phyxels[i].pos.z);
		sphereEffect.SetMatrix("World", world);
		phyxelSphere.Draw(&sphereEffect);
	}
}
