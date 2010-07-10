#include "Volume.h"
#include "Globals.h"
#include "TextureCreator.h"
#include "MathHelper.h"

Volume::Volume(){
}

void Volume::Init(bool d)
{
	this->pos = D3DXVECTOR3(World._41, World._42, World._43);
	this->deformable = d;

	if(deformable){
		D3DXVECTOR3 Min = D3DXVECTOR3(FLT_MAX, FLT_MAX, FLT_MAX), Max = D3DXVECTOR3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

		for (UINT i = 0; i < surfaces.size(); i++)
		{
			for (int j = 0; j < surfaces[i]->GetEdgeSurfelCount() ; j++)
			{
				bool posMaj = MathHelper::PositiveMajor(surfaces[i]->GetEdgeSurfel(j));
				bool posMin = MathHelper::PositiveMinor(surfaces[i]->GetEdgeSurfel(j));
				bool negMaj = MathHelper::NegativeMajor(surfaces[i]->GetEdgeSurfel(j));
				bool negMin = MathHelper::NegativeMinor(surfaces[i]->GetEdgeSurfel(j));

				if(negMaj && Min.x > surfaces[i]->GetEdgeSurfel(j)->vertex.pos.x - surfaces[i]->GetEdgeSurfel(j)->vertex.majorAxis.x)
					Min.x = surfaces[i]->GetEdgeSurfel(j)->vertex.pos.x - surfaces[i]->GetEdgeSurfel(j)->vertex.majorAxis.x;
				if(posMaj && Max.x < surfaces[i]->GetEdgeSurfel(j)->vertex.pos.x + surfaces[i]->GetEdgeSurfel(j)->vertex.majorAxis.x)
					Max.x = surfaces[i]->GetEdgeSurfel(j)->vertex.pos.x + surfaces[i]->GetEdgeSurfel(j)->vertex.majorAxis.x;

				if(negMin && Min.x > surfaces[i]->GetEdgeSurfel(j)->vertex.pos.x - surfaces[i]->GetEdgeSurfel(j)->vertex.minorAxis.x)
					Min.x = surfaces[i]->GetEdgeSurfel(j)->vertex.pos.x - surfaces[i]->GetEdgeSurfel(j)->vertex.minorAxis.x;
				if(posMin && Max.x < surfaces[i]->GetEdgeSurfel(j)->vertex.pos.x + surfaces[i]->GetEdgeSurfel(j)->vertex.minorAxis.x)
					Max.x = surfaces[i]->GetEdgeSurfel(j)->vertex.pos.x + surfaces[i]->GetEdgeSurfel(j)->vertex.minorAxis.x;


				if(negMaj && Min.y > surfaces[i]->GetEdgeSurfel(j)->vertex.pos.y - surfaces[i]->GetEdgeSurfel(j)->vertex.majorAxis.y)
					Min.y = surfaces[i]->GetEdgeSurfel(j)->vertex.pos.y - surfaces[i]->GetEdgeSurfel(j)->vertex.majorAxis.y;
				if(posMaj && Max.y < surfaces[i]->GetEdgeSurfel(j)->vertex.pos.y + surfaces[i]->GetEdgeSurfel(j)->vertex.majorAxis.y)
					Max.y = surfaces[i]->GetEdgeSurfel(j)->vertex.pos.y + surfaces[i]->GetEdgeSurfel(j)->vertex.majorAxis.y;

				if(negMin && Min.y > surfaces[i]->GetEdgeSurfel(j)->vertex.pos.y - surfaces[i]->GetEdgeSurfel(j)->vertex.minorAxis.y)
					Min.y = surfaces[i]->GetEdgeSurfel(j)->vertex.pos.y - surfaces[i]->GetEdgeSurfel(j)->vertex.minorAxis.y;
				if(posMin && Max.y < surfaces[i]->GetEdgeSurfel(j)->vertex.pos.y + surfaces[i]->GetEdgeSurfel(j)->vertex.minorAxis.y)
					Max.y = surfaces[i]->GetEdgeSurfel(j)->vertex.pos.y + surfaces[i]->GetEdgeSurfel(j)->vertex.minorAxis.y;


				if(negMaj && Min.z > surfaces[i]->GetEdgeSurfel(j)->vertex.pos.z - surfaces[i]->GetEdgeSurfel(j)->vertex.majorAxis.z)
					Min.z = surfaces[i]->GetEdgeSurfel(j)->vertex.pos.z - surfaces[i]->GetEdgeSurfel(j)->vertex.majorAxis.z;
				if(posMaj && Max.z < surfaces[i]->GetEdgeSurfel(j)->vertex.pos.z + surfaces[i]->GetEdgeSurfel(j)->vertex.majorAxis.z)
					Max.z = surfaces[i]->GetEdgeSurfel(j)->vertex.pos.z + surfaces[i]->GetEdgeSurfel(j)->vertex.majorAxis.z;

				if(negMin && Min.z > surfaces[i]->GetEdgeSurfel(j)->vertex.pos.z - surfaces[i]->GetEdgeSurfel(j)->vertex.minorAxis.z)
					Min.z = surfaces[i]->GetEdgeSurfel(j)->vertex.pos.z - surfaces[i]->GetEdgeSurfel(j)->vertex.minorAxis.z;
				if(posMin && Max.z < surfaces[i]->GetEdgeSurfel(j)->vertex.pos.z + surfaces[i]->GetEdgeSurfel(j)->vertex.minorAxis.z)
					Max.z = surfaces[i]->GetEdgeSurfel(j)->vertex.pos.z + surfaces[i]->GetEdgeSurfel(j)->vertex.minorAxis.z;
			}
		}

		float halfwidth = max(Max.x - Min.x, max(Max.y - Min.y, Max.z - Min.z))*0.5f;

		grid = new PhyxelGrid(20, Min, Max, pos + (Min + (Max - Min)/2.0f));

		for(UINT i = 0; i<surfaces.size(); i++){
			grid->InsertSurface(surfaces[i]);
		}
		grid->Init();
	}
}

void Volume::And(Volume *volume){

}

void Volume::Or(Volume *volume){

}

void Volume::Draw(){

	for(unsigned int i = 0; i<surfaces.size(); i++){
		surfaces[i]->Draw();
	}

	if(deformable){
		if(Helpers::Globals::DRAW_OCTREE){
			grid->Draw();
		//	octree->Draw(this->pos);
		}
		else{
		//	octree->CleanUpDrawables();
		}

		/*if(Helpers::Globals::DRAW_PHYXELS)
			phyxelObject.Draw();*/
	}
}


void Volume::Update(float dt){
	for(int i = 0; i<surfaces.size(); i++){
		surfaces[i]->Update(dt);
	}

	if(deformable){
		//phyxelObject.Update(dt);
	}

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
	if(deformable){
		grid->CleanUp();
		delete grid;

		//phyxelObject.CleanUp();

		//octree->CleanUp();
		//delete octree;
	}
}


