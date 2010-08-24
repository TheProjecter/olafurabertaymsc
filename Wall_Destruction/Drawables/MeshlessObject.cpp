#include "MeshlessObject.h"
#include "PointCloudHandler.h"
#include "Globals.h"
#include "ObjectHelper.h"

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
		if(volume){
			volume->CleanUp();
			delete volume;
			volume = NULL;
		}		
		if(tree){
			tree->CleanUp();
			delete tree;
			tree = NULL;
		}
	}


	void MeshlessObject::Draw(){
		volume->Draw();
	}

	void MeshlessObject::Update(float dt){
		volume->Update(dt);
	}

	void MeshlessObject::Init(){

		tree = NULL;
		volume = NULL;
		
		Helpers::Globals::DebugInformation.StartTimer();
		
		ProjectStructs::MESHLESS_OBJECT_STRUCT meshlessObjectStruct = PointCloud::PointCloudHandler::ProcessPointCloud(pointCloudXmlFile, *&tree, *&volume);

		Helpers::Globals::DebugInformation.EndTimer(DEBUG_TYPE, D3DXCOLOR(0.0f, 1.0f, 0.0f, 0.0f), "%s - Ran the Pointcloud handler. ", meshlessObjectStruct.name.c_str());

		this->texture = meshlessObjectStruct.texture;
		this->name = meshlessObjectStruct.name;
		this->world = meshlessObjectStruct.world;
		this->position = meshlessObjectStruct.transform;
	}
}
