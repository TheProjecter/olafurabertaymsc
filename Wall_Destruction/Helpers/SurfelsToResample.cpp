#include "SurfelsToResample.h"
#include "Algorithms.h"
#include "PhysicsWrapper.h"
#include "MathHelper.h"
#include "DebugToFile.h"
#include <algorithm>
#include <set>

std::vector<ProjectStructs::SURFEL*> SurfelsToResample::existingSurfels;

std::vector<ProjectStructs::SURFEL*> SurfelsToResample::surfelsToRecheckNeighbors;

std::vector<ProjectStructs::SURFEL*> SurfelsToResample::newSurfels;

std::vector<ProjectStructs::SURFEL*> SurfelsToResample::surfelsToResetGrid;
std::vector<ProjectStructs::SURFEL*> SurfelsToResample::deletedSurfels;

std::map<ProjectStructs::SURFEL*, std::vector<ProjectStructs::SURFEL*>> SurfelsToResample::newNeighbors;
std::map<ProjectStructs::SURFEL*, std::vector<ProjectStructs::SURFEL*>> SurfelsToResample::inverseNewNeighbors;

std::map<ProjectStructs::SURFEL*, std::map<ProjectStructs::SURFEL*, bool>> SurfelsToResample::neigbourCheck;

std::map<float, std::vector<ProjectStructs::SURFEL*>> SurfelsToResample::surfelsLeft;
std::vector<ProjectStructs::SURFEL*> SurfelsToResample::surfelBatch;

ProjectStructs::SURFEL* SurfelsToResample::lastExistingSurfel;

struct refinedSurfelsComparator{
	bool operator() (ProjectStructs::SURFEL* i,ProjectStructs::SURFEL* j) { 
		return i->displacement < j->displacement;
	}
} myRefinedSurfelsComparator;

//const int SurfelsToResample::batchSize = 10;

void SurfelsToResample::AddNeighbourCheck(ProjectStructs::SURFEL* checker, ProjectStructs::SURFEL* checkee){
	return;
	//std::vector<ProjectStructs::SURFEL*> v = ;
	//if(find(v.begin(), v.end(), checkee) == v.end())
	neigbourCheck[checker][checkee] = true;
	neigbourCheck[checkee][checker] = true;

	// insert the neighbours
	std::map<float, ProjectStructs::SURFEL*>::iterator neighbours = checker->neighbors.begin();
	for( ; neighbours != checker->neighbors.end(); neighbours++){
		neigbourCheck[checker][neighbours->second];
		neigbourCheck[neighbours->second][checker];
	}
}

void SurfelsToResample::AddNeighbourCheck(ProjectStructs::SURFEL* checker, std::vector<ProjectStructs::SURFEL*> checkees){
	return;	
	for(int i = 0; i<checkees.size(); i++){
		if(checker == checkees[i])
			continue;

		AddNeighbourCheck(checker, checkees[i]);
	}	
}

std::map<ProjectStructs::SURFEL*, bool> SurfelsToResample::GetNeighbourCheckers(ProjectStructs::SURFEL* surfel){
	
	if(neigbourCheck.find(surfel) != neigbourCheck.end())
		return neigbourCheck[surfel];

	return std::map<ProjectStructs::SURFEL*, bool>();
}

void SurfelsToResample::AddExistingSurfel(ProjectStructs::SURFEL* surfel){
	AddExistingSurfel(surfel, true);
}	

void SurfelsToResample::AddExistingSurfel(ProjectStructs::SURFEL* surfel, bool addNeighbours){
	if(find(surfelsToRecheckNeighbors.begin(), surfelsToRecheckNeighbors.end(), surfel) == surfelsToRecheckNeighbors.end()){
	
		existingSurfels.push_back(surfel);

		surfelsToRecheckNeighbors.push_back(surfel);
		surfelsToResetGrid.push_back(surfel);
		/*newNeighbors[surfel].push_back(surfel);
		inverseNewNeighbors[surfel].push_back(surfel);	
*/
		std::map<float, ProjectStructs::SURFEL*>::iterator neighbourIterator;

		if(addNeighbours){
			for(neighbourIterator = surfel->neighbors.begin(); neighbourIterator != surfel->neighbors.end(); neighbourIterator++){
				AddExistingSurfel(neighbourIterator->second, false);
			}
		}
	}
}	


void SurfelsToResample::AddExistingSurfels(std::vector<ProjectStructs::SURFEL*> surfel){
	for(int i = 0; i< surfel.size(); i++){
		AddExistingSurfel(surfel[i]);
	}
}

void SurfelsToResample::AddNewSurfel(ProjectStructs::SURFEL* surfel, ProjectStructs::SURFEL* parent){
	
	if(find(surfelsToRecheckNeighbors.begin(), surfelsToRecheckNeighbors.end(), surfel) != surfelsToRecheckNeighbors.end()){
		delete surfel;
		surfel = NULL;
		return;
	}

	newSurfels.push_back(surfel);
	surfelsToResetGrid.push_back(surfel);
	ImpactList::AddAffectedSurfel(surfel);
	
}

void SurfelsToResample::AddNewSurfel(ProjectStructs::SURFEL* surfel){
	//return;
// 	newSurfels.push_back(surfel);
// 	AddSurfelToRecheckNeighbors(surfel);	
}	

void SurfelsToResample::AddSurfelToRecheckNeighbors(ProjectStructs::SURFEL* surfel, ProjectStructs::SURFEL* parent){
	if(find(surfelsToRecheckNeighbors.begin(), surfelsToRecheckNeighbors.end(), surfel) == surfelsToRecheckNeighbors.end()){
		surfelsToRecheckNeighbors.push_back(surfel);
		//newNeighbors[parent].push_back(surfel);
		//inverseNewNeighbors[surfel].push_back(parent);
	}
}

void SurfelsToResample::Resample(){
	for(int i = 0; i<existingSurfels.size(); i++){
		ProjectStructs::SURFEL* surfel = existingSurfels[i];
		
		Algorithms::RefineSurfel(existingSurfels[i]);

		surfel->vertex->deltaUV *= Algorithms::Scale;
		surfel->vertex->minorAxis *= Algorithms::Scale;
		surfel->vertex->majorAxis *= Algorithms::Scale;
	}
}

void SurfelsToResample::ResetGrid(ProjectStructs::SURFEL* surfel){
	
	if(surfel == NULL)
		return;

	if(surfel->hasRigidBody){
		PhysicsWrapper::RemoveRigidBodyWithoutLockingWorld(surfel->rigidBody, surfel->contactListener);
		surfel->hasRigidBody = false;

		for(unsigned int j = 0; j < surfel->intersectingCells.size(); j++){
			std::vector<ProjectStructs::SURFEL*>::iterator index = find(surfel->intersectingCells[j]->surfels.begin(),
				surfel->intersectingCells[j]->surfels.end(), surfel);		
			if(index  != surfel->intersectingCells[j]->surfels.end()){
				surfel->intersectingCells[j]->surfels.erase(index);
			}
		}
	}
	
}

void SurfelsToResample::ResetGrid(){
	//PhysicsWrapper::LockWorld();

	for(int i = 0; i<existingSurfels.size(); i++){
		ResetGrid(existingSurfels[i]);
	}
	

	//PhysicsWrapper::UnLockWorld();
}

void SurfelsToResample::CalculateNeighborsForExistingSurfels(){
	for(unsigned int i = 0; i< existingSurfels.size(); i++){

		//	Algorithms::CalculateNeighbors(existingSurfels[i]);
		//DebugToFile::StartTimer();
		//int lastWholeList = Algorithms::WentThroughWholeListCount;
		Algorithms::CalculateNeighbors(existingSurfels[i], ImpactList::GetAffectedSurfelVector());
		//DebugToFile::EndTimer("Calculated neighbour for existing surfel, neighbour count %d, whole list = %d ", existingSurfels[i]->neighbors.size(), Algorithms::WentThroughWholeListCount - lastWholeList);

		if(existingSurfels[i]->vertexGridCell)
			existingSurfels[i]->vertexGridCell->neighborCount = 0;
	}
}

void SurfelsToResample::CalculateNeighborsForNewSurfels(){
	for(unsigned int i = 0; i< newSurfels.size(); i++){
		//Algorithms::CalculateNeighbors(newSurfels[i]);
		//DebugToFile::StartTimer();
		//int lastWholeList = Algorithms::WentThroughWholeListCount;
		Algorithms::CalculateNeighbors(newSurfels[i], ImpactList::GetAffectedSurfelVector());
		//DebugToFile::EndTimer("Calculated neighbour for new surfel, neighbour count %d, whole list = %d ", newSurfels[i]->neighbors.size(), Algorithms::WentThroughWholeListCount - lastWholeList);

		if(newSurfels[i]->vertexGridCell)
			newSurfels[i]->vertexGridCell->neighborCount = 0;
	}
}

void SurfelsToResample::CalculateNeighbors(){

	std::vector<ProjectStructs::SURFEL*> newSurfelsToRecheckNeighbors, tmpSurfelsToRecheckNeighbors = surfelsToRecheckNeighbors;
	std::map<float, ProjectStructs::SURFEL*>::iterator surfelIterator;
	
	for(unsigned int i = 0; i<tmpSurfelsToRecheckNeighbors.size(); i++){

		surfelsToRecheckNeighbors[i]->oldNeighbors = surfelsToRecheckNeighbors[i]->neighbors ;
		surfelsToRecheckNeighbors[i]->oldInverseNeighbors = surfelsToRecheckNeighbors[i]->inverseNeighbors;

		// erase the neighbours awareness of this surfel
		std::map<float, ProjectStructs::SURFEL*>::iterator neighbourIterator;

		for(neighbourIterator = surfelsToRecheckNeighbors[i]->neighbors.begin(); neighbourIterator != surfelsToRecheckNeighbors[i]->neighbors.end(); neighbourIterator++){
			neighbourIterator->second->neighbors.erase(neighbourIterator->second->inverseNeighbors[surfelsToRecheckNeighbors[i]]);
			neighbourIterator->second->inverseNeighbors.erase(surfelsToRecheckNeighbors[i]);
		}

		surfelsToRecheckNeighbors[i]->neighbors.clear();
		surfelsToRecheckNeighbors[i]->inverseNeighbors.clear();

		for(surfelIterator = tmpSurfelsToRecheckNeighbors[i]->neighbors.begin(); surfelIterator != tmpSurfelsToRecheckNeighbors[i]->neighbors.end(); surfelIterator++){
			AddSurfelToRecheckNeighbors(tmpSurfelsToRecheckNeighbors[i], NULL);
		}
	}

	for(unsigned int i = 0; i< surfelsToRecheckNeighbors.size(); i++){
		
		//DebugToFile::Debug("calculating neighbour for 0x%x", surfelsToRecheckNeighbors[i]);
		
		Algorithms::CalculateNeighbors(surfelsToRecheckNeighbors[i]);
		
		if(surfelsToRecheckNeighbors[i]->vertexGridCell)
			surfelsToRecheckNeighbors[i]->vertexGridCell->neighborCount = 0;
	}
}

bool SurfelsToResample::ContainsNewSurfel(ProjectStructs::SURFEL* surfel){
	bool contains = false;

	for(int i = 0; i<newSurfels.size() && !contains; i++){
		contains = MathHelper::Intersection(newSurfels[i], surfel, 0.3f);

		if(contains){
			lastExistingSurfel = newSurfels[i];
		}
	}

	return contains;
}

ProjectStructs::SURFEL* SurfelsToResample::GetLastIntersectingSurfel(){
	return lastExistingSurfel;
}

void SurfelsToResample::Clear(){

	SurfelsToResample::deletedSurfels.clear();
	SurfelsToResample::existingSurfels.clear();
	SurfelsToResample::inverseNewNeighbors.clear();
	SurfelsToResample::newNeighbors.clear();
	SurfelsToResample::newSurfels.clear();
	SurfelsToResample::surfelsToRecheckNeighbors.clear();
	SurfelsToResample::surfelsToResetGrid.clear();
	SurfelsToResample::surfelsLeft.clear();
	SurfelsToResample::surfelBatch.clear();

}

void SurfelsToResample::IsNewSurfelNULL( int i )
{
	if(newSurfels[i] == NULL)
	{
		DeleteNewSurfel(i);
	}
}

void SurfelsToResample::DeleteNewSurfel(int i){

	std::map<ProjectStructs::SURFEL*, std::vector<ProjectStructs::SURFEL*>>::iterator newNeighborsIterator;

	std::vector<ProjectStructs::SURFEL*> deletedNeighbors, deletedInverseNeighbors;

	for(int j = 0; j< newNeighbors[newSurfels[i]].size(); j++){
		inverseNewNeighbors.erase(newNeighbors[newSurfels[i]][j]);
	}

	for(int j = 0; j< newNeighbors[NULL].size(); j++){
		inverseNewNeighbors.erase(newNeighbors[NULL][j]);
	}

	newNeighbors.erase(newSurfels[i]);
	inverseNewNeighbors.erase(newSurfels[i]);
	
	surfelsToRecheckNeighbors.erase(find(surfelsToRecheckNeighbors.begin(), surfelsToRecheckNeighbors.end(), newSurfels[i]));

	deletedSurfels.push_back(newSurfels[i]);

//	delete newSurfels[i];
	newSurfels[i] = NULL;
	newSurfels.erase(newSurfels.begin() + i);

	newNeighbors.erase(NULL);
	inverseNewNeighbors.erase(NULL);
}

void SurfelsToResample::ResetBatch()
{
	surfelBatch.clear();
	
	std::map<float, std::vector<ProjectStructs::SURFEL*>>::iterator surfelsLeftIterator = surfelsLeft.begin();
	
	int newBatchSize = 0;
	std::vector<float> mapElementsToErase;
	for( ; surfelsLeftIterator != surfelsLeft.end() && newBatchSize < batchSize; surfelsLeftIterator++){
		int surfelsToErase = 0;

		for(int i = 0; i < surfelsLeftIterator->second.size() && newBatchSize < batchSize; i++ ){
			surfelBatch.push_back(surfelsLeftIterator->second[i]);
			newBatchSize++;
			surfelsToErase++;
		}

		surfelsLeftIterator->second.erase(surfelsLeftIterator->second.begin(), surfelsLeftIterator->second.begin() + surfelsToErase);

		if(surfelsLeftIterator->second.size() == 0)
			mapElementsToErase.push_back(surfelsLeftIterator->first);
	}
	
	for(int i = 0; i< mapElementsToErase.size(); i++){
		surfelsLeft.erase(mapElementsToErase[i]);
	}
}

void SurfelsToResample::ResampleBatch()
{
	for(int i = 0; i<surfelBatch.size(); i++){
		ProjectStructs::SURFEL* surfel = surfelBatch[i];

		Algorithms::RefineSurfel(surfelBatch[i]);

		surfel->vertex->deltaUV *= Algorithms::Scale;
		surfel->vertex->minorAxis *= Algorithms::Scale;
		surfel->vertex->majorAxis *= Algorithms::Scale;
	}
}

void SurfelsToResample::CalculateBatchNeighbors(){
	for(int i = 0; i<surfelBatch.size(); i++){
		ProjectStructs::SURFEL* surfel = surfelBatch[i];
		
		surfelBatch[i]->neighbors.clear();
		surfelBatch[i]->inverseNeighbors.clear();

		Algorithms::CalculateNeighbors(surfelBatch[i]);
	}
}

void SurfelsToResample::ResetGridBatch()
{
	for(int i = 0; i < surfelBatch.size(); i++){
		if(existingSurfels[i] == NULL)
			continue;

		//if(existingSurfels[i]->hasRigidBody){
		PhysicsWrapper::RemoveRigidBodyWithoutLockingWorld(surfelBatch[i]->rigidBody, surfelBatch[i]->contactListener);
		existingSurfels[i]->hasRigidBody = false;

		for(unsigned int j = 0; j < surfelBatch[i]->intersectingCells.size(); j++){
			std::vector<ProjectStructs::SURFEL*>::iterator index = find(surfelBatch[i]->intersectingCells[j]->surfels.begin(),
				surfelBatch[i]->intersectingCells[j]->surfels.end(), surfelBatch[i]);		
			if(index  != surfelBatch[i]->intersectingCells[j]->surfels.end()){
				surfelBatch[i]->intersectingCells[j]->surfels.erase(index);
			}
		}
		//}
	}

}
