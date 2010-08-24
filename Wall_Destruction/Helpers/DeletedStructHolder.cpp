#include "DeletedStructHolder.h"
#include <algorithm>

std::vector<ProjectStructs::SURFEL*> DeletedStructHolder::deletedSurfels;

void DeletedStructHolder::AddDeletedSurfel(ProjectStructs::SURFEL* deletedSurfel){
	if(find(deletedSurfels.begin(), deletedSurfels.end(), deletedSurfel) == deletedSurfels.end())
		deletedSurfels.push_back(deletedSurfel);
}	

void DeletedStructHolder::DeleteObjects(){

	for(unsigned int i = 0; i< deletedSurfels.size(); i++){
		
		if(!deletedSurfels[i])
			continue;
		else{

			delete deletedSurfels[i]->vertex;
			deletedSurfels[i]->vertex = NULL;

			delete deletedSurfels[i];
			deletedSurfels[i] = NULL;
		}
	}		

	deletedSurfels.clear();
}

bool DeletedStructHolder::ContainsSurfel( ProjectStructs::SURFEL* surfel )
{
	return find(deletedSurfels.begin(), deletedSurfels.end(), surfel) != deletedSurfels.end();
}

