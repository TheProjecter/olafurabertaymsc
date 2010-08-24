#ifndef MESHLESS_OBJECT_H
#define MESHLESS_OBJECT_H

#include "Drawable.h"
#include "Globals.h"
#include "Volume.h"
#include "CSGTree.h"

namespace Drawables{

	class MeshlessObject 
	{
	public:
		MeshlessObject(){}
		MeshlessObject(std::string pointCloudXmlFile);

		~MeshlessObject(void);

		void Draw();
		void Update(float dt);
		void Init();
		void CleanUp();
	
		void SetDeformable(bool deformable){this->deformable = deformable;}
		void SetWorld(D3DXMATRIX world){this->world = world;}

//		SurfelObject GetSurfelObject(){return surfelObject;}

		Volume* GetVolume(){
			if(volume)
				return volume;

			return tree->GetComputedVolume();
		}

		D3DXVECTOR3 Position(){return position;}
		std::string GetName(){return name;}

	private:
		CSGTree *tree;
		Volume *volume;

		std::string pointCloudXmlFile;
		bool deformable;
		int pointCloudSize;
		std::string texture;
		std::string name;

		D3DXMATRIX world;
		D3DXVECTOR3 position;
	};
}

#endif

