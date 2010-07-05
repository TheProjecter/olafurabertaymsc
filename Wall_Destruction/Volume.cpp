#include "Volume.h"
#include "Globals.h"
#include "TextureCreator.h"

Volume::Volume(){
}

void Volume::Init()
{
	D3DXVECTOR3 Min = D3DXVECTOR3(FLT_MAX, FLT_MAX, FLT_MAX), Max = D3DXVECTOR3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	/*for (int i = 0; i < surfaceCount ; i++)
	{
		for (int j = 0; j < surfaces[i]->GetEdgeSurfelCount() ; j++)
		{
			if(Min.x > surfaces[i]->GetEdgeSurfel(j).pos.x)
				Min.x = surfaces[i]->GetEdgeSurfel(j).pos.x;
			if(Max.x < surfaces[i]->GetEdgeSurfel(j).pos.x)
				Max.x = surfaces[i]->GetEdgeSurfel(j).pos.x;

			if(Min.y > surfaces[i]->GetEdgeSurfel(j).pos.y)
				Min.y = surfaces[i]->GetEdgeSurfel(j).pos.y;
			if(Max.y < surfaces[i]->GetEdgeSurfel(j).pos.y)
				Max.y = surfaces[i]->GetEdgeSurfel(j).pos.y;

			if(Min.z > surfaces[i]->GetEdgeSurfel(j).pos.z)
				Min.z = surfaces[i]->GetEdgeSurfel(j).pos.z;
			if(Max.z < surfaces[i]->GetEdgeSurfel(j).pos.z)
				Max.z = surfaces[i]->GetEdgeSurfel(j).pos.z;	
		}
	}*/

	this->pos = D3DXVECTOR3(World._41, World._42, World._43);

	//octree = Octree(pos, max(Max.x - Min.x, max(Max.y - Min.y, Max.z - Min.z))*0.5f+ 0.05f, 2);

	// determine the intersection between surfaces
	/*for(int i = 0; i<surfaceCount; i++){
		for(int j = i+1; j<surfaceCount; j++){
			surfaces[i].edgeSurfels
		}
	}*/

	//for(int i = 0; i<surfaceCount; i++){
	//	surfaces[i]->Init();
	//	octree.InsertSurface(surfaces[i]);
	//}
}

void Volume::And(Volume *volume){

}

void Volume::Or(Volume *volume){

}

void Volume::Draw(){

	for(int  i = 0; i<surfaces.size(); i++){
		surfaces[i]->Draw();
	}

	/*	if(Helpers::Globals::DRAW_OCTREE){
		octree.Draw();
	}
	else{
		octree.CleanUpDrawables();
	}*/
}


void Volume::Update(float dt){

}

void Volume::AddSurface(Surface *surface){
	surface->SetWorld(World);
	surface->Init();
	surfaces.push_back(surface);
}

void Volume::CleanUp(){
	for(UINT i = 0; i<surfaces.size(); i++){
		surfaces[i]->CleanUp();
		delete surfaces[i];
	}
}


