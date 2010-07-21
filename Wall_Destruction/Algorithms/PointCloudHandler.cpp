#include "PointCloudHandler.h"

#include "tinyxml.h"
#include "Globals.h"
#include "MathHelper.h"
#include <ctime>

namespace PointCloud{
	
	const string PointCloudHandler::XML_MESHLESS_ROOT = "meshless_object";
	const string PointCloudHandler::XML_MESHLESS_NAME = "name";
	const string PointCloudHandler::XML_MESHLESS_DEFORMABLE = "deformable";
	const string PointCloudHandler::XML_MESHLESS_TEXTURE = "texture";
	const string PointCloudHandler::XML_MESHLESS_WORLDTRANSLATION = "worldtranslation";
	const string PointCloudHandler::XML_MESHLESS_WORLDSCALE = "worldscale";
	const string PointCloudHandler::XML_MESHLESS_WORLDYAWPITCHROLL = "worldyawpitchroll";

	const string PointCloudHandler::XML_MESHLESS_MATERIAL = "material";
	const string PointCloudHandler::XML_MESHLESS_DENSITY = "density";
	const string PointCloudHandler::XML_MESHLESS_YOUNGS_MODULUS = "youngs_modulus";
	const string PointCloudHandler::XML_MESHLESS_POISSON_RATIO = "poisson_ratio";
	const string PointCloudHandler::XML_MESHLESS_DAMPING_CONSTANT_PHI = "damping_constant_phi";
	const string PointCloudHandler::XML_MESHLESS_DAMPING_CONSTANT_PSI = "damping_constant_psi";
	const string PointCloudHandler::XML_MESHLESS_TOUGHNESS = "toughness";
	
	const string PointCloudHandler::XML_MESHLESS_VOLUME = "volume";
	const string PointCloudHandler::XML_MESHLESS_POINTSET = "pointset";
	const string PointCloudHandler::XML_MESHLESS_POSITION = "position";
	const string PointCloudHandler::XML_MESHLESS_DIMENSIONS = "dimensions";
	const string PointCloudHandler::XML_MESHLESS_NORMAL = "normal";
	
// attributes
	const string PointCloudHandler::XML_MESHLESS_X = "x";
	const string PointCloudHandler::XML_MESHLESS_Y = "y";
	const string PointCloudHandler::XML_MESHLESS_Z = "z";
	const string PointCloudHandler::XML_MESHLESS_U = "u";
	const string PointCloudHandler::XML_MESHLESS_V = "v";
	const string PointCloudHandler::XML_MESHLESS_TRUE = "true";
	const string PointCloudHandler::XML_MESHLESS_FALSE = "false";
	const string PointCloudHandler::XML_MESHLESS_MATERIAL_SIGMA = "sigma";
	const string PointCloudHandler::XML_MESHLESS_MATERIAL_RHO = "rho";	
	const string PointCloudHandler::XML_MESHLESS_SURFEL_RADIUS = "radius";

	// surfel
	const string PointCloudHandler::XML_MESHLESS_SURFEL = "surfel";
	const string PointCloudHandler::XML_MESHLESS_SURFEL_POSITION = "position";
	const string PointCloudHandler::XML_MESHLESS_SURFEL_MAJOR_AXIS = "majorAxis";
	const string PointCloudHandler::XML_MESHLESS_SURFEL_MINOR_AXIS = "minorAxis";
	const string PointCloudHandler::XML_MESHLESS_SURFEL_NORMAL = "normal";
	const string PointCloudHandler::XML_MESHLESS_SURFEL_COUNT = "count";


	ProjectStructs::MESHLESS_OBJECT_STRUCT PointCloudHandler::ProcessPointCloud( std::string file, CSGTree *&volumeTree , Volume *&volume)
	{		
		TiXmlDocument pointCloudXml;

		if(!pointCloudXml.LoadFile(file.c_str())){
			std::string errorText = "Could not parse " + file;			
			std::string errorHeader = "Point Cloud Error";

			MessageBox(0, errorText.c_str(), errorHeader.c_str(), 0);             
		}

		ProjectStructs::MESHLESS_OBJECT_STRUCT meshlessStruct;

		TiXmlNode *meshNode = pointCloudXml.FirstChild(XML_MESHLESS_ROOT);
		
		meshlessStruct.name = meshNode->FirstChild(XML_MESHLESS_NAME)->FirstChild()->Value();
		meshlessStruct.materialProperties.deformable = meshNode->FirstChild(XML_MESHLESS_DEFORMABLE)->FirstChild()->Value() == XML_MESHLESS_TRUE;
		meshlessStruct.texture= meshNode->FirstChild(XML_MESHLESS_TEXTURE)->FirstChild()->Value();

		D3DXVECTOR3 translation = GetVector3(meshNode->FirstChild(XML_MESHLESS_WORLDTRANSLATION)->ToElement());
		D3DXVECTOR3 scale = GetVector3(meshNode->FirstChild(XML_MESHLESS_WORLDSCALE)->ToElement());
		D3DXVECTOR3 yawpitchroll= GetVector3(meshNode->FirstChild(XML_MESHLESS_WORLDYAWPITCHROLL)->ToElement());

		double d;
		meshNode->FirstChild(XML_MESHLESS_MATERIAL)->ToElement()->Attribute(XML_MESHLESS_MATERIAL_RHO.c_str(), &d);
		meshlessStruct.rho = (float)d;
		meshNode->FirstChild(XML_MESHLESS_MATERIAL)->ToElement()->Attribute(XML_MESHLESS_MATERIAL_SIGMA.c_str(), &d);
		meshlessStruct.sigma = (float)d;

		if(meshlessStruct.materialProperties.deformable){
			meshlessStruct.materialProperties.density = (float)atof(meshNode->FirstChild(XML_MESHLESS_DENSITY)->FirstChild()->Value());
			meshlessStruct.materialProperties.toughness = (float)atof(meshNode->FirstChild(XML_MESHLESS_TOUGHNESS)->FirstChild()->Value());
			meshlessStruct.materialProperties.youngsModulus = (float)atof(meshNode->FirstChild(XML_MESHLESS_YOUNGS_MODULUS)->FirstChild()->Value());// * pow(10.0f, 5.0f);
			meshlessStruct.materialProperties.poissonRatio = (float)atof(meshNode->FirstChild(XML_MESHLESS_POISSON_RATIO)->FirstChild()->Value());
		}

		meshlessStruct.transform = translation;

		D3DXMATRIX translationMatrix, scaleMatrix, yawpitchrollMatrix;
		D3DXMatrixTranslation(&translationMatrix, translation.x, translation.y, translation.z);
		D3DXMatrixScaling(&scaleMatrix, scale.x, scale.y, scale.z);
		D3DXMatrixRotationYawPitchRoll(&yawpitchrollMatrix, yawpitchroll.x, yawpitchroll.y, yawpitchroll.z);
		meshlessStruct.scale = scale;
		meshlessStruct.rotation= yawpitchroll;

		meshlessStruct.world = scaleMatrix * yawpitchrollMatrix * translationMatrix;
		
		TiXmlNode* volumeNode = meshNode->FirstChild(XML_MESHLESS_VOLUME);

		TiXmlNode* surfel;

		std::vector<Volume*> volumes;

		do{
			surfel = volumeNode->FirstChild(XML_MESHLESS_SURFEL);
			Volume *v = new Volume();
			v->SetWorld(meshlessStruct.world);

			do{
				D3DXVECTOR3 pos = GetVector3(surfel ->FirstChild(XML_MESHLESS_SURFEL_POSITION)->ToElement());
				D3DXVECTOR3 majorAxis = GetVector3(surfel ->FirstChild(XML_MESHLESS_SURFEL_MAJOR_AXIS)->ToElement());
				D3DXVECTOR3 minorAxis = GetVector3(surfel ->FirstChild(XML_MESHLESS_SURFEL_MINOR_AXIS)->ToElement());
				D3DXVECTOR3 normal = GetVector3(surfel ->FirstChild(XML_MESHLESS_SURFEL_NORMAL)->ToElement());
				D3DXVECTOR2 count = GetVector2(surfel ->FirstChild(XML_MESHLESS_SURFEL_COUNT)->ToElement());

				CreateSurface(v, pos, majorAxis, minorAxis, normal, count.x, count.y, meshlessStruct.texture);

				surfel = surfel->NextSibling(XML_MESHLESS_SURFEL);
			
			}while(surfel);

			v->Init(meshlessStruct.materialProperties);
			
			volumes.push_back(v);

			volumeNode = volumeNode->NextSibling(XML_MESHLESS_VOLUME);

		}while(volumeNode);

		if(volumes.size() == 1)
		{
			volume = volumes[0];

			volumes.clear();
			return meshlessStruct;
		}
		else if(volumes.size() == 2)
		{
			volumeTree = new CSGTree();
			volumeTree->CreateTree(volumeTree->And(volumes[0], volumes[1]));
			volumes.clear();
			return meshlessStruct;
		}

		volumeTree = new CSGTree();

		std::vector<OPERATOR_NODE*> one, two;

		bool done = false;
		int size = volumes.size();

		for(unsigned int i = 0; i<volumes.size()-1; i+=2){
			one.push_back(volumeTree->And(volumes[i], volumes[i+1]));
		}
		if(volumes.size() % 2 == 1){
			one[one.size()-1] = volumeTree->And(one[one.size()-1], volumes[volumes.size()-1]);
		}
		
		while(!done){
			two.clear();

			for(unsigned int i = 0; i<one.size()-1; i+=2){
				two.push_back(volumeTree->And(one[i], one[i+1]));
			}
			if(one.size() % 2 == 1){
				two[two.size()-1] = volumeTree->And(two[two.size()-1], one[one.size()-1]);
			}

			if(one.size() == 2){
				volumeTree->CreateTree(volumeTree->And(one[0], one[1]));
				break;
			}
			if(two.size() == 2){
				volumeTree->CreateTree(volumeTree->And(two[0], two[1]));
				break;
			}
			if(one.size() == 1){
				volumeTree->CreateTree(one[0]);
				break;
			}
			if(two.size() == 1){
				volumeTree->CreateTree(two[0]);
				break;
			}

			one.clear();

			for(unsigned int i = 0; i<two.size()-1; i+=2)
				one.push_back(volumeTree->And(two[i], two[i+1]));
			
			if(two.size() % 2 == 1){
				one[one.size()-1] = volumeTree->And(one[one.size()-1], two[two.size()-1]);
			}
		}
		
		volumes.clear();
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

	void PointCloudHandler::CreateSurface( Volume * v, D3DXVECTOR3 pos, D3DXVECTOR3 majorAxis, D3DXVECTOR3 minorAxis, D3DXVECTOR3 normal, float U, float V, std::string texture)
	{
		Drawables::Surface *surface = new Drawables::Surface();
		surface->SetPosition(pos);
		
		D3DXVECTOR2 delta = D3DXVECTOR2(0.5f/(float)U, 0.5f/(float)V);
		surface->SetDeltaSurfelUV(delta);

		D3DXVECTOR3 calculatedNormal;
		D3DXVec3Cross(&calculatedNormal, &minorAxis, &majorAxis);

		for(float i = -U+1; i<=U-1; i++)
		{
			for (float j = -V+1; j <= V-1; j++)
			{
				surface->AddSurfel(ProjectStructs::StructHelper::CreateSurfelPointer(pos + i*majorAxis + j*minorAxis , normal, majorAxis, minorAxis, D3DXVECTOR2(delta.x * (i+U), delta.y *(j+V))));
			}
		}

		// edges
		// Top - bottom - works
		for(float i = -U+1; i<=U-1; i++)
		{
			surface->AddEdgeSurfel(ProjectStructs::StructHelper::CreateSurfelEdgePointer(pos + i*majorAxis - V*minorAxis, normal, majorAxis, minorAxis, D3DXVECTOR3(1.0f, 0.0f, 1.0f ), 
				D3DXVECTOR2(delta.x * (i+U), 0.0)));

 			surface->AddEdgeSurfel(ProjectStructs::StructHelper::CreateSurfelEdgePointer(pos + i*majorAxis + V*minorAxis, normal, majorAxis, minorAxis, D3DXVECTOR3(1.0f, 0.0f, 0.0f),
 				D3DXVECTOR2(delta.x * (i+U), V )));
		}

		// left / right - works
		for(float i = -V+1; i<=V-1; i++)
		{
			surface->AddEdgeSurfel(ProjectStructs::StructHelper::CreateSurfelEdgePointer(pos - U*majorAxis + i*minorAxis, normal, majorAxis, minorAxis, D3DXVECTOR3(0.0f, 1.0f, 1.0f), 
				D3DXVECTOR2(0.0f, delta.y * (i+V))));
 			surface->AddEdgeSurfel(ProjectStructs::StructHelper::CreateSurfelEdgePointer(pos + U*majorAxis + i*minorAxis, normal, majorAxis, minorAxis, D3DXVECTOR3(0.0f, 1.0f, 0.0f),
				D3DXVECTOR2(U, delta.y * (i+V))));
		}

		//corners
		surface->AddEdgeSurfel(ProjectStructs::StructHelper::CreateSurfelEdgePointer(pos + U*majorAxis + V*minorAxis, normal, majorAxis, minorAxis, D3DXVECTOR3(-1.0f, -1.0f, 1.0f),
			D3DXVECTOR2(U, V)));
		surface->AddEdgeSurfel(ProjectStructs::StructHelper::CreateSurfelEdgePointer(pos - U*majorAxis + V*minorAxis, normal, majorAxis, minorAxis, D3DXVECTOR3(1.0f, -1.0f, 1.0f),
			D3DXVECTOR2(0.0f, V)));
		surface->AddEdgeSurfel(ProjectStructs::StructHelper::CreateSurfelEdgePointer(pos + U*majorAxis - V*minorAxis, normal, majorAxis, minorAxis, D3DXVECTOR3(-1.0f, 1.0f, 1.0f),
			D3DXVECTOR2(U, 0.0f)));
		surface->AddEdgeSurfel(ProjectStructs::StructHelper::CreateSurfelEdgePointer(pos - U*majorAxis - V*minorAxis, normal, majorAxis, minorAxis, D3DXVECTOR3(1.0f, 1.0f, 1.0f),
			D3DXVECTOR2(0.0f, 0.0f)));

		surface->SetTexture(texture);

		v->AddSurface(surface);
	}
}

