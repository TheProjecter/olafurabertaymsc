#ifndef MESHLESS_OBJECT_H
#define MESHLESS_OBJECT_H

#include "SurfelObject.h"
#include "Drawable.h"
#include "Globals.h"
#include "HavokPhysicsInclude.h"

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

		void SetRigidBody(hkpRigidBody* rigid){rigidBodies.push_back(rigid);}
		hkpRigidBody* GetRigidBody(){return rigidBodies[0];}

		void SetDeformable(bool deformable){this->deformable = deformable;}
		void SetWorld(D3DXMATRIX world){this->world = world;}
		void ResetBuffers();

		SurfelObject GetSurfelObject(){return surfelObject;}
		D3DXVECTOR3 Position(){return position;}
		std::string GetName(){return name;}

	private:
		SurfelObject surfelObject;
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
