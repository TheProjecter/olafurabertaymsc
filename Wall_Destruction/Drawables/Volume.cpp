#include "Volume.h"
#include "Globals.h"
#include "TextureCreator.h"
#include "MathHelper.h"
#include <vector>
#include <algorithm>

Volume::Volume(){
}

void Volume::Init(bool d)
{
	this->pos = D3DXVECTOR3(World._41, World._42, World._43);
	this->deformable = d;

	if(deformable){
		D3DXVECTOR3 Min = D3DXVECTOR3(FLT_MAX, FLT_MAX, FLT_MAX), Max = D3DXVECTOR3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

		std::vector<D3DXVECTOR3> surfelVertexList;
		std::vector<D3DXVECTOR3> edgeVertexList;
		std::vector<ProjectStructs::SURFEL*> surfelList;
		std::vector<ProjectStructs::SURFEL_EDGE*> edgeList;
		
		for(int i = 0; i<surfaces.size(); i++){
			// read from the surfel vertex buffer
			ProjectStructs::SOLID_VERTEX* vertices = 0;

			HR(surfaces[i]->GetSurfelReadableBuffer()->Map(D3D10_MAP_READ, 0, reinterpret_cast< void** >(&vertices)));
			surfaces[i]->GetSurfelReadableBuffer()->Unmap();

			for(int j = 0; j<surfaces[i]->GetSurfaceSurfelCount(); j++){
				surfelVertexList.push_back(vertices[j*6].pos);
				surfelList.push_back(surfaces[i]->GetSurfaceSurfel(j));
				surfelVertexList.push_back(vertices[j*6+1].pos);
				surfelList.push_back(surfaces[i]->GetSurfaceSurfel(j));
				surfelVertexList.push_back(vertices[j*6+2].pos);
				surfelList.push_back(surfaces[i]->GetSurfaceSurfel(j));
				surfelVertexList.push_back(vertices[j*6+3].pos);
				surfelList.push_back(surfaces[i]->GetSurfaceSurfel(j));
				surfelVertexList.push_back(vertices[j*6+4].pos);
				surfelList.push_back(surfaces[i]->GetSurfaceSurfel(j));
				surfelVertexList.push_back(vertices[j*6+5].pos);
				surfelList.push_back(surfaces[i]->GetSurfaceSurfel(j));

			}

			vertices = 0;

			HR(surfaces[i]->GetEdgeReadableBuffer()->Map(D3D10_MAP_READ, 0, reinterpret_cast< void** >(&vertices)));
			surfaces[i]->GetEdgeReadableBuffer()->Unmap();

			for(int j = 0; j<surfaces[i]->GetEdgeSurfelCount(); j++){
				edgeVertexList.push_back(vertices[j*6].pos);
				edgeList.push_back(surfaces[i]->GetEdgeSurfel(j));
				edgeVertexList.push_back(vertices[j*6+1].pos);
				edgeList.push_back(surfaces[i]->GetEdgeSurfel(j));
				edgeVertexList.push_back(vertices[j*6+2].pos);
				edgeList.push_back(surfaces[i]->GetEdgeSurfel(j));
				edgeVertexList.push_back(vertices[j*6+3].pos);
				edgeList.push_back(surfaces[i]->GetEdgeSurfel(j));
				edgeVertexList.push_back(vertices[j*6+4].pos);
				edgeList.push_back(surfaces[i]->GetEdgeSurfel(j));
				edgeVertexList.push_back(vertices[j*6+5].pos);
				edgeList.push_back(surfaces[i]->GetEdgeSurfel(j));
			}
		}

		for (UINT i = 0; i < edgeVertexList.size(); i++)
		{
			if(Min.x > edgeVertexList[i].x)
				Min.x = edgeVertexList[i].x;
			else if(Max.x < edgeVertexList[i].x)
				Max.x = edgeVertexList[i].x;

			if(Min.y > edgeVertexList[i].y)
				Min.y = edgeVertexList[i].y;
			else if(Max.y < edgeVertexList[i].y)
				Max.y = edgeVertexList[i].y;

			if(Min.z > edgeVertexList[i].z)
				Min.z = edgeVertexList[i].z;
			else if(Max.z < edgeVertexList[i].z)
				Max.z = edgeVertexList[i].z;
		}

		grid = new PhyxelGrid(30, Min, Max, pos + (Min + (Max - Min)/2.0f));

		grid->InsertPoints(surfelVertexList, surfelList);
		grid->InsertEdges(edgeVertexList, edgeList);
		
		grid->Init();

		surfelList.clear();
		surfelList.swap(std::vector<ProjectStructs::SURFEL*>());
		edgeList.clear();
		edgeList.swap(std::vector<ProjectStructs::SURFEL_EDGE*>());
		surfelVertexList.clear();
		surfelVertexList.swap(std::vector<D3DXVECTOR3>());
		edgeVertexList.clear();
		edgeVertexList.swap(std::vector<D3DXVECTOR3>());

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
		if(Helpers::Globals::DRAW_PHYXEL_GRID){
			grid->Draw();
		}
	}
}


void Volume::Update(float dt){
	for(int i = 0; i<surfaces.size(); i++){
		surfaces[i]->Update(dt);
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
	}
}


