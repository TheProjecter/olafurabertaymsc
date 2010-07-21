#ifndef MESHLESS_OBJECT_H
#define MESHLESS_OBJECT_H

#include "PhyxelObject.h"
#include "Drawable.h"
#include "Globals.h"
#include "HavokPhysicsInclude.h"
#include "Volume.h"
#include "CSGTree.h"
#include "ContactListener.h"

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

		void AddContactListener(ContactListener *cl){contactListeners.push_back(cl);}
		void AddRigidBody(hkpRigidBody* rigid){rigidBodies.push_back(rigid);}
		std::vector<hkpRigidBody*> GetRigidBodies(){return rigidBodies;}

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
//		SurfelObject surfelObject;
		PhyxelObject phyxelObject;

		CSGTree *tree;
		Volume *volume;

		std::string pointCloudXmlFile;
		bool deformable;
		int pointCloudSize;
		std::string texture;
		std::string name;

		D3DXMATRIX world;
		D3DXVECTOR3 position;

		std::vector<hkpRigidBody*> rigidBodies;
		std::vector<ContactListener*> contactListeners;
	};
}

#endif

