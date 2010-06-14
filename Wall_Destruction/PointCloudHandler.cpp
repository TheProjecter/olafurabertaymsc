#include "PointCloudHandler.h"

#include "tinyxml.h"
#include "Globals.h"

namespace PointCloud{

	const string PointCloudHandler::XML_MESHLESS_ROOT = "meshless_object";
	const string PointCloudHandler::XML_MESHLESS_NAME = "name";
	const string PointCloudHandler::XML_MESHLESS_DEFORMABLE = "deformable";
	const string PointCloudHandler::XML_MESHLESS_POINTSET = "pointset";
	const string PointCloudHandler::XML_MESHLESS_POSITION = "position";
	const string PointCloudHandler::XML_MESHLESS_DIMENSIONS = "dimensions";
	const string PointCloudHandler::XML_MESHLESS_NORMAL = "normal";
	const string PointCloudHandler::XML_MESHLESS_TEXTURE = "texture";
	const string PointCloudHandler::XML_MESHLESS_WORLDTRANSLATION = "worldtranslation";
	const string PointCloudHandler::XML_MESHLESS_WORLDSCALE = "worldscale";
	const string PointCloudHandler::XML_MESHLESS_WORLDYAWPITCHROLL = "worldyawpitchroll";

	const string PointCloudHandler::XML_MESHLESS_MATERIAL = "material";
	const string PointCloudHandler::XML_MESHLESS_MATERIAL_SIGMA = "sigma";
	const string PointCloudHandler::XML_MESHLESS_MATERIAL_RHO = "rho";	

	const string PointCloudHandler::XML_MESHLESS_X = "x";
	const string PointCloudHandler::XML_MESHLESS_Y = "y";
	const string PointCloudHandler::XML_MESHLESS_Z = "z";
	const string PointCloudHandler::XML_MESHLESS_U = "u";
	const string PointCloudHandler::XML_MESHLESS_V = "v";
	const string PointCloudHandler::XML_MESHLESS_TRUE = "true";
	const string PointCloudHandler::XML_MESHLESS_FALSE = "false";

	Structs::MESHLESS_OBJECT_STRUCT PointCloudHandler::ProcessPointCloud(std::string file){		

		TiXmlDocument pointCloudXml;

		if(!pointCloudXml.LoadFile(file.c_str())){
			std::string errorText = "Could not parse " + file;			
			std::string errorHeader = "Point Cloud Error";

			MessageBox(0, errorText.c_str(), errorHeader.c_str(), 0);             
		}

		Structs::MESHLESS_OBJECT_STRUCT meshlessStruct;

		TiXmlNode *meshNode = pointCloudXml.FirstChild(XML_MESHLESS_ROOT);
		
		meshlessStruct.name = meshNode->FirstChild(XML_MESHLESS_NAME)->FirstChild()->Value();
		meshlessStruct.deformable = meshNode->FirstChild(XML_MESHLESS_DEFORMABLE)->FirstChild()->Value() == XML_MESHLESS_TRUE;
		meshlessStruct.texture= meshNode->FirstChild(XML_MESHLESS_TEXTURE)->FirstChild()->Value();

		D3DXVECTOR3 translation = GetVector3(meshNode->FirstChild(XML_MESHLESS_WORLDTRANSLATION)->ToElement());
		D3DXVECTOR3 scale = GetVector3(meshNode->FirstChild(XML_MESHLESS_WORLDSCALE)->ToElement());
		D3DXVECTOR3 yawpitchroll= GetVector3(meshNode->FirstChild(XML_MESHLESS_WORLDYAWPITCHROLL)->ToElement());

		double d;
		meshNode->FirstChild(XML_MESHLESS_MATERIAL)->ToElement()->Attribute(XML_MESHLESS_MATERIAL_RHO.c_str(), &d);
		meshlessStruct.rho = (float)d;
		meshNode->FirstChild(XML_MESHLESS_MATERIAL)->ToElement()->Attribute(XML_MESHLESS_MATERIAL_SIGMA.c_str(), &d);
		meshlessStruct.sigma = (float)d;
		
		D3DXMATRIX translationMatrix, scaleMatrix, yawpitchrollMatrix;
		D3DXMatrixTranslation(&translationMatrix, translation.x, translation.y, translation.z);
		D3DXMatrixScaling(&scaleMatrix, scale.x, scale.y, scale.z);
		D3DXMatrixRotationYawPitchRoll(&yawpitchrollMatrix, yawpitchroll.x, yawpitchroll.y, yawpitchroll.z);

		meshlessStruct.transform = translation;
		meshlessStruct.scale = scale;
		meshlessStruct.rotation= yawpitchroll;

		meshlessStruct.world = scaleMatrix * yawpitchrollMatrix * translationMatrix;

		TiXmlNode* pointset = meshNode->FirstChild(XML_MESHLESS_POINTSET);
		
		std::vector<Structs::SURFEL_VERTEX> surfels;

		do{
			D3DXVECTOR3 pos = GetVector3(pointset->FirstChild(XML_MESHLESS_POSITION)->ToElement());
			D3DXVECTOR2 dimensions = GetVector2(pointset->FirstChild(XML_MESHLESS_DIMENSIONS)->ToElement());
			D3DXVECTOR3 normal = GetVector3(pointset->FirstChild(XML_MESHLESS_NORMAL)->ToElement());

			surfels.push_back(Structs::SURFEL_VERTEX(pos, normal, dimensions));

			pointset = pointset->NextSibling(XML_MESHLESS_POINTSET);
		}while(pointset);

		meshlessStruct.surfels = surfels;
		return meshlessStruct;
	}

	D3DXVECTOR3 PointCloudHandler::GetVector3(TiXmlElement *element){
		D3DXVECTOR3 temp;
		double x, y, z;

		element->Attribute(XML_MESHLESS_X.c_str(), &x);
		element->Attribute(XML_MESHLESS_Y.c_str(), &y);
		element->Attribute(XML_MESHLESS_Z.c_str(), &z);

		temp = D3DXVECTOR3((float)x, (float)y, (float)z);

		return temp;
	}

	D3DXVECTOR2 PointCloudHandler::GetVector2(TiXmlElement *element){
		D3DXVECTOR2 temp;
		double u, v;

		element->Attribute(XML_MESHLESS_U.c_str(), &u);
		element->Attribute(XML_MESHLESS_V.c_str(), &v);

		temp = D3DXVECTOR2((float)u, (float)v);

		return temp;
	}
}

