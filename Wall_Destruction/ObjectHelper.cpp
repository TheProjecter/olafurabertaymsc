#include "ObjectHelper.h"

namespace Helpers{
	std::map<std::string, Drawables::MeshlessObject*, Structs::NameComparer> ObjectHelper::MeshlessObjects;
	std::map<std::string, Drawables::MeshlessObject*, Structs::NameComparer>::const_iterator ObjectHelper::MeshlessObjectsIterator;
}
