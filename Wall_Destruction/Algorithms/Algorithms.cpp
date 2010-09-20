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
const float Algorithms::Scale = 0.75f;
int Algorithms::WentThroughWholeListCount = 0;

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

struct DistanceComparator{
	bool operator() (ProjectStructs::SURFEL* i,ProjectStructs::SURFEL* j) { 
		return D3DXVec3LengthSq(&(surfel->initialPosition - i->initialPosition)) < D3DXVec3LengthSq(&(surfel->initialPosition - j->initialPosition));
	}
	ProjectStructs::SURFEL* surfel;
} MyDistanceComparator;

void Algorithms::CalculateNeighbors(ProjectStructs::SURFEL* surfel, std::vector<ProjectStructs::SURFEL*> deformedSurfels){
	
	std::map<float, ProjectStructs::SURFEL*>::iterator neighbours = surfel->neighbors.begin();

	for( ; neighbours != surfel->neighbors.end(); neighbours++){
		deformedSurfels.push_back(neighbours->second);
	}	

	MyDistanceComparator.surfel = surfel;
	sort(deformedSurfels.begin(), deformedSurfels.end(), MyDistanceComparator);

	surfel->neighbors.clear();
	surfel->inverseNeighbors.clear();

	D3DXVECTOR3 firstPos;
	float firstAngle = 0.0f;

	int neighbourCount = Helpers::Globals::PI / angleThreshold;

	for(int i = 1; i < deformedSurfels.size(); i++){
		ProjectStructs::SURFEL* neighbour = deformedSurfels[i];

		if(surfel == neighbour || surfel->initialPosition == neighbour->initialPosition)
			continue;

		if(surfel->neighbors.size() == 0 ){
			firstPos = neighbour->initialPosition;
			surfel->neighbors[firstAngle] = neighbour;
			surfel->inverseNeighbors[neighbour] = firstAngle;
			continue;
		}
		else {
			// check the angle between this surfel neighbor and the last surfel neighbor
			float angle = MathHelper::GetPlaneAngle(surfel->vertex->normal, surfel->initialPosition, firstPos, neighbour->initialPosition);
			
			AngleCheck checker = CheckAngle(angle, surfel);
			bool addSurfel = true;
			if(checker.tooCloseToOverAngle && checker.overAngle != -1){
				float previousLength = D3DXVec3Length(&(surfel->neighbors[checker.overAngle]->initialPosition - surfel->initialPosition));
				float currentLength = D3DXVec3Length(&(neighbour->initialPosition- surfel->initialPosition));
				
				addSurfel &= currentLength < previousLength;
				
				if(currentLength < previousLength){					
					surfel->inverseNeighbors.erase(surfel->neighbors[checker.overAngle]);
					surfel->neighbors.erase(checker.overAngle);
					if(checker.overAngle == firstAngle)
						firstAngle = checker.overAngle;
				}
			}
			
			if(checker.tooCloseToUnderAngle && checker.underAngle != -1){
				float previousLength = D3DXVec3Length(&(surfel->neighbors[checker.underAngle]->initialPosition- surfel->initialPosition));
				float currentLength = D3DXVec3Length(&(neighbour->initialPosition- surfel->initialPosition));

				addSurfel &= currentLength < previousLength;
				if(currentLength < previousLength){					
					surfel->inverseNeighbors.erase(surfel->neighbors[checker.underAngle]);
					surfel->neighbors.erase(checker.underAngle);
					if(checker.underAngle == firstAngle)
						firstAngle = checker.underAngle;
				}
			}

			if(addSurfel){
				surfel->neighbors[angle] = neighbour;
				surfel->inverseNeighbors[neighbour] = angle;
			}
	
			if(surfel->neighbors.size() == neighbourCount)
				return;
		}	
	}
	Algorithms::WentThroughWholeListCount++;
	return;
}


AngleCheck Algorithms::CheckAngle(float angle, ProjectStructs::SURFEL* surfel){

	int closest = 0;
	float minDifference = FLT_MAX; // eða eitthvað

	std::map<float, ProjectStructs::SURFEL*>::iterator surfelNeighbours;
	AngleCheck checker;
	checker.underAngle = -1;
	checker.overAngle = -1;
	checker.tooCloseToOverAngle = false;
	checker.tooCloseToUnderAngle = false;

	for(surfelNeighbours = surfel->neighbors.begin(); surfelNeighbours != surfel->neighbors.end(); surfelNeighbours++){
		float absolute = fabs(angle- surfelNeighbours->first );
		float twoPiAbsolute = fabs(Helpers::Globals::TWO_PI - angle + surfelNeighbours->first );
		if( absolute < minDifference && absolute <= twoPiAbsolute){
			if(angle > surfelNeighbours->first){
				checker.underAngle = surfelNeighbours->first;
				checker.tooCloseToUnderAngle = absolute < angleThreshold;
			}
			else {
				checker.overAngle = surfelNeighbours->first;
				checker.tooCloseToOverAngle = absolute < angleThreshold ;
			}
			
			minDifference = absolute;
		}
		else if(twoPiAbsolute < minDifference && twoPiAbsolute < absolute){
			if(angle > surfelNeighbours->first){
				checker.underAngle = surfelNeighbours->first;
				checker.tooCloseToUnderAngle = twoPiAbsolute < angleThreshold;
			}
			else {
				checker.overAngle = surfelNeighbours->first;
				checker.tooCloseToOverAngle = twoPiAbsolute < angleThreshold ;
			}

			minDifference = twoPiAbsolute ;
		}
		else if(checker.overAngle == -1){
			checker.overAngle = surfelNeighbours->first;
			checker.tooCloseToOverAngle = absolute < angleThreshold || Helpers::Globals::TWO_PI - absolute < angleThreshold ;
		}

	}
	return checker;
}

// algorithm taken from Dynamic surfel set refinement for high-quality rendering
void Algorithms::CalculateNeighbors(ProjectStructs::SURFEL* surfel){
	float firstAngle = 0.0f;

	D3DXVECTOR3 firstPos;
	std::map<ProjectStructs::SURFEL*, bool> neighbours = SurfelsToResample::GetNeighbourCheckers(surfel);

	if(neighbours.size() > 0){

		std::map<ProjectStructs::SURFEL*, bool>::iterator neighboursIterator = neighbours.begin(); 
		for( ; neighboursIterator != neighbours.end(); neighboursIterator++){
			
			ProjectStructs::SURFEL* neighbour = neighboursIterator->first;

			if(surfel == neighbour)
				continue;

			if(surfel->neighbors.size() == 0 ){
				firstPos = neighbour->initialPosition;
				surfel->neighbors[firstAngle] = neighbour;
				surfel->inverseNeighbors[neighbour] = firstAngle;
				continue;
			}
			else {
				// check the angle between this surfel neighbor and the last surfel neighbor
				float angle = MathHelper::GetPlaneAngle(surfel->vertex->normal, surfel->initialPosition, firstPos, neighbour->initialPosition);

				if(surfel->neighbors.count(angle) != 0)
				{
					ProjectStructs::SURFEL* angleNeighbour = surfel->neighbors[angle];
					float previousLength = D3DXVec3Length(&(angleNeighbour->initialPosition - surfel->initialPosition));
					float currentLength = D3DXVec3Length(&(neighbour->initialPosition - surfel->initialPosition));

					if(currentLength < previousLength){
						angleNeighbour->isChecked = false;
						surfel->inverseNeighbors.erase(angleNeighbour);
						surfel->neighbors[angle] = neighbour;
						surfel->inverseNeighbors[neighbour] = angle;
					}
				}
				else{
					if(surfel->inverseNeighbors.count(neighbour) == 0){
						surfel->neighbors[angle] = neighbour;
						surfel->inverseNeighbors[neighbour] = angle;
					}
				}
			}

		}
		
		DecreaseNeighbors(surfel);

		return;

	}

	std::map<unsigned int, std::vector<unsigned int>> checkedSurfels; // [cellIndex , vector of surfel indices]

	//DebugToFile::StartTimer();
	for(unsigned int i = 0; i<surfel->intersectingCells.size(); i++){
		ProjectStructs::Phyxel_Grid_Cell* cell = surfel->intersectingCells[i];

		checkedSurfels[i] = std::vector<unsigned int>();

		// add all neighboring surfels into the map
		for(unsigned int j = 0; j < cell->surfels.size(); j++){

			//intersectionið er ekki alveg að gera sig
			if(surfel == cell->surfels[j] /*|| !MathHelper::Intersection(surfel, cell->surfels[j], 0.75f) || !MathHelper::SameDirection(surfel->vertex->normal, cell->surfels[j]->vertex->normal)*/)
				continue;

			if(surfel->neighbors.size() == 0 ){
				firstPos = MathHelper::ProjectVectorToPlane(surfel->vertex->normal, surfel->initialPosition, cell->surfels[j]->initialPosition);
				surfel->neighbors[firstAngle] = cell->surfels[j];
				surfel->inverseNeighbors[cell->surfels[j]] = firstAngle;
				cell->surfels[j]->isChecked= true;
				checkedSurfels[i].push_back(j);
				continue;
			}
			else if(!cell->surfels[j]->isChecked){
				// check the angle between this surfel neighbor and the last surfel neighbor
				//float angle = MathHelper::Get3DAngle(cell->surfels[j]->vertex->pos - surfel->vertex->pos, firstPos - surfel->vertex->pos, surfel->vertex->normal);
				//float angle = MathHelper::Get3DAngle(surfel, cell->surfels[j]->vertex->pos, firstPos);
				float angle = MathHelper::GetPlaneAngle(surfel->vertex->normal, surfel->initialPosition, firstPos, cell->surfels[j]->initialPosition);
			
				if(surfel->neighbors.count(angle) != 0)
				{
					float previousLength = D3DXVec3Length(&(surfel->neighbors[angle]->initialPosition - surfel->initialPosition));
					float currentLength = D3DXVec3Length(&(cell->surfels[j]->initialPosition - surfel->initialPosition));

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
					if(surfel->inverseNeighbors.count(cell->surfels[j]) == 0){
						surfel->neighbors[angle] = cell->surfels[j];
						surfel->inverseNeighbors[cell->surfels[j]] = angle;
						cell->surfels[j]->isChecked= true;
						checkedSurfels[i].push_back(j);
					}
				}
			}
		}		
	}
	//DebugToFile::EndTimer("Neighbors - First loop");

	//DebugToFile::StartTimer();
	DecreaseNeighbors(surfel);
	//DebugToFile::EndTimer("Neighbors - Decrease neighbors");
}

void Algorithms::CalculateNeighbor(ProjectStructs::SURFEL* surfel, ProjectStructs::SURFEL* neighbor){
	if(neighbor == NULL || neighbor->vertex == NULL)
		return;

	if(surfel->neighbors.size() == 0 ){
		surfel->neighbors[0.0f] = neighbor;
		surfel->inverseNeighbors[neighbor] = 0.0f;
		return;
	}
	else{
		// check the angle between this surfel neighbor and the last surfel neighbor
		//float angle = MathHelper::Get3DAngle(neighbor->vertex->pos - surfel->vertex->pos, surfel->neighbors[0.0f]->vertex->pos - surfel->vertex->pos, surfel->vertex->normal);
		//float angle = MathHelper::Get3DAngle(surfel, cell->surfels[j]->vertex->pos, firstPos);
		float angle = MathHelper::GetPlaneAngle(surfel->vertex->normal, surfel->initialPosition, surfel->neighbors[0.0f]->initialPosition , neighbor->initialPosition);

		if(surfel->neighbors.count(angle) != 0)
		{
			float previousLength = D3DXVec3Length(&(surfel->neighbors[angle]->initialPosition - surfel->initialPosition));
			float currentLength = D3DXVec3Length(&(neighbor->initialPosition - surfel->initialPosition));

			if(currentLength < previousLength){	
				surfel->inverseNeighbors.erase(surfel->neighbors[angle]);
				surfel->neighbors[angle] = neighbor;
				surfel->inverseNeighbors[neighbor] = angle;
			}
		}
		else{
			if(surfel->inverseNeighbors.count(neighbor) == 0){
				surfel->neighbors[angle] = neighbor;
				surfel->inverseNeighbors[neighbor] = angle;
			}
		}
	}
}

struct comparatorObject{
	bool operator() (ProjectStructs::SURFEL* i,ProjectStructs::SURFEL* j) { 
		return surfel->inverseNeighbors[i] < surfel->inverseNeighbors[j];
	}
	ProjectStructs::SURFEL* surfel;
} myComparatorObject;

// algorithm taken from Dynamic surfel set refinement for high-quality rendering
void Algorithms::RefineSurfel(ProjectStructs::SURFEL* surfel){

	myComparatorObject.surfel = surfel;
	
	if(surfel->vertexGridCell && surfel->vertexGridCell->changed == false && surfel->vertexGridCell->neighborCount != 0 && surfel->vertexGridCell->surfels.size() == 0){	
		ReleaseCOM(surfel->vertexGridCell->neighborVertexBuffer);	
	}
	
	if(surfel->vertexGridCell) {
		surfel->vertexGridCell->neighborCount = 0;
		surfel->vertexGridCell->changed = true;
	}
	
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

//	int tmpCount = 0;

	std::map<ProjectStructs::SURFEL*, std::vector<ProjectStructs::SURFEL*>> polygonNeighbors;

	std::vector<ProjectStructs::SURFEL*> newSurfels;

	// add the neighbours of the surfels into the surfelsInPolygon list
	for(neighborIterator = surfel->neighbors.begin(); neighborIterator != surfel->neighbors.end(); neighborIterator++){
		//DebugToFile::Debug("Surfel 0x%x at (%.3f, %.3f, %.3f)", neighborIterator->second, neighborIterator->second->vertex->pos.x, neighborIterator->second->vertex->pos.y, neighborIterator->second->vertex->pos.z);
		surfelsInPolygon.push_back(neighborIterator->second);

	//	SurfelsToResample::AddNeighbourCheck(surfel, neighborIterator->second);
  
		for(neighborNeighborIterator = neighborIterator->second->neighbors.begin(); neighborNeighborIterator != neighborIterator->second->neighbors.end(); neighborNeighborIterator++){
			if(surfel->inverseNeighbors.count(neighborNeighborIterator->second) != 0)
			{
				polygonNeighbors[neighborIterator->second].push_back(neighborNeighborIterator->second);
			}
	//		tmpCount++;
		}
	}

	for(UINT i = 1; i < surfelsInPolygon.size(); i++){
		ProjectStructs::SURFEL* polygonSurfel = surfelsInPolygon[i];
		std::vector<ProjectStructs::SURFEL*> polygonNeighboursVector = polygonNeighbors[polygonSurfel];
		bool validSurfel = polygonNeighboursVector.size() != 0 && (polygonCount == 0 || find(neighborhoodPolygons[polygonCount-1].begin(), neighborhoodPolygons[polygonCount-1].end(), polygonSurfel) == neighborhoodPolygons[polygonCount-1].end());

		if(!validSurfel)
			continue;

		neighborhoodPolygons.push_back(std::vector<ProjectStructs::SURFEL*>());
		
		neighborhoodPolygons[polygonCount].push_back(surfel);
		neighborhoodPolygons[polygonCount].push_back(polygonSurfel);
		surfelsUsed.push_back(polygonSurfel);
// 
// 		for(UINT j = 0; j<polygonNeighbors[polygonSurfel].size(); j++){
// 			neighborhoodPolygons[polygonCount].push_back(polygonNeighbors[polygonSurfel][j]);	
// //			tmpCount++;
// 		}
		neighborhoodPolygons[polygonCount].insert(neighborhoodPolygons[polygonCount].end(), polygonNeighboursVector.begin(), polygonNeighboursVector.end());

		sort(neighborhoodPolygons[polygonCount].begin(), neighborhoodPolygons[polygonCount].end(), myComparatorObject);

		polygonCount++;
	}

	std::vector<ProjectStructs::SURFEL*> interiorSurfels;
	std::vector<ProjectStructs::SURFEL*> exteriorSurfels;

	// ok, now the polygons have been created and now surfels have to be inserted
	for(UINT i = 0; i< neighborhoodPolygons.size(); i++){
		
		ProjectStructs::SURFEL* ithSurfel, *jthSurfel;

		float level = (float)neighborhoodPolygons[i].size();
		float invLevel = 1.0f / level;

		ProjectStructs::SURFEL* s = ProjectStructs::StructHelper::CreateSurfelPointer(MathHelper::GetZeroVector(), MathHelper::GetZeroVector(), MathHelper::GetZeroVector(),
			MathHelper::GetZeroVector(), D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR2(0.0f, 0.0f));


		int iLevel = (int)level-1;
		std::vector<ProjectStructs::SURFEL*> newSurfels;

		for(UINT j = 0; j<level; j++){
			ithSurfel = neighborhoodPolygons[i][j];
			s->vertex->pos += ithSurfel->vertex->pos;
			s->vertex->majorAxis += ithSurfel->vertex->majorAxis;
			s->vertex->minorAxis += ithSurfel->vertex->minorAxis;
			s->vertex->normal += ithSurfel->vertex->normal;
			s->lastDisplacement += ithSurfel->lastDisplacement;
			s->initialPosition += ithSurfel->initialPosition;
			s->vertex->UV += ithSurfel->vertex->UV ;//* 2.0f;
			s->vertex->deltaUV += ithSurfel->vertex->deltaUV;
			s->displacement += ithSurfel->displacement;

			int next= (j+1)%iLevel;

			ithSurfel = neighborhoodPolygons[i][next];
			jthSurfel = neighborhoodPolygons[i][j];

			ProjectStructs::SURFEL* newSurfel = ProjectStructs::StructHelper::CreateSurfelPointer(MathHelper::GetZeroVector(), MathHelper::GetZeroVector(), MathHelper::GetZeroVector(),
				MathHelper::GetZeroVector(), D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR2(0.0f, 0.0f));

			newSurfel->vertex->pos = (jthSurfel->vertex->pos + ithSurfel->vertex->pos) * 0.5f;
			newSurfel->vertex->normal = (jthSurfel->vertex->normal + ithSurfel->vertex->normal) * 0.5f;
			newSurfel->lastDisplacement = (jthSurfel->lastDisplacement + ithSurfel->lastDisplacement) * 0.5f;
			newSurfel->initialPosition= (jthSurfel->initialPosition+ ithSurfel->initialPosition) * 0.5f;
			newSurfel->vertex->UV = (jthSurfel->vertex->UV + ithSurfel->vertex->UV) * 0.5f ;

			newSurfel->vertex->deltaUV = (jthSurfel->vertex->deltaUV + ithSurfel->vertex->deltaUV) * 0.5f * Algorithms::Scale;
			newSurfel->vertex->majorAxis = (jthSurfel->vertex->majorAxis + ithSurfel->vertex->majorAxis) * 0.5f * Algorithms::Scale;
			newSurfel->vertex->minorAxis = (jthSurfel->vertex->minorAxis + ithSurfel->vertex->minorAxis) * 0.5f * Algorithms::Scale;

			newSurfel->vertex->clipPlane = MathHelper::GetClipPlane(jthSurfel->vertex->clipPlane, ithSurfel->vertex->clipPlane);

			newSurfel->vertex->frontFacing = surfel->vertex->frontFacing;
			newSurfel->displacement = (jthSurfel->displacement + ithSurfel->displacement) * 0.5f;

			//			tmpCount1++;

			if(SurfelsToResample::ContainsNewSurfel(newSurfel))
			{		
				delete newSurfel;
				newSurfel = NULL;
			}
			else{
				SurfelsToResample::AddNewSurfel(newSurfel, surfel);	
			}
		}
		
		s->vertex->pos *= invLevel;
		
		s->vertex->deltaUV *= invLevel * Algorithms::Scale;
		s->vertex->majorAxis *= invLevel * Algorithms::Scale;
		s->vertex->minorAxis *= invLevel * Algorithms::Scale;

		s->vertex->normal *= invLevel;
		s->vertex->UV *= invLevel;
		s->lastDisplacement *= invLevel;
		s->initialPosition *= invLevel;
		s->displacement *= invLevel;
		s->vertex->frontFacing = surfel->vertex->frontFacing;

		if(SurfelsToResample::ContainsNewSurfel(s))
		{
			delete s;
			s = NULL;
		}
		else{
			SurfelsToResample::AddNewSurfel(s, surfel);	
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
		
		if(abs(angles[lastIndex] - angles[currentIndex]) < angleThreshold || abs(Helpers::Globals::TWO_PI - angles[currentIndex] - angles[lastIndex]) < angleThreshold){
			if(D3DXVec3Length(&(surfels[lastIndex]->initialPosition - surfel->initialPosition)) < D3DXVec3Length(&(surfels[currentIndex]->initialPosition - surfel->initialPosition))){
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
