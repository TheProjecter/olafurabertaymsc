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
		surfelObject.CleanUp();
	}

	void MeshlessObject::Draw(){

		this->surfelObject.Draw();
	}

	void MeshlessObject::Update(float dt){
	}

	void MeshlessObject::Init(){
		Structs::MESHLESS_OBJECT_STRUCT meshlessObjectStruct = PointCloud::PointCloudHandler::ProcessPointCloud(pointCloudXmlFile);
		this->texture = meshlessObjectStruct.texture;
		this->deformable = meshlessObjectStruct.deformable;
		this->name = meshlessObjectStruct.name;
		this->world = meshlessObjectStruct.world;
		this->position = meshlessObjectStruct.transform;

		this->surfelObject = SurfelObject(meshlessObjectStruct.surfels);
		this->surfelObject.SetSurfelsSolidTexture(texture);
		this->surfelObject.SetMaterialInfo(meshlessObjectStruct.sigma, meshlessObjectStruct.rho);
		this->surfelObject.SetWorld(this->world);
		this->surfelObject.Init();

		Helpers::ObjectHelper::MeshlessObjects[this->name] = this;		

		PhysicsWrapper::AddMeshlessObject(this);
	}

	void MeshlessObject::ResetBuffers()
	{
		this->surfelObject.ResetSurfels();
	}
}