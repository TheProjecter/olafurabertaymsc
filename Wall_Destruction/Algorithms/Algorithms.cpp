#include "Algorithms.h"
#include "Structs.h"
#include "Globals.h"
#include "DebugToFile.h"
#include "MathHelper.h"
#include "DeletedStructHolder.h"
#include "FractureManager.h"
#include "PointGrid.h"
#include "SurfelsToResample.h"

std::vector<PointGrid*> Algorithms::grids;
const float Algorithms::angleThreshold = Helpers::Globals::PI / 8.0f;

template<class T>
struct map_data_compare : public std::binary_function<typename T::value_type, 
	typename T::mapped_type, 
	bool>
{
public:
	bool operator() (typename T::value_type &pair, 
		typename T::mapped_type i) const
	{
		return pair.second == i;
	}
};

std::vector<ProjectStructs::SURFEL*> Algorithms::Splat(std::vector<ProjectStructs::SURFEL*> surfelsWithoutMinorMajorAndNormals, float maxRadiusLength, D3DXVECTOR3 generalNormal, D3DXVECTOR3 volumePos){

	D3DXVECTOR3 MIN = D3DXVECTOR3(FLT_MAX, FLT_MAX, FLT_MAX);
	D3DXVECTOR3 MAX = D3DXVECTOR3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	D3DXVECTOR3 POS = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	float count = 0;

	for(unsigned int i = 0; i < surfelsWithoutMinorMajorAndNormals.size(); i++){
		POS += surfelsWithoutMinorMajorAndNormals[i]->vertex->pos;
		if(MIN.x > surfelsWithoutMinorMajorAndNormals[i]->vertex->pos.x)
			MIN.x = surfelsWithoutMinorMajorAndNormals[i]->vertex->pos.x;
		if(MAX.x < surfelsWithoutMinorMajorAndNormals[i]->vertex->pos.x)
			MAX.x = surfelsWithoutMinorMajorAndNormals[i]->vertex->pos.x;

		if(MIN.y > surfelsWithoutMinorMajorAndNormals[i]->vertex->pos.y)
			MIN.y = surfelsWithoutMinorMajorAndNormals[i]->vertex->pos.y;
		if(MAX.y < surfelsWithoutMinorMajorAndNormals[i]->vertex->pos.y)
			MAX.y = surfelsWithoutMinorMajorAndNormals[i]->vertex->pos.y;

		if(MIN.z > surfelsWithoutMinorMajorAndNormals[i]->vertex->pos.z)
			MIN.z = surfelsWithoutMinorMajorAndNormals[i]->vertex->pos.z;
		if(MAX.z < surfelsWithoutMinorMajorAndNormals[i]->vertex->pos.z)
			MAX.z = surfelsWithoutMinorMajorAndNormals[i]->vertex->pos.z;

		count++;
	}

	// should not happen
	if(count == 0)
		return surfelsWithoutMinorMajorAndNormals;

	POS /= count;

	// add the points to an grid
	PointGrid* grid = new PointGrid(MIN, MAX, POS + volumePos, 3.0f, maxRadiusLength);
	grid->Init();
	surfelsWithoutMinorMajorAndNormals = grid->InsertPoints(surfelsWithoutMinorMajorAndNormals);

	// the vector should be resampled...

	for(unsigned int i = 0; i<surfelsWithoutMinorMajorAndNormals.size(); i++){

		D3DXVECTOR3 index = grid->GetIndexOfPosition(surfelsWithoutMinorMajorAndNormals[i]->vertex->pos);

		if(!grid->GetCells().ValidIndex(index)){
			delete surfelsWithoutMinorMajorAndNormals[i];

			surfelsWithoutMinorMajorAndNormals.erase(surfelsWithoutMinorMajorAndNormals.begin() + i);

			i--;

			continue;
		}

		ProjectStructs::Point_Grid_Cell* cell = grid->GetCell(index);

		float majorAxisDistance = FLT_MAX;
		float minorAxisDistance = FLT_MAX;
		float minorAxisAngle = FLT_MAX;

		int majorAxisIndex = -1;
		int majorAxisSurfelIndex = -1;
		int minorAxisIndex = -1;
		int minorAxisSurfelIndex = -1;
		D3DXVECTOR3 majorDirection, minorDirection;

		for(unsigned int j = 0; j<cell->neighbors.GetSize(); j++){
			if(!cell->neighbors[j] || j == 13)
				continue;
			for(unsigned int k = 0; k<cell->neighbors[j]->surfels.size(); k++){
				float tmpDistance = D3DXVec3Length(&(cell->neighbors[j]->surfels[k]->vertex->pos - surfelsWithoutMinorMajorAndNormals[i]->vertex->pos));
				// we don't want the distance to be 0
				if(tmpDistance < majorAxisDistance && tmpDistance > 0.1f){
					majorAxisDistance = tmpDistance;
					majorAxisIndex = j;
					majorAxisSurfelIndex = k;
					D3DXVec3Normalize(&majorDirection, &(cell->neighbors[j]->surfels[k]->vertex->pos - surfelsWithoutMinorMajorAndNormals[i]->vertex->pos));
					majorDirection.x = abs(floor(majorDirection.x + 0.5f));
					majorDirection.y = abs(floor(majorDirection.y + 0.5f));
					majorDirection.z = abs(floor(majorDirection.z + 0.5f));
				}
			}			
		}

		for(unsigned int j = 0; j<cell->neighbors.GetSize(); j++){
			if(!cell->neighbors[j] || j == 13)
				continue;

			for(unsigned int k = 0; k<cell->neighbors[j]->surfels.size(); k++){
				if(j == majorAxisIndex && k == majorAxisSurfelIndex)
					continue;

				float tmpDistance = D3DXVec3Length(&(cell->neighbors[j]->surfels[k]->vertex->pos - surfelsWithoutMinorMajorAndNormals[i]->vertex->pos));
				float angle = abs(Helpers::Globals::HALF_PI - abs(MathHelper::GetAngleBetweenVectors(surfelsWithoutMinorMajorAndNormals[i]->vertex->pos - cell->neighbors[j]->surfels[k]->vertex->pos, 
					surfelsWithoutMinorMajorAndNormals[i]->vertex->pos - cell->neighbors[majorAxisIndex]->surfels[majorAxisSurfelIndex]->vertex->pos, true)));
				D3DXVec3Normalize(&minorDirection, &(cell->neighbors[j]->surfels[k]->vertex->pos - surfelsWithoutMinorMajorAndNormals[i]->vertex->pos));
				
				minorDirection.x = abs(floor(minorDirection.x + 0.5f));
				minorDirection.y = abs(floor(minorDirection.y + 0.5f));
				minorDirection.z = abs(floor(minorDirection.z + 0.5f));

				// we don't want the distance to be 0
				if(angle < minorAxisAngle && tmpDistance > 0.1f && minorDirection != majorDirection){
					minorAxisDistance = tmpDistance;
					minorAxisIndex = j;
					minorAxisAngle = angle;
					minorAxisSurfelIndex = k;
				}
			}			
		}

		if(minorAxisIndex == -1 || majorAxisIndex == -1){
			surfelsWithoutMinorMajorAndNormals.erase(surfelsWithoutMinorMajorAndNormals.begin() + i);
			i--;
			continue;
		}

		D3DXVECTOR3 direction = surfelsWithoutMinorMajorAndNormals[i]->vertex->pos - cell->neighbors[majorAxisIndex]->surfels[majorAxisSurfelIndex]->vertex->pos;
		D3DXVec3Normalize(&direction, &direction);
		surfelsWithoutMinorMajorAndNormals[i]->vertex->majorAxis= direction * majorAxisDistance;

		direction = surfelsWithoutMinorMajorAndNormals[i]->vertex->pos - cell->neighbors[minorAxisIndex]->surfels[minorAxisSurfelIndex]->vertex->pos;
		D3DXVec3Normalize(&direction, &direction);
		surfelsWithoutMinorMajorAndNormals[i]->vertex->minorAxis = direction * minorAxisDistance;

		D3DXVec3Cross(&surfelsWithoutMinorMajorAndNormals[i]->vertex->normal, &surfelsWithoutMinorMajorAndNormals[i]->vertex->majorAxis, &surfelsWithoutMinorMajorAndNormals[i]->vertex->minorAxis);
		D3DXVec3Normalize(&surfelsWithoutMinorMajorAndNormals[i]->vertex->normal, &surfelsWithoutMinorMajorAndNormals[i]->vertex->normal);

		if(MathHelper::Facing(surfelsWithoutMinorMajorAndNormals[i]->vertex->normal, generalNormal)){
			direction = surfelsWithoutMinorMajorAndNormals[i]->vertex->pos - cell->neighbors[minorAxisIndex]->surfels[minorAxisSurfelIndex]->vertex->pos;
			D3DXVec3Normalize(&direction, &direction);
			surfelsWithoutMinorMajorAndNormals[i]->vertex->minorAxis = direction * -1.0f * minorAxisDistance;

			D3DXVec3Cross(&surfelsWithoutMinorMajorAndNormals[i]->vertex->normal, &surfelsWithoutMinorMajorAndNormals[i]->vertex->majorAxis, &surfelsWithoutMinorMajorAndNormals[i]->vertex->minorAxis);
			D3DXVec3Normalize(&surfelsWithoutMinorMajorAndNormals[i]->vertex->normal, &surfelsWithoutMinorMajorAndNormals[i]->vertex->normal);
		}

		DebugToFile::Debug("Created a new surfel: pos(%.3f, %.3f, %.3f) minor(%.3f, %.3f, %.3f) major(%.3f, %.3f, %.3f) normal(%.3f, %.3f, %.3f)", 
			surfelsWithoutMinorMajorAndNormals[i]->vertex->pos.x, surfelsWithoutMinorMajorAndNormals[i]->vertex->pos.y, surfelsWithoutMinorMajorAndNormals[i]->vertex->pos.z,
			surfelsWithoutMinorMajorAndNormals[i]->vertex->minorAxis.x, surfelsWithoutMinorMajorAndNormals[i]->vertex->minorAxis.y, surfelsWithoutMinorMajorAndNormals[i]->vertex->minorAxis.z,
			surfelsWithoutMinorMajorAndNormals[i]->vertex->majorAxis.x, surfelsWithoutMinorMajorAndNormals[i]->vertex->majorAxis.y, surfelsWithoutMinorMajorAndNormals[i]->vertex->majorAxis.z,
			surfelsWithoutMinorMajorAndNormals[i]->vertex->normal.x, surfelsWithoutMinorMajorAndNormals[i]->vertex->normal.y, surfelsWithoutMinorMajorAndNormals[i]->vertex->normal.z);		
	}

	grids.push_back(grid);

	return surfelsWithoutMinorMajorAndNormals;
}

std::vector<ProjectStructs::SURFEL*> Algorithms::ResampleSurfel( ProjectStructs::SURFEL* surfel, ProjectStructs::IMPACT* impact, D3DXMATRIX surfaceWorldMatrix)
{
	std::vector<ProjectStructs::SURFEL*> surfels;

	if(DeletedStructHolder::ContainsSurfel(surfel))
		return surfels;

	D3DXVECTOR3 major = surfel->vertex->majorAxis / 3.0f;
	D3DXVECTOR3 minor = surfel->vertex->minorAxis / 3.0f;
	D3DXVECTOR3 pos = surfel->vertex->pos;
	D3DXVECTOR2 newDelta = surfel->vertex->deltaUV / 3.0f;
	D3DXVECTOR3 newSurfelPos;
	D3DXVECTOR3 displacement;
	D3DXVECTOR3 worldTransform = D3DXVECTOR3(surfaceWorldMatrix._41, surfaceWorldMatrix._42, surfaceWorldMatrix._43);

	for(float k = -1.0f; k < 2.0f; k+=1.0f){
		for(float l = -1.0f; l < 2.0f; l+=1.0f){
			newSurfelPos = pos + k * major + l * minor;

			displacement = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			for(unsigned int i = 0; i < surfel->intersectingCells.size(); i++){
				ProjectStructs::PHYXEL_NODE* phyxel = surfel->intersectingCells[i]->phyxel;

				float w = FractureManager::CalculateWeight(newSurfelPos + worldTransform, phyxel->pos, phyxel->supportRadius);
				displacement += (phyxel->displacement) * w;
			}

			ProjectStructs::SURFEL *newSurfel = ProjectStructs::StructHelper::CreateSurfelPointer(
				newSurfelPos + displacement,
				MathHelper::GetZeroVector(), 
				MathHelper::GetZeroVector(), 
				MathHelper::GetZeroVector(), 
			//	major, minor, surfel->vertex->normal,
				surfel->vertex->UV + D3DXVECTOR2(k * newDelta.x, l * newDelta.y),
				newDelta
			);

			surfels.push_back(newSurfel);
		}
	}

	return surfels;
}

// algorithm taken from Dynamic surfel set refinement for high-quality rendering
void Algorithms::CalculateNeighbors(ProjectStructs::SURFEL* surfel){
	float firstAngle = 0.0f;

	std::map<unsigned int, std::vector<unsigned int>> checkedSurfels; // [cellIndex , vector of surfel indices]

	D3DXVECTOR3 firstPos;

	DebugToFile::StartTimer();
	for(unsigned int i = 0; i<surfel->intersectingCells.size(); i++){
		ProjectStructs::Phyxel_Grid_Cell* cell = surfel->intersectingCells[i];

		checkedSurfels[i] = std::vector<unsigned int>();

		// add all neighboring surfels into the map
		for(unsigned int j = 0; j < cell->surfels.size(); j++){

			if(surfel == cell->surfels[j] /*|| !MathHelper::Intersection(surfel, cell->surfels[j], 0.75f)*/ || !MathHelper::SameDirection(surfel->vertex->normal, cell->surfels[j]->vertex->normal))
				continue;

			if(surfel->neighbors.size() == 0 ){
				firstPos = cell->surfels[j]->vertex->pos;
				surfel->neighbors[firstAngle] = cell->surfels[j];
				surfel->inverseNeighbors[cell->surfels[j]] = firstAngle;
				cell->surfels[j]->isChecked= true;
				checkedSurfels[i].push_back(j);
				continue;
			}
			else if(!cell->surfels[j]->isChecked){
				// check the angle between this surfel neighbor and the last surfel neighbor
				float angle = MathHelper::Get3DAngle(cell->surfels[j]->vertex->pos - surfel->vertex->pos, firstPos - surfel->vertex->pos, surfel->vertex->normal);
				//float angle = MathHelper::Get3DAngle(surfel, cell->surfels[j]->vertex->pos, firstPos);
			
				if(surfel->neighbors.find(angle) != surfel->neighbors.end())
				{
					float previousLength = D3DXVec3Length(&(surfel->neighbors[angle]->vertex->pos - surfel->vertex->pos));
					float currentLength = D3DXVec3Length(&(cell->surfels[j]->vertex->pos - surfel->vertex->pos));

					if(currentLength < previousLength){
						surfel->neighbors[angle]->isChecked = false;
						surfel->inverseNeighbors.erase(surfel->neighbors[angle]);
						surfel->neighbors[angle] = cell->surfels[j];
						surfel->inverseNeighbors[cell->surfels[j]] = angle;
						cell->surfels[j]->isChecked= true;
						checkedSurfels[i].push_back(j);
					}
				}
				else{
					if(surfel->inverseNeighbors.find(cell->surfels[j]) == surfel->inverseNeighbors.end()){
						surfel->neighbors[angle] = cell->surfels[j];
						surfel->inverseNeighbors[cell->surfels[j]] = angle;
						cell->surfels[j]->isChecked= true;
						checkedSurfels[i].push_back(j);
					}
				}
			}
		}		
	}
	DebugToFile::EndTimer("Neighbors - First loop");

	DebugToFile::StartTimer();
	DecreaseNeighbors(surfel);
	DebugToFile::EndTimer("Neighbors - Decrease neighbors");

	std::map<unsigned int, std::vector<unsigned int>>::const_iterator checkedSurfelsIterator;
	for(checkedSurfelsIterator = checkedSurfels.begin(); checkedSurfelsIterator != checkedSurfels.end(); checkedSurfelsIterator++){
		for(unsigned int i = 0; i<checkedSurfelsIterator->second.size(); i++){
			surfel->intersectingCells[checkedSurfelsIterator->first]->surfels[i]->isChecked = false;
		}
	}
}

// algorithm taken from Dynamic surfel set refinement for high-quality rendering
void Algorithms::RefineSurfel(ProjectStructs::SURFEL* surfel){	
	if(surfel->vertexGridCell->changed == false && surfel->vertexGridCell->neighborCount != 0 && surfel->vertexGridCell->surfels.size() == 0){	
		ReleaseCOM(surfel->vertexGridCell->neighborVertexBuffer);	
	}

	surfel->vertexGridCell->neighborCount = 0;
	surfel->vertexGridCell->changed = true;
	
	std::map<float, ProjectStructs::SURFEL*>::iterator neighborIterator;
	std::map<float, ProjectStructs::SURFEL*>::iterator nextNeighborIterator;
	std::map<float, ProjectStructs::SURFEL*>::iterator neighborNeighborIterator;
	std::vector<ProjectStructs::SURFEL*> surfelsUsed;
	std::vector<ProjectStructs::SURFEL*> surfelsInPolygon;
	// add the parent surfel to the used surfel list and to the surfels in polygon list
	surfelsUsed.push_back(surfel);
	surfelsInPolygon.push_back(surfel);

	// create a set of "neighborhood polygon"'s
	std::vector<std::vector<ProjectStructs::SURFEL*>> neighborhoodPolygons;
	int polygonCount = 0;

	int count = 0;
	// add the neighbours of the surfels into the surfelsInPolygon list
	for(neighborIterator = surfel->neighbors.begin(); neighborIterator != surfel->neighbors.end(); neighborIterator++){
		surfelsInPolygon.push_back(neighborIterator->second);
	}

	// go through all the surfels that are a part of this polygon
	for(UINT i = 1; i < surfelsInPolygon.size(); i++){
		ProjectStructs::SURFEL* polygonSurfel = surfelsInPolygon[i];
		bool validSurfel = find(surfelsUsed.begin(), surfelsUsed.end(), polygonSurfel) == surfelsUsed.end();

		if(!validSurfel)
			continue;

		neighborhoodPolygons.push_back(std::vector<ProjectStructs::SURFEL*>());
		neighborhoodPolygons[polygonCount].push_back(surfel);
		neighborhoodPolygons[polygonCount].push_back(polygonSurfel);
		surfelsUsed.push_back(polygonSurfel);

		ProjectStructs::SURFEL* lastSurfel = polygonSurfel;
		while(lastSurfel != NULL){
			lastSurfel = NULL;

			for(UINT k = 0; k<surfelsInPolygon.size() && lastSurfel == NULL; k++){
				bool connectedToAllPolygons = true;
				if(surfelsInPolygon[k] == surfel || find(neighborhoodPolygons[polygonCount].begin(), neighborhoodPolygons[polygonCount].end(), surfelsInPolygon[k]) != neighborhoodPolygons[polygonCount].end())
					continue;

				for(UINT j = 0; j<neighborhoodPolygons[polygonCount].size() && connectedToAllPolygons; j++){
					// get the first surfel that is connected to all the polygon and is not already a part of the polygon
					connectedToAllPolygons = neighborhoodPolygons[polygonCount][j]->inverseNeighbors.find(surfelsInPolygon[k]) != neighborhoodPolygons[polygonCount][j]->inverseNeighbors.end();
				}

				if(connectedToAllPolygons){
					lastSurfel = surfelsInPolygon[k];
					neighborhoodPolygons[polygonCount].push_back(surfelsInPolygon[k]);
				}
			}		
		}
		polygonCount++;
	}

	for(int i = 0; i<neighborhoodPolygons.size(); i++){
		for(int j = 0; j<neighborhoodPolygons[i].size(); j++){
			neighborIterator = neighborhoodPolygons[i][j]->neighbors.begin();

			SurfelsToResample::AddSurfelToRecheckNeighbors(neighborhoodPolygons[i][j]);

			for( ; neighborIterator != neighborhoodPolygons[i][j]->neighbors.end(); neighborIterator++){
				SurfelsToResample::AddSurfelToRecheckNeighbors(neighborIterator->second);
			}
		}
	}

	// ok, now the polygons have been created and now surfels have to be inserted
	for(UINT i = 0; i< neighborhoodPolygons.size(); i++){
		
		float level = (float)neighborhoodPolygons[i].size();
		
		ProjectStructs::SURFEL* s = ProjectStructs::StructHelper::CreateSurfelPointer(MathHelper::GetZeroVector(), MathHelper::GetZeroVector(), MathHelper::GetZeroVector(),
			MathHelper::GetZeroVector(), D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR2(0.0f, 0.0f));

		for(UINT j = 0; j<level; j++){
			s->vertex->pos += neighborhoodPolygons[i][j]->vertex->pos;
			s->vertex->majorAxis += neighborhoodPolygons[i][j]->vertex->majorAxis;
			s->vertex->minorAxis += neighborhoodPolygons[i][j]->vertex->minorAxis;
			s->vertex->normal += neighborhoodPolygons[i][j]->vertex->normal;
			s->lastDisplacement += neighborhoodPolygons[i][j]->lastDisplacement;
			s->vertex->UV += neighborhoodPolygons[i][j]->vertex->UV ;//* 2.0f;
			s->vertex->deltaUV += neighborhoodPolygons[i][j]->vertex->deltaUV;
		}
		
		s->vertex->pos /= level;
		s->vertex->deltaUV /= level * 2.0f;
		s->vertex->majorAxis /= level * 2.0f;
		s->vertex->minorAxis /= level * 2.0f;
		s->vertex->normal /= level;
		s->vertex->UV /= level;
		s->lastDisplacement /= level;
		s->displacement = MathHelper::GetZeroVector();
		s->vertex->frontFacing = surfel->vertex->frontFacing;

		if(SurfelsToResample::ContainsNewSurfel(s))
		{
			delete s;
			s = NULL;
		}
		else{
			SurfelsToResample::AddNewSurfel(s);	
		}

		for(UINT j = 0; j<level-1; j++){

			ProjectStructs::SURFEL* newSurfel = ProjectStructs::StructHelper::CreateSurfelPointer(MathHelper::GetZeroVector(), MathHelper::GetZeroVector(), MathHelper::GetZeroVector(),
				MathHelper::GetZeroVector(), D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR2(0.0f, 0.0f));

			newSurfel->vertex->pos = (neighborhoodPolygons[i][j]->vertex->pos + neighborhoodPolygons[i][j+1]->vertex->pos) * 0.5f;
			newSurfel->vertex->normal = (neighborhoodPolygons[i][j]->vertex->normal + neighborhoodPolygons[i][j+1]->vertex->normal) * 0.5f;
			newSurfel->lastDisplacement = (neighborhoodPolygons[i][j]->lastDisplacement + neighborhoodPolygons[i][j+1]->lastDisplacement) * 0.5f;

			newSurfel->vertex->UV = (neighborhoodPolygons[i][j]->vertex->UV + neighborhoodPolygons[i][j+1]->vertex->UV) * 0.5f ;
			newSurfel->vertex->deltaUV += (neighborhoodPolygons[i][j]->vertex->deltaUV + neighborhoodPolygons[i][j+1]->vertex->deltaUV) * 0.25f;
			newSurfel->vertex->majorAxis = (neighborhoodPolygons[i][j]->vertex->majorAxis + neighborhoodPolygons[i][j+1]->vertex->majorAxis) * 0.25f;
			newSurfel->vertex->minorAxis = (neighborhoodPolygons[i][j]->vertex->minorAxis + neighborhoodPolygons[i][j+1]->vertex->minorAxis) * 0.25f;
			
			newSurfel->vertex->clipPlane = MathHelper::GetClipPlane(neighborhoodPolygons[i][j]->vertex->clipPlane, neighborhoodPolygons[i][j+1]->vertex->clipPlane);
			
			newSurfel->vertex->frontFacing = surfel->vertex->frontFacing;
			newSurfel->displacement = MathHelper::GetZeroVector();

			if(SurfelsToResample::ContainsNewSurfel(newSurfel))
			{
				delete newSurfel;
				newSurfel = NULL;
			}
			else{
				SurfelsToResample::AddNewSurfel(newSurfel);	
			}
		}		
	}
}

void Algorithms::Draw(){
	for(unsigned int i = 0; i<grids.size(); i++){
		grids[i]->Draw();
	}
}

void Algorithms::CleanUp(){
	for(unsigned int i = 0; i<grids.size(); i++){
		grids[i]->CleanUp();
	}
	grids.clear();
}

void Algorithms::DecreaseNeighbors( ProjectStructs::SURFEL* surfel )
{
	std::map<float, ProjectStructs::SURFEL*>::iterator neighborIterator;

	std::vector<float> angles;
	std::vector<ProjectStructs::SURFEL*> surfels;

	int lastIndex, currentIndex, nextIndex;

	// now the neighbors are in ascending angle order
	// if two neighbors are to close to each other, take out the one further away
	//DebugToFile::Debug("Before deletion");
	for(neighborIterator = surfel->neighbors.begin(); neighborIterator != surfel->neighbors.end(); neighborIterator++){
		angles.push_back(neighborIterator->first);
		surfels.push_back(neighborIterator->second);
		neighborIterator->second->isChecked = false;
		
	//	DebugToFile::Debug("0x%x  %.3f", neighborIterator->second, neighborIterator->first);
	}

	bool exitSituationEncountered = surfels.size() == 0;

	for(currentIndex = 1; !exitSituationEncountered ; currentIndex++){
		lastIndex = currentIndex - 1;

		exitSituationEncountered = (currentIndex != (currentIndex % angles.size()));
		currentIndex = (currentIndex % angles.size());
		
		if(abs(angles[lastIndex] - angles[currentIndex]) < angleThreshold || abs(Helpers::Globals::TWO_PI - abs(angles[lastIndex] - angles[currentIndex])) < angleThreshold){
			if(D3DXVec3Length(&(surfels[lastIndex]->vertex->pos - surfel->vertex->pos)) < D3DXVec3Length(&(surfels[currentIndex]->vertex->pos - surfel->vertex->pos))){
				//DebugToFile::Debug("Deleting:");
				//DebugToFile::Debug("0x%x  %.3f", surfels[currentIndex], angles[currentIndex]);
				
				// delete the currentIndex
 				surfel->neighbors.erase(angles[currentIndex]);
				surfel->inverseNeighbors.erase(surfels[currentIndex]);
				angles.erase(angles.begin() + currentIndex);
				surfels.erase(surfels.begin() + currentIndex);
			}
			else{
				//DebugToFile::Debug("Deleting:");
				//DebugToFile::Debug("0x%x  %.3f", surfels[lastIndex], angles[lastIndex]);
				
				// delete the lastIndex
				surfel->neighbors.erase(angles[lastIndex]);
				surfel->inverseNeighbors.erase(surfels[lastIndex]);
				angles.erase(angles.begin() + lastIndex);
				surfels.erase(surfels.begin() + lastIndex);
			}

			currentIndex--;
		}
	}
/*
	for(neighborIterator = surfel->neighbors.begin(); neighborIterator != surfel->neighbors.end(); neighborIterator++){
		neighborIterator->second->newlySampledNeighbors[neighborIterator->first] = surfel;
	}
*/
	/*DebugToFile::Debug("After deletion");
	for(neighborIterator = surfel->neighbors.begin(); neighborIterator != surfel->neighbors.end(); neighborIterator++){
		DebugToFile::Debug("0x%x  %.3f", neighborIterator->second, neighborIterator->first);
	}*/
}