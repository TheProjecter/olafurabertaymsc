#include "ObjectHelper.h"

namespace Helpers{
	std::map<std::string, Drawables::MeshlessObject*, ProjectStructs::NameComparer> ObjectHelper::MeshlessObjects;
	std::map<std::string, Drawables::MeshlessObject*, ProjectStructs::NameComparer>::const_iterator ObjectHelper::MeshlessObjectsIterator;
}
