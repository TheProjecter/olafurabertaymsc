#include "MeshlessObject.h"
#include "PointCloudHandler.h"
#include "Globals.h"
#include "ObjectHelper.h"
#include "PhysicsWrapper.h"

namespace Drawables{
	MeshlessObject::MeshlessObject(std::string pointCloudXmlFile)
	{
		this->pointCloudXmlFile = pointCloudXmlFile;
		this->texture = "";
		Init();
	}

	MeshlessObject::~MeshlessObject(void)
	{
	}

	void MeshlessObject::CleanUp(){
//		surfelObject.CleanUp();
		if(deformable){
			phyxelObject.CleanUp();
		}

		if(volume){
			volume->CleanUp();
			delete volume;
		}		
		if(tree){
			tree->CleanUp();
			delete tree;
		}
		for(unsigned int i = 0; i< contactListeners.size();i++){
			delete contactListeners[i];
		}

		contactListeners.clear();
		contactListeners.swap( std::vector<ContactListener*>() );

		rigidBodies.clear();
		rigidBodies.swap( std::vector<hkpRigidBody*>() );
	}


	void MeshlessObject::Draw(){
//		if(volume)
			volume->Draw();

		if(deformable){
			this->phyxelObject.Draw();
		}
	}

	void MeshlessObject::Update(float dt){
		volume->Update(dt);
	}

	void MeshlessObject::Init(){

		tree = NULL;
		volume = NULL;
		
		Helpers::Globals::DebugInformation.StartTimer();
		
		ProjectStructs::MESHLESS_OBJECT_STRUCT meshlessObjectStruct = PointCloud::PointCloudHandler::ProcessPointCloud(pointCloudXmlFile, *&tree, *&volume);

		Helpers::Globals::DebugInformation.EndTimer(D3DXCOLOR(0.0f, 1.0f, 0.0f, 0.0f), "%s - Ran the Pointcloud handler. ", meshlessObjectStruct.name.c_str());

		this->texture = meshlessObjectStruct.texture;
		this->deformable = meshlessObjectStruct.deformable;
		this->name = meshlessObjectStruct.name;
		this->world = meshlessObjectStruct.world;
		this->position = meshlessObjectStruct.transform;

		Helpers::Globals::DebugInformation.StartTimer();
		PhysicsWrapper::AddMeshlessObject(this);
		
		Helpers::Globals::DebugInformation.EndTimer(D3DXCOLOR(0.0f, 1.0f, 0.0f, 0.0f), "%s - Added the meshless object into Havok", meshlessObjectStruct.name.c_str(), meshlessObjectStruct.surfels.size());
	}
}
