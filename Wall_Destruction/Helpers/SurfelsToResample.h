#ifndef SURFEL_TO_RESAMPLE_H
#define SURFEL_TO_RESAMPLE_H

#include "Projectiles.h"

class SurfelsToResample
{
public:
	static void AddExistingSurfel(ProjectStructs::SURFEL* surfel);

	static void AddNewSurfel(ProjectStructs::SURFEL* surfel);

	static void AddSurfelToRecheckNeighbors(ProjectStructs::SURFEL* surfel);
	
	static int GetSurfelCount(){return existingSurfels.size();}
	static ProjectStructs::SURFEL* GetSurfel(int i){return existingSurfels[i];}

	static int GetNewSurfelSize(){return newSurfels.size();}
	static ProjectStructs::SURFEL* GetNewSurfel(int i){return newSurfels[i];}

	static void ClearNewSurfels(){newSurfels.clear();}

	static void ResetGrid();
	static bool ContainsNewSurfel(ProjectStructs::SURFEL* surfel);

	static void Resample();
	static void CalculateNeighbors();
	static void Clear();
	static void IsNewSurfelNULL( int i );

	static void DeleteNewSurfel(int i);
private:
	static std::vector<ProjectStructs::SURFEL*> surfelsToResetNeighbors;
	
	static std::vector<ProjectStructs::SURFEL*> surfelsToResetGrid;
	
	static std::vector<ProjectStructs::SURFEL*> existingSurfels;
	
	static std::vector<ProjectStructs::SURFEL*> newSurfels;
	
	static std::vector<ProjectStructs::SURFEL*> surfelsToRecheckNeighbors;
};

#endif
