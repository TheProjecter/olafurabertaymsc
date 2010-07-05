#ifndef MESHLESS_OBJECT_H
#define MESHLESS_OBJECT_H

#include "SurfelObject.h"
#include "PhyxelObject.h"
#include "Drawable.h"
#include "Globals.h"
#include "HavokPhysicsInclude.h"
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
		void DrawDepth();
		void DrawAttributes(ID3D10ShaderResourceView* depth);
		void DrawNormalization(ID3D10ShaderResourceView* attributeMap);
		void Update(float dt);
		void Init();
		void CleanUp();

		void SetRigidBody(hkpRigidBody* rigid){rigidBodies.push_back(rigid);}
		hkpRigidBody* GetRigidBody(){return rigidBodies[0];}

		void SetDeformable(bool deformable){this->deformable = deformable;}
		void SetWorld(D3DXMATRIX world){this->world = world;}

//		SurfelObject GetSurfelObject(){return surfelObject;}

		Volume* GetVolume(){return volume;}

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

		ID3D10Buffer *pointVertexBuffer;
		Helpers::CustomEffect pointEffect;

		std::vector<hkpRigidBody*> rigidBodies;
	};
}

#endif
