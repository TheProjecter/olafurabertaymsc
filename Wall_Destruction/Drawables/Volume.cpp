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

std::map<ProjectStructs::PHYXEL_NODE*, float> Volume::weights;

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

	
	D3DXVECTOR3 dimensions = (Max - Min) * 0.5f;
// 	Min -= 3.0f * dimensions;
// 	Max += 3.0f * dimensions;
	
	vertexBufferGrid = new VertexBufferGrid(Min - 3.0f * dimensions, Max + 3.0f * dimensions, pos, materialProperties);

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

		for(UINT i = 0; i<surfelList.size(); i+=4){
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

	DebugToFile::Debug("Created %d surfels", newSurfels.size());
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

	vertexBufferGrid->Draw(this->World);
	
	if(materialProperties.deformable){

		if(Helpers::Globals::DRAW_PHYXEL_GRID)
			phyxelGrid->Draw();
		if(Helpers::Globals::DRAW_PHYXELS)
			ImpactList::Draw();
	}
}

struct phyxelComparator{
	bool operator() (ProjectStructs::PHYXEL_NODE* i,ProjectStructs::PHYXEL_NODE* j) { 
		/*float w1, w2;
		if(weights.find(i) != weights.end())
			w1 = weights[i];
		else {
			w1 = FractureManager::CalculateWeight(surfel->initialPosition+ pos, i->pos, i->supportRadius);
			weights[i] = w1;
		}
		if(weights.find(j) != weights.end())
			w2 = weights[j];
		else {
			w2 = FractureManager::CalculateWeight(surfel->initialPosition+ pos, j->pos, j->supportRadius);
			weights[j] = w2;
		}

		return  w1 > w2;*/
		Volume::weights[i] = FractureManager::CalculateWeight(surfel->vertex->pos + pos, i->pos + i->displacement, i->supportRadius);
		Volume::weights[j] = FractureManager::CalculateWeight(surfel->vertex->pos + pos, j->pos + j->displacement, j->supportRadius);
		return Volume::weights[i] > Volume::weights[j];
	
		//return FractureManager::CalculateWeight(surfel->initialPosition+ pos, i->pos, i->supportRadius) > FractureManager::CalculateWeight(surfel->initialPosition+ pos, j->pos, j->supportRadius);

	}
	ProjectStructs::SURFEL* surfel;
	D3DXVECTOR3 pos;
} myPhyxelComparator;

void Volume::Update(float dt){


	/*
		Calculate stress and strain

		reset the vertex buffers that are the neighbours of the impacted vertex buffer
			
		calculate displacement for the surfels in those vertex buffers

		refine the surfels

		calculate neighbours
	*/
	
	//ImpactList::Update(dt);

	if(!materialProperties.deformable){
		vertexBufferGrid->Update();
		return;
	}
	
	ImpactList::CalculateImpacts();

	/*if(ImpactList::GetAffectedPhyxels().size() == 0)
	{
		vertexBufferGrid->Update();
		return;
	}*/
	if(ImpactList::GetAffectedPhyxels().size() == 0/* && SurfelsToResample::GetSurfelsLeftCount() == 0*/)
	{
		vertexBufferGrid->Update();
		return;
	}

	std::vector<ProjectStructs::PHYXEL_NODE*> phyxels = ImpactList::GetAffectedPhyxelVector();

	PhysicsWrapper::LW();

	VertexBufferGrid::RadiusScale = 1.0f;
	VertexBufferGrid::isChanged = true;

	//SurfelsToResample::Clear();

	DebugToFile::StartTimer();
	if(ImpactList::GetAffectedPhyxels().size() != 0){
		FractureManager::CalculateAndInitiateFractures(this, dt);

		std::vector<ProjectStructs::Vertex_Grid_Cell*> changedCells;

//		DebugToFile::StartTimer();
/*		std::map<ProjectStructs::SURFEL*, std::vector<ProjectStructs::PHYXEL_NODE*>>::iterator surfelToPhyxelIterator;

		for(surfelToPhyxelIterator = ImpactList::surfelsToPhyxel.begin(); surfelToPhyxelIterator != ImpactList::surfelsToPhyxel.end(); surfelToPhyxelIterator++){*/
		std::map<ProjectStructs::SURFEL*, bool> affectedSurfels = ImpactList::GetAffectedSurfels();
		std::map<ProjectStructs::SURFEL*, bool>::iterator surfelIterator = affectedSurfels.begin();
		for( ; surfelIterator != affectedSurfels.end(); surfelIterator++){

			ProjectStructs::SURFEL* impactedSurfel = surfelIterator->first;
		/*	myPhyxelComparator.surfel = impactedSurfel;
			myPhyxelComparator.pos = this->pos;
			Volume::weights.clear();

			sort(phyxels.begin(), phyxels.end(), myPhyxelComparator);
*/
			//ProjectStructs::SURFEL* impactedSurfel = affectedSurfels[i];
			impactedSurfel->displacement *= 0;
			bool surfelDisplaced = false;

			//for(int i = 0; i< surfelToPhyxelIterator->second.size(); i++){		

				//ProjectStructs::PHYXEL_NODE* phyxel = surfelToPhyxelIterator->second[i];
		/*	for(int i = 0; i<ImpactList::GetAffectedPhyxels().size(); i++){
				ProjectStructs::PHYXEL_NODE* phyxel = ImpactList::GetAffectedPhyxels()[i];
*/
			std::map<ProjectStructs::PHYXEL_NODE*, bool> affectedPhyxels = ImpactList::GetAffectedPhyxels();

			std::map<ProjectStructs::PHYXEL_NODE*, bool>::iterator phyxelIterator = affectedPhyxels.begin();
			for( ; phyxelIterator!= affectedPhyxels.end(); phyxelIterator++){

				ProjectStructs::PHYXEL_NODE* phyxel = phyxelIterator->first;
				float w = FractureManager::CalculateWeight(impactedSurfel->initialPosition + pos, phyxel->pos, phyxel->supportRadius);
		/*	for(int i = 0; i<phyxels.size(); i++){
				ProjectStructs::PHYXEL_NODE* phyxel = phyxels[i];
				float w = Volume::weights[phyxel];//FractureManager::CalculateWeight(impactedSurfel->initialPosition+ pos, phyxel->pos, phyxel->supportRadius);
*/

				if(w != 0.0f && phyxel->totalDisplacement != MathHelper::GetZeroVector())
				{
					D3DXVECTOR3 displacement = w * (phyxel->displacement + phyxel->totalDisplacement);
					if(D3DXVec3Length(&displacement) >= 0.1f){
						impactedSurfel->displacement += displacement;
						impactedSurfel->displacementCount++;
						surfelDisplaced = true;
					}
				}
			}

			if(surfelDisplaced){
				SurfelsToResample::AddExistingSurfel(impactedSurfel);

				impactedSurfel->vertex->pos = impactedSurfel->initialPosition + impactedSurfel->displacement;
			}
		}

//		DebugToFile::EndTimer("Add phyxel displacement to surfels");
	}

	StepResampleAlgorithm();
	ImpactList::Emptylist();
	
	DebugToFile::EndTimer("CRASH!!!");

	PhysicsWrapper::ULW();


	DebugToFile::CloseFile();


/*
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
				impactedSurfel->displacedPhyxels.push_back(phyxel);

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
					surfel->displacedPhyxels.push_back(phyxel);
					SurfelsToResample::AddExistingSurfel(surfel);

					surfel->displacement += displacement;
					surfel->displacementCount++;
				}
			}

			phyxel->isChanged = false;
		}
		DebugToFile::EndTimer("Moved the surfels");

		if(SurfelsToResample::GetExistingSurfelCount() != 0){

			DebugToFile::StartTimer();
			DebugToFile::EndTimer("Reset surfels");

			std::vector<ProjectStructs::SURFEL*> surfels;

		//	PhysicsWrapper::LockWorld();

				DebugToFile::StartTimer();
				SurfelsToResample::ResetGrid();
				DebugToFile::EndTimer("Reset grid");

				DebugToFile::StartTimer();
				for(int i = 0; i < SurfelsToResample::GetExistingSurfelCount(); i++){
					//DebugToFile::StartTimer();
					
					if(D3DXVec3Length(&SurfelsToResample::GetExistingSurfel(i)->vertex->majorAxis) * D3DXVec3Length(&SurfelsToResample::GetExistingSurfel(i)->vertex->minorAxis) > materialProperties.minimunSurfelSize)
						Algorithms::RefineSurfel(SurfelsToResample::GetExistingSurfel(i));

					//DebugToFile::EndTimer("Refined one surfel");
				}
				DebugToFile::EndTimer("Refined all surfels");

				DebugToFile::StartTimer();
				SurfelsToResample::CalculateNeighbors();
				DebugToFile::EndTimer("Calculated neighbors");

				DebugToFile::StartTimer();
				SurfelsToResample::Resample();	
				DebugToFile::EndTimer("Resampled surfels");

				DebugToFile::StartTimer();
				for(int i = 0; i < SurfelsToResample::GetNewSurfelSize(); i++){
					if(!vertexBufferGrid->PopulateNode(SurfelsToResample::GetNewSurfel(i))){
						SurfelsToResample::DeleteNewSurfel(i);
						i--;
					}
					else{
						
						vertexBufferGrid->ResetSurfel(SurfelsToResample::GetNewSurfel(i));
					}
				}

				for(int i = 0; i < SurfelsToResample::GetExistingSurfelCount(); i++){				
					vertexBufferGrid->ResetSurfel(SurfelsToResample::GetExistingSurfel(i));
				}

				DebugToFile::EndTimer("populated node of vertexbuffergrid");

				DebugToFile::StartTimer();
				vertexBufferGrid->Update();
				DebugToFile::EndTimer("Updated vertexbuffergrid");


				for(int i = 0; i < SurfelsToResample::GetNewSurfelSize(); i++){
					MathHelper::DisplaceSurfel(SurfelsToResample::GetNewSurfel(i), this->pos);
				}

				for(int i = 0; i < SurfelsToResample::GetExistingSurfelCount(); i++){				
					MathHelper::DisplaceSurfel(SurfelsToResample::GetExistingSurfel(i), this->pos);
				}

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
	*/
}

/*
void Volume::AddSurface(Surface *surface){
	surface->SetWorld(World);X
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
	DebugToFile::Debug("Surfel size %d", newSurfels.size());

	vertexBufferGrid->CleanUp();
	delete vertexBufferGrid;

	if(materialProperties.deformable){
		phyxelGrid->CleanUp();
		delete phyxelGrid;

		ImpactList::CleanUp();
	}
}

void Volume::StepResampleAlgorithm()
{
//	SurfelsToResample::SortSurfels();

	// get batch to update 
/*	SurfelsToResample::ResetBatch();

	DebugToFile::StartTimer();
	SurfelsToResample::ResampleBatch();
	DebugToFile::EndTimer("Resample surfels");

	DebugToFile::StartTimer();
	for(int i = 0; i<SurfelsToResample::GetSurfelBatch().size(); i++){
		vertexBufferGrid->PopulateNode(SurfelsToResample::GetSurfelBatch()[i]);
	}
	DebugToFile::EndTimer("Populate vertex buffer grid nodes");

	DebugToFile::StartTimer();
	vertexBufferGrid->ResetSurfels(SurfelsToResample::GetSurfelBatch());
	DebugToFile::EndTimer("Reset existing surfels in vertex bugger grid");

	DebugToFile::StartTimer();
	SurfelsToResample::CalculateBatchNeighbors();
	DebugToFile::EndTimer("Calculate neighbors for existing surfels");

	DebugToFile::StartTimer();
	for(int i = 0; i < SurfelsToResample::GetSurfelBatch().size(); i++){
		MathHelper::DisplaceSurfel(SurfelsToResample::GetSurfelBatch()[i], this->pos);
	}
	DebugToFile::EndTimer("Displaced surfels");

	DebugToFile::StartTimer();
	SurfelsToResample::ResetGridBatch();
	DebugToFile::EndTimer("Reset grid");

	DebugToFile::StartTimer();
	vertexBufferGrid->ResetSurfels(SurfelsToResample::GetSurfelBatch());
	DebugToFile::EndTimer("Reset surfels");
*/

	SurfelsToResample::Resample();

	std::map<ProjectStructs::Vertex_Grid_Cell*, std::vector<ProjectStructs::SURFEL*>> cellToSurfel;

	for(int i = 0; i<SurfelsToResample::GetExistingSurfelCount(); i++){
	
		ProjectStructs::SURFEL* surfel = SurfelsToResample::GetExistingSurfel(i);
		Algorithms::CalculateNeighbors(surfel, ImpactList::GetAffectedSurfelVector());
		MathHelper::DisplaceSurfel(surfel, this->pos);

		//SurfelsToResample::ResetGrid(surfel);
		vertexBufferGrid->ResetSurfel(surfel);
		cellToSurfel[surfel->vertexGridCell].push_back(surfel);
	}
	
	for(int i = 0; i<SurfelsToResample::GetNewSurfelSize(); i++){
		ProjectStructs::SURFEL* surfel = SurfelsToResample::GetNewSurfel(i);
		vertexBufferGrid->PopulateNode(surfel);
		Algorithms::CalculateNeighbors(surfel, ImpactList::GetAffectedSurfelVector());
		MathHelper::DisplaceSurfel(surfel, this->pos);

		vertexBufferGrid->ResetSurfel(surfel);
		cellToSurfel[surfel->vertexGridCell].push_back(surfel);
	}

	std::map<ProjectStructs::Vertex_Grid_Cell*, std::vector<ProjectStructs::SURFEL*>>::iterator cellToSurfelIterator;

	for(cellToSurfelIterator = cellToSurfel.begin(); cellToSurfelIterator != cellToSurfel.end(); cellToSurfelIterator++){
		vertexBufferGrid->ResetCell(cellToSurfelIterator->first, cellToSurfelIterator->second);
	}
	
	SurfelsToResample::Clear();

	//////////////////////////////////////////////////////////
/*

	
	SurfelsToResample::Resample();
	
	for(int i = 0; i<SurfelsToResample::GetNewSurfelSize(); i++){
		vertexBufferGrid->PopulateNode(SurfelsToResample::GetNewSurfel(i));
	}

	vertexBufferGrid->ResetSurfels(SurfelsToResample::GetExistingSurfels());

	SurfelsToResample::CalculateNeighborsForExistingSurfels();

	vertexBufferGrid->ResetSurfels(SurfelsToResample::GetNewSurfels());

	SurfelsToResample::CalculateNeighborsForNewSurfels();

	for(int i = 0; i < SurfelsToResample::GetNewSurfelSize(); i++){
		MathHelper::DisplaceSurfel(SurfelsToResample::GetNewSurfel(i), this->pos);
	}

	for(int i = 0; i < SurfelsToResample::GetExistingSurfelCount(); i++){
		MathHelper::DisplaceSurfel(SurfelsToResample::GetExistingSurfel(i), this->pos);
	}

	SurfelsToResample::ResetGrid();

	SurfelsToResample::Clear();*/

}


