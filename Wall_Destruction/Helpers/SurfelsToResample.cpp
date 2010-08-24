#include "SurfelsToResample.h"
#include "Algorithms.h"
#include "MathHelper.h"
#include <algorithm>

std::vector<ProjectStructs::SURFEL*> SurfelsToResample::existingSurfels;

std::vector<ProjectStructs::SURFEL*> SurfelsToResample::surfelsToRecheckNeighbors;

std::vector<ProjectStructs::SURFEL*> SurfelsToResample::newSurfels;

std::vector<ProjectStructs::SURFEL*> SurfelsToResample::surfelsToResetGrid;

void SurfelsToResample::AddExistingSurfel(ProjectStructs::SURFEL* surfel){
	if(find(existingSurfels.begin(), existingSurfels.end(), surfel) == existingSurfels.end()){
		existingSurfels.push_back(surfel);
		AddSurfelToRecheckNeighbors(surfel);
		surfelsToResetGrid.push_back(surfel);
	}
}	

void SurfelsToResample::AddNewSurfel(ProjectStructs::SURFEL* surfel){
	//return;
	newSurfels.push_back(surfel);
	AddSurfelToRecheckNeighbors(surfel);	
}	

void SurfelsToResample::AddSurfelToRecheckNeighbors(ProjectStructs::SURFEL* surfel){
	if(find(surfelsToRecheckNeighbors.begin(), surfelsToRecheckNeighbors.end(), surfel) == surfelsToRecheckNeighbors.end())
		surfelsToRecheckNeighbors.push_back(surfel);
}

void SurfelsToResample::Resample(){
	for(int i = 0; i<existingSurfels.size(); i++){
		ProjectStructs::SURFEL* surfel = existingSurfels[i];
		surfel->vertex->deltaUV *= 0.5f;
		surfel->vertex->minorAxis *= 0.5f;
		surfel->vertex->majorAxis *= 0.5f;
	}
}

void SurfelsToResample::ResetGrid(){
	for(int i = 0; i < surfelsToResetGrid.size(); i++){
		if(surfelsToResetGrid[i] == NULL)
			continue;

		for(unsigned int j = 0; j < surfelsToResetGrid[i]->intersectingCells.size(); j++){
			std::vector<ProjectStructs::SURFEL*>::iterator index = find(surfelsToResetGrid[i]->intersectingCells[j]->surfels.begin(),
				surfelsToResetGrid[i]->intersectingCells[j]->surfels.end(), surfelsToResetGrid[i]);		
			if(index  != surfelsToResetGrid[i]->intersectingCells[j]->surfels.end()){
				surfelsToResetGrid[i]->intersectingCells[j]->surfels.erase(index);
			}
		}
	}
	surfelsToResetGrid.clear();
}

void SurfelsToResample::CalculateNeighbors(){

	std::vector<ProjectStructs::SURFEL*> newSurfelsToRecheckNeighbors;
	std::map<float, ProjectStructs::SURFEL*>::iterator surfelIterator;
	for(unsigned int i = 0; i<surfelsToRecheckNeighbors.size(); i++){
		for(surfelIterator = surfelsToRecheckNeighbors[i]->neighbors.begin(); surfelIterator != surfelsToRecheckNeighbors[i]->neighbors.end(); surfelIterator++){
			newSurfelsToRecheckNeighbors.push_back(surfelIterator->second);
		}
	}

	for(unsigned int i = 0; i<newSurfelsToRecheckNeighbors.size(); i++){
		AddSurfelToRecheckNeighbors(newSurfelsToRecheckNeighbors[i]);
	}

	for(unsigned int i = 0; i<surfelsToRecheckNeighbors.size(); i++){
		surfelsToRecheckNeighbors[i]->neighbors.clear();
		surfelsToRecheckNeighbors[i]->inverseNeighbors.clear();

		Algorithms::CalculateNeighbors(surfelsToRecheckNeighbors[i]);
		surfelsToRecheckNeighbors[i]->vertexGridCell->neighborCount = 0;
	}

	// reset the normal vector and major and minor axis
/*
	for(unsigned int i = 0; i<surfelsToRecheckNeighbors.size(); i++){
		std::map<float, ProjectStructs::SURFEL*>::iterator neighborIterator;
		D3DXVECTOR3 newMajor = D3DXVECTOR3(0.0f, 0.0f, 0.0f), newMinor= D3DXVECTOR3(0.0f, 0.0f, 0.0f), newNormal= D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		float closestAngle = FLT_MAX;
		float closestLength = FLT_MAX, nextClosestLength = FLT_MAX, tmp = FLT_MAX;
		D3DXPLANE surfelPlane;
		D3DXPlaneFromPointNormal(&surfelPlane, &surfelsToRecheckNeighbors[i]->vertex->pos, &surfelsToRecheckNeighbors[i]->vertex->normal);

		for(neighborIterator = surfelsToRecheckNeighbors[i]->neighbors.begin(); neighborIterator != surfelsToRecheckNeighbors[i]->neighbors.end(); neighborIterator++){
			
			bool isPointOnPlane = (surfelPlane.a * neighborIterator->second->vertex->pos.x + surfelPlane.b * neighborIterator->second->vertex->pos.y + surfelPlane.c * neighborIterator->second->vertex->pos.z - surfelPlane.d == 0.0f);

			// only want to rotate the surfel if the neighbors are not in the plane
			if(!isPointOnPlane){
				tmp = D3DXVec3Length(&(surfelsToRecheckNeighbors[i]->vertex->pos - neighborIterator->second->vertex->pos));

				if(tmp <= closestLength && (closestAngle == FLT_MAX || (neighborIterator->first - closestAngle) >= Helpers::Globals::HALF_PI && (neighborIterator->first - closestAngle) <= Helpers::Globals::PI ))	{
					closestLength = tmp;
					newMinor = newMajor;
					newMajor = surfelsToRecheckNeighbors[i]->vertex->pos - neighborIterator->second->vertex->pos;
					closestAngle = neighborIterator->first;
				}
				else if(tmp <= nextClosestLength && (closestAngle == FLT_MAX || (neighborIterator->first - closestAngle) >= Helpers::Globals::HALF_PI && (neighborIterator->first - closestAngle) <= Helpers::Globals::PI )){
					nextClosestLength = tmp;
					newMinor = surfelsToRecheckNeighbors[i]->vertex->pos - neighborIterator->second->vertex->pos;
				}
			}
		}	

		if(closestAngle != FLT_MAX){
			// make the axis be positive
			newMajor.x = abs(newMajor.x);
			newMajor.y = abs(newMajor.y);
			newMajor.z = abs(newMajor.z);

			newMinor.x = abs(newMinor.x);
			newMinor.y = abs(newMinor.y);
			newMinor.z = abs(newMinor.z);
			
			surfelsToRecheckNeighbors[i]->vertex->majorAxis = newMinor;
			surfelsToRecheckNeighbors[i]->vertex->minorAxis = newMajor;

			D3DXVec3Cross(&newNormal, &newMajor, &newMinor);
			if(surfelsToRecheckNeighbors[i]->vertex->frontFacing){
				D3DXVec3Normalize(&surfelsToRecheckNeighbors[i]->vertex->normal, &newNormal);
			}
			else{
				D3DXVec3Normalize(&surfelsToRecheckNeighbors[i]->vertex->normal, &(-newNormal));
			}
		}
	}*/
}

bool SurfelsToResample::ContainsNewSurfel(ProjectStructs::SURFEL* surfel){
	bool contains = false;

	for(int i = 0; i<surfelsToRecheckNeighbors.size() && !contains; i++){
		contains = MathHelper::Intersection(surfelsToRecheckNeighbors[i], surfel, 0.1f);
	}

	return contains;
}

void SurfelsToResample::Clear(){
	existingSurfels.clear();
	surfelsToRecheckNeighbors.clear();
	surfelsToResetGrid.clear();

	newSurfels.clear();
}

void SurfelsToResample::IsNewSurfelNULL( int i )
{
	if(newSurfels[i] == NULL)
	{
		newSurfels.erase(newSurfels.begin() + i);
	}
}

void SurfelsToResample::DeleteNewSurfel(int i){

	surfelsToRecheckNeighbors.erase(find(surfelsToRecheckNeighbors.begin(), surfelsToRecheckNeighbors.end(), newSurfels[i]));

	delete newSurfels[i];
	newSurfels[i] = NULL;
	newSurfels.erase(newSurfels.begin() + i);

}

