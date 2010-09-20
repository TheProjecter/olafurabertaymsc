#ifndef SURFEL_TO_RESAMPLE_H
#define SURFEL_TO_RESAMPLE_H

#include "Projectiles.h"
#include <algorithm>


class SurfelsToResample
{
public:
	static void AddExistingSurfel(ProjectStructs::SURFEL* surfel);
	static void AddExistingSurfels(std::vector<ProjectStructs::SURFEL*> surfel);
	static void AddExistingSurfel(ProjectStructs::SURFEL* surfel, bool addNeighbours);
	static void AddNewSurfel(ProjectStructs::SURFEL* surfel);
	static void AddNewSurfel(ProjectStructs::SURFEL* surfel, ProjectStructs::SURFEL* parent);

	static std::vector<ProjectStructs::SURFEL*> GetNewResampledSurfels(ProjectStructs::SURFEL* parent){
		if(newNeighbors.find(parent) == newNeighbors.end())
			return std::vector<ProjectStructs::SURFEL*>();

		return newNeighbors[parent];
	}

	static std::vector<ProjectStructs::SURFEL*> GetParentOfNewSurfel(ProjectStructs::SURFEL* newSurfel){
		if(inverseNewNeighbors.find(newSurfel) == inverseNewNeighbors.end())
			return std::vector<ProjectStructs::SURFEL*>();
		
		return inverseNewNeighbors[newSurfel];
	}

	static void AddSurfelToRecheckNeighbors(ProjectStructs::SURFEL* surfel, ProjectStructs::SURFEL* parent);
	
	static int GetExistingSurfelCount(){return existingSurfels.size();}
	static std::vector<ProjectStructs::SURFEL*> GetExistingSurfels(){return existingSurfels;}
	static ProjectStructs::SURFEL* GetExistingSurfel(int i){return existingSurfels[i];}

	static int GetNewSurfelSize(){return newSurfels.size();}
	static ProjectStructs::SURFEL* GetNewSurfel(int i){return newSurfels[i];}
	static std::vector<ProjectStructs::SURFEL*> GetNewSurfels(){
		return newSurfels;
	}

	static void ClearNewSurfels(){newSurfels.clear();}

	static void ResetGrid();
	static void ResetGrid(ProjectStructs::SURFEL* surfel);
	static bool ContainsNewSurfel(ProjectStructs::SURFEL* surfel);

	static int GetSurfelsLeftCount(){return surfelsLeft.size();}

	static void Resample();
	static void CalculateNeighbors();
	static void CalculateNeighborsForExistingSurfels();
	static void CalculateNeighborsForNewSurfels();
	static void Clear();
	static void IsNewSurfelNULL( int i );
	
	static bool HasSurfelBeenDeleted( ProjectStructs::SURFEL* surfel){
		return find(deletedSurfels.begin(), deletedSurfels.end(), surfel) != deletedSurfels.end();
	}

	static void AddNeighbourCheck(ProjectStructs::SURFEL* checker, ProjectStructs::SURFEL* checkee);
	static void AddNeighbourCheck(ProjectStructs::SURFEL* checker, std::vector<ProjectStructs::SURFEL*> checkees);
	static std::map<ProjectStructs::SURFEL*, bool>  GetNeighbourCheckers(ProjectStructs::SURFEL* surfel);
	static ProjectStructs::SURFEL* GetLastIntersectingSurfel();

	static void DeleteNewSurfel(int i);
	static void ResetBatch();
	static void ResampleBatch();
	static std::vector<ProjectStructs::SURFEL*> GetSurfelBatch(){return surfelBatch;}
	static void CalculateBatchNeighbors();
	static void ResetGridBatch();

private:
	static std::vector<ProjectStructs::SURFEL*> surfelsToResetNeighbors;
	
	static std::vector<ProjectStructs::SURFEL*> surfelsToResetGrid;
	
	static std::vector<ProjectStructs::SURFEL*> existingSurfels;
	
	// displacement distance, surfels with that displacement
	static std::map<float, std::vector<ProjectStructs::SURFEL*>> surfelsLeft;
	
	static std::vector<ProjectStructs::SURFEL*> surfelBatch;

	static std::vector<ProjectStructs::SURFEL*> newSurfels;
	
	static std::vector<ProjectStructs::SURFEL*> surfelsToRecheckNeighbors;

	static std::map<ProjectStructs::SURFEL*, std::vector<ProjectStructs::SURFEL*>> newNeighbors;
	static std::map<ProjectStructs::SURFEL*, std::vector<ProjectStructs::SURFEL*>> inverseNewNeighbors;

	static std::map<ProjectStructs::SURFEL*, std::map<ProjectStructs::SURFEL*, bool>> neigbourCheck;

	static std::vector<ProjectStructs::SURFEL*> deletedSurfels;
	static ProjectStructs::SURFEL* lastExistingSurfel;

	static const int batchSize = 25;
};

#endif
