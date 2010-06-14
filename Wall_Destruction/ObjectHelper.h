#ifndef OBJECT_HELPER
#define OBJECT_HELPER

#include <map>
#include <string>
#include "MeshlessObject.h"
#include "Structs.h"

namespace Helpers{
	class ObjectHelper
	{
	public:
		static std::map<std::string, Drawables::MeshlessObject*, Structs::NameComparer> MeshlessObjects;
		static std::map<std::string, Drawables::MeshlessObject*, Structs::NameComparer>::const_iterator MeshlessObjectsIterator;		
	};
}

#endif
