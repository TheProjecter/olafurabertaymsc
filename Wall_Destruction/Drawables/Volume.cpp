#include "Volume.h"
#include "Globals.h"
#include "TextureCreator.h"
#include "MathHelper.h"
#include "FractureManager.h"
#include "DeletedStructHolder.h"
#include "SurfelsToResample.h"
#include "Algorithms.h"
#include "DebugToFile.h"
#include "PhysicsWrapper.h"
#include <vector>
#include <map>
#include <algorithm>


Volume::Volume(ProjectStructs::MATERIAL_PROPERTIES materialProperties){
	
	this->materialProperties = materialProperties;
}

void Volume::Init()
{
	this->pos = D3DXVECTOR3(World._41, World._42, World._43);

	//cracked = false;

	D3DXVECTOR3 Min = D3DXVECTOR3(FLT_MAX, FLT_MAX, FLT_MAX), Max = D3DXVECTOR3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	for(int i = 0; i<newSurfels.size(); i++){

		float sizeOfSurfel = max(D3DXVec3Length(&newSurfels[i]->vertex->majorAxis), D3DXVec3Length(&newSurfels[i]->vertex->minorAxis));

		if(Min.x > newSurfels[i]->vertex->pos.x - sizeOfSurfel)
			Min.x = newSurfels[i]->vertex->pos.x - sizeOfSurfel;
		if(Min.y > newSurfels[i]->vertex->pos.y - sizeOfSurfel)
			Min.y = newSurfels[i]->vertex->pos.y - sizeOfSurfel;
		if(Min.z > newSurfels[i]->vertex->pos.z - sizeOfSurfel)
			Min.z = newSurfels[i]->vertex->pos.z - sizeOfSurfel;

		if(Max.x < newSurfels[i]->vertex->pos.x + sizeOfSurfel)
			Max.x = newSurfels[i]->vertex->pos.x + sizeOfSurfel;
		if(Max.y < newSurfels[i]->vertex->pos.y + sizeOfSurfel)
			Max.y = newSurfels[i]->vertex->pos.y + sizeOfSurfel;
		if(Max.z < newSurfels[i]->vertex->pos.z + sizeOfSurfel)
			Max.z = newSurfels[i]->vertex->pos.z + sizeOfSurfel;
	}

	if(materialProperties.deformable){
		D3DXVECTOR3 halfWidth = (Max - Min) * 0.5f;
		Min -= 3.0f * halfWidth;
		Max += 3.0f * halfWidth;
	}
	
	vertexBufferGrid = new VertexBufferGrid(Min, Max, pos, materialProperties);

	vertexBufferGrid->InsertPoints(newSurfels);

	if(materialProperties.deformable){

		this->phyxelGrid = new PhyxelGrid(Min, Max , pos + (Min + (Max - Min)/2.0f), materialProperties );

		vertexBufferGrid->SetPhyxelGrid(this->phyxelGrid);

		vertexBufferGrid->InitCells();
		
		std::vector<D3DXVECTOR3> surfelVertexList;
		std::vector<ProjectStructs::SURFEL*> surfelList;

		std::map<int, ProjectStructs::Vertex_Grid_Cell*>::const_iterator vertexGridCellIterator;

		for(vertexGridCellIterator = vertexBufferGrid->GetFrontOfPopulatedCells(); 
			vertexGridCellIterator != vertexBufferGrid->GetEndOfPopulatedCells(); vertexGridCellIterator++){
			
			ProjectStructs::SOLID_VERTEX* vertices = 0;
			HR(vertexGridCellIterator->second->readableVertexBuffer->Map(D3D10_MAP_READ, 0, reinterpret_cast< void** >(&vertices)));
			vertexGridCellIterator->second->readableVertexBuffer->Unmap();

			for(int i = 0; i < vertexGridCellIterator->second->surfels.size(); i++){
				surfelVertexList.push_back(vertices[i*6].pos);
				surfelList.push_back(vertexGridCellIterator->second->surfels[i]);
				surfelVertexList.push_back(vertices[i*6+1].pos);
				surfelList.push_back(vertexGridCellIterator->second->surfels[i]);
				surfelVertexList.push_back(vertices[i*6+2].pos);
				surfelList.push_back(vertexGridCellIterator->second->surfels[i]);
				//surfelVertexList.push_back(vertices[i*6+3].pos);
				//surfelList.push_back(vertexGridCellIterator->second->surfels[i]);
				surfelVertexList.push_back(vertices[i*6+4].pos);
				surfelList.push_back(vertexGridCellIterator->second->surfels[i]);
				//surfelVertexList.push_back(vertices[i*6+5].pos);
				//surfelList.push_back(vertexGridCellIterator->second->surfels[i]);
			}
		}

		phyxelGrid->InsertPoints(surfelVertexList, surfelList);
		
		phyxelGrid->Init();

		for(UINT i = 0; i<surfelList.size(); i++){
			Algorithms::CalculateNeighbors(surfelList[i]);
		}

		surfelList.clear();
		surfelList.swap(std::vector<ProjectStructs::SURFEL*>());
		
		surfelVertexList.clear();
		surfelVertexList.swap(std::vector<D3DXVECTOR3>());
	}
	else{
		vertexBufferGrid->InitCells();
	}
}
/*
void Volume::ResetSurfaces(){
	for(unsigned int i = 0; i < surfaces.size(); i++){
		surfaces[i]->ResetBuffers();
	}
}*/

void Volume::AddSurfel(ProjectStructs::SURFEL* surfel){

	if(surfel->vertex->frontFacing == -1){
		D3DXVECTOR3 normalCheck;
		D3DXVec3Cross(&normalCheck, &surfel->vertex->minorAxis, &surfel->vertex->majorAxis);
		surfel->vertex->frontFacing = MathHelper::Sign(normalCheck.x) == MathHelper::Sign(surfel->vertex->normal.x) &&
			MathHelper::Sign(normalCheck.y) == MathHelper::Sign(surfel->vertex->normal.y) &&
			MathHelper::Sign(normalCheck.z) == MathHelper::Sign(surfel->vertex->normal.z) ? 1 : 0;
	}

	newSurfels.push_back(surfel);
}

void Volume::Draw(){

/*	for(unsigned int i = 0; i<surfaces.size(); i++){
		surfaces[i]->Draw();
	}
	*/

	vertexBufferGrid->Draw(this->World);
	
	if(materialProperties.deformable){

		if(Helpers::Globals::DRAW_PHYXEL_GRID)
			phyxelGrid->Draw();
		if(Helpers::Globals::DRAW_PHYXELS)
			ImpactList::Draw();
	}
}

void Volume::Update(float dt){

	/*if(cracked){
		ResetSurfaces();
		cracked = false;
	}*/

	// Check if the user wants to clear the impacts
	ImpactList::Update(dt);
	if(materialProperties.deformable)
		ImpactList::CalculateImpacts();

	if(materialProperties.deformable && ImpactList::GetImpactCount() != 0){

		VertexBufferGrid::LastRadiusScale = 1.0f;
		VertexBufferGrid::RadiusScale = 1.0f;

		SurfelsToResample::Clear();

		// displace and fracture the phyxel node
		DebugToFile::StartTimer();
		FractureManager::CalculateAndInitiateFractures(this, dt);	
		DebugToFile::EndTimer("Calculated stress in ");

		D3DXVECTOR3 displacement, intersectingCellDisplacement;
		int count = 0;

		float minHeight = FLT_MAX;
		float minWidth = FLT_MAX;

		DebugToFile::StartTimer();
		// resample the affected surfels
		for(unsigned int i = 0; i<ImpactList::GetImpactCount(); i++){			
			ProjectStructs::PHYXEL_NODE* phyxel = ImpactList::GetImpact(i)->phyxel;
			ProjectStructs::SURFEL* impactedSurfel = ImpactList::GetImpact(i)->surfel;
			count = 0;

//			if(phyxel->isChanged){
				// take out the surfel and insert a crater
			float w = FractureManager::CalculateWeight(impactedSurfel->vertex->pos + this->pos, phyxel->pos, phyxel->supportRadius);

			if(w != 0.0f)
			{
				D3DXVECTOR3 displacement = w * (phyxel->displacement- impactedSurfel->lastDisplacement);
				SurfelsToResample::AddExistingSurfel(impactedSurfel);

				impactedSurfel->displacement += displacement;
				impactedSurfel->displacementCount++;
			}

			for(std::map<float, ProjectStructs::SURFEL*>::iterator surfelNeighborIterator = impactedSurfel->neighbors.begin(); 
				surfelNeighborIterator != impactedSurfel->neighbors.end(); surfelNeighborIterator++){

					ProjectStructs::SURFEL* surfel= surfelNeighborIterator->second;		

					float w = FractureManager::CalculateWeight(surfel->vertex->pos + this->pos, phyxel->pos, phyxel->supportRadius);

					if(w != 0.0f)
					{
						D3DXVECTOR3 displacement = w * (phyxel->displacement- surfel->lastDisplacement);
						SurfelsToResample::AddExistingSurfel(surfel);

						surfel->displacement += displacement;
						surfel->displacementCount++;
					}

			}

			/*	for(unsigned int j = 0; j < phyxel->parent->surfels.size(); j++){
					ProjectStructs::SURFEL* surfel= phyxel->parent->surfels[j];		
					
					//float w = FractureManager::CalculateWeight(surfel->vertex->pos + this->pos, ImpactList::GetImpact(i)->impactPos, 2.25f * phyxel->supportRadius);
					//float w = FractureManager::CalculateWeight(surfel->vertex->pos + this->pos, ImpactList::GetImpact(i)->impactPos, 3.0f* phyxel->supportRadius);
					//float w = FractureManager::CalculateWeight(surfel->vertex->pos + this->pos, phyxel->pos, phyxel->supportRadius);
					float w = FractureManager::CalculateWeight(surfel->vertex->pos + this->pos, phyxel->pos, phyxel->supportRadius);
					
					if(w != 0.0f)
					{
						D3DXVECTOR3 displacement = w * (phyxel->displacement- surfel->lastDisplacement);
						SurfelsToResample::AddExistingSurfel(surfel);

						surfel->displacement += displacement;
						surfel->displacementCount++;
					}
				}*/
				phyxel->isChanged = false;
//			}
		}
		DebugToFile::EndTimer("Moved the surfels");

		if(SurfelsToResample::GetSurfelCount() != 0){

			DebugToFile::StartTimer();
			for( int i = 0; i < SurfelsToResample::GetSurfelCount(); i++){

				ProjectStructs::SURFEL* surfel = SurfelsToResample::GetSurfel(i);
				if(surfel->displacementCount == 0)
					continue;

				D3DXVECTOR3 minor = SurfelsToResample::GetSurfel(i)->vertex->minorAxis;
				D3DXVECTOR3 major = SurfelsToResample::GetSurfel(i)->vertex->majorAxis;
				D3DXVECTOR3 normal = SurfelsToResample::GetSurfel(i)->vertex->normal;
				D3DXVECTOR3 displacement = surfel->displacement;

				D3DXMATRIX majorRotation, minorRotation, normalRotation;

				D3DXVec3Normalize(&normal, &(normal-displacement));

				float majorAngle = MathHelper::Get3DAngle(displacement + major, major, normal); 
				float minorAngle = MathHelper::Get3DAngle(displacement + minor, minor, normal);

				D3DXMatrixRotationAxis(&majorRotation, &(displacement), majorAngle);
				D3DXMatrixRotationAxis(&minorRotation, &(displacement), minorAngle);

				D3DXVec3TransformCoord(&major, &major, &majorRotation);
				D3DXVec3TransformCoord(&minor, &minor, &minorRotation);

				surfel->vertex->minorAxis = minor;
				surfel->vertex->majorAxis = major;
				surfel->vertex->normal = normal;

				surfel->vertex->pos += displacement;///(float)surfel->displacementCount;
				surfel->lastDisplacement += displacement;///(float)surfel->displacementCount;
				surfel->displacementCount = 0;
				surfel->displacement *= 0;
				vertexBufferGrid->ResetSurfel(surfel);
			}
			DebugToFile::EndTimer("Reset surfels");

			std::vector<ProjectStructs::SURFEL*> surfels;

		//	PhysicsWrapper::LockWorld();

//			for(int j = 0; j<2; j++){
/*
				for(int i = 0; i<surfels.size(); i++){
					SurfelsToResample::AddExistingSurfel(surfels[i]);
				}			
				surfels.clear();
*/
				DebugToFile::StartTimer();
				SurfelsToResample::ResetGrid();
				DebugToFile::EndTimer("Reset grid");

				DebugToFile::StartTimer();
				for(int i = 0; i < SurfelsToResample::GetSurfelCount(); i++){
					//DebugToFile::StartTimer();
					
					if(D3DXVec3Length(&SurfelsToResample::GetSurfel(i)->vertex->majorAxis) * D3DXVec3Length(&SurfelsToResample::GetSurfel(i)->vertex->minorAxis) > materialProperties.minimunSurfelSize)
						Algorithms::RefineSurfel(SurfelsToResample::GetSurfel(i));

					//DebugToFile::EndTimer("Refined one surfel");
				}
				DebugToFile::EndTimer("Refined all surfels");

				DebugToFile::StartTimer();
				SurfelsToResample::Resample();	
				DebugToFile::EndTimer("Resampled surfels");

				DebugToFile::StartTimer();
				for(int i = 0; i < SurfelsToResample::GetNewSurfelSize(); i++){
					if(!vertexBufferGrid->PopulateNode(SurfelsToResample::GetNewSurfel(i))){
						SurfelsToResample::DeleteNewSurfel(i);
						i--;
					}
				}
				DebugToFile::EndTimer("populated node of vertexbuffergrid");

				DebugToFile::StartTimer();
				vertexBufferGrid->Update();
				DebugToFile::EndTimer("Updated vertexbuffergrid");

				// insert new surfels into the grid
				DebugToFile::StartTimer();
				for(int i = 0; i<SurfelsToResample::GetNewSurfelSize(); i++){

					ProjectStructs::SURFEL* surfel = SurfelsToResample::GetNewSurfel(i);

					ProjectStructs::SOLID_VERTEX* vertices = 0;
					HR(surfel->vertexGridCell->readableVertexBuffer->Map(D3D10_MAP_READ, 0, reinterpret_cast< void** >(&vertices)));
					surfel->vertexGridCell->readableVertexBuffer->Unmap();

					std::vector<ProjectStructs::SURFEL*>::iterator index = find(surfel->vertexGridCell->surfels.begin(), surfel->vertexGridCell->surfels.end(), surfel);

					if(index != surfel->vertexGridCell->surfels.end()){
						int surfelIndexInVertexBuffer = distance(surfel->vertexGridCell->surfels.begin(), index);

						std::vector<D3DXVECTOR3> points;
						points.push_back(vertices[surfelIndexInVertexBuffer * 6].pos);
						points.push_back(vertices[surfelIndexInVertexBuffer * 6+1].pos);
						points.push_back(vertices[surfelIndexInVertexBuffer * 6+2].pos);
						//points.push_back(vertices[surfelIndexInVertexBuffer * 6+3].pos);
						points.push_back(vertices[surfelIndexInVertexBuffer * 6+4].pos);
						//points.push_back(vertices[surfelIndexInVertexBuffer * 6+5].pos);

						phyxelGrid->InsertPoint(points, surfel);
					}
					else
						printf("....");
				}
				DebugToFile::EndTimer("put surfels into the phyxelgrid");

				DebugToFile::StartTimer();
				SurfelsToResample::CalculateNeighbors();
				DebugToFile::EndTimer("Calculated neighbors");
				
				VertexBufferGrid::isChanged = true;

				DebugToFile::StartTimer();
				SurfelsToResample::Clear();
				DebugToFile::EndTimer("Clear");				
			}
		//	PhysicsWrapper::UnLockWorld();
			
//		}

		// clear the impact list
		if(ImpactList::GetImpactCount() != 0)
			ImpactList::Emptylist();
	}
	
	vertexBufferGrid->Update();

/*
	for(unsigned int i = 0; i<surfaces.size(); i++){
		surfaces[i]->Update(dt);
	}*/
}

/*
void Volume::AddSurface(Surface *surface){
	surface->SetWorld(World);
	surface->Init(GetMaterialProperties());
	surfaces.push_back(surface);
}
*/

void Volume::AddCrack(ProjectStructs::CRACK_NODE* crack){
	//this->phyxelGrid->InsertCrack(crack);

/*	if(crack->s1)
		surfaces[0]->AddSurfel(crack->s1);
	if(crack->s2)
		surfaces[0]->AddSurfel(crack->s2);

	if(crack->e1)
		surfaces[0]->AddEdgeSurfel(crack->e1);
	if(crack->e2)
		surfaces[0]->AddEdgeSurfel(crack->e2);
*/
//	cracked = true;
}

void Volume::CleanUp(){
/*	for(UINT i = 0; i<surfaces.size(); i++){
		surfaces[i]->CleanUp();
		delete surfaces[i];
	}
*/
	vertexBufferGrid->CleanUp();
	delete vertexBufferGrid;

	if(materialProperties.deformable){
		phyxelGrid->CleanUp();
		delete phyxelGrid;

		ImpactList::CleanUp();
	}
}



