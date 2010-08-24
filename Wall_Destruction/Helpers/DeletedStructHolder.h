#ifndef VOLUME_STRUCT_HOLDER
#define VOLUME_STRUCT_HOLDER

#include "Structs.h"

class DeletedStructHolder
{
public:
	static void AddDeletedSurfel(ProjectStructs::SURFEL* deletedSurfel);

	static unsigned int GetDeletedSurfelSize(){return deletedSurfels.size();}

	static ProjectStructs::SURFEL* GetDeletedSurfel(unsigned int i){return deletedSurfels[i];}

	static void DeleteObjects();

	static bool ContainsSurfel( ProjectStructs::SURFEL* surfel );

private:
	static std::vector<ProjectStructs::SURFEL*> deletedSurfels;
};

#endif