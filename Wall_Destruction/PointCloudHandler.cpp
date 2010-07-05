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


	Structs::MESHLESS_OBJECT_STRUCT PointCloudHandler::ProcessPointCloud( std::string file, CSGTree *&volumeTree , Volume *&volume)
	{		
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

	//	do{
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

			volume = v;
			volume->Init();
			return meshlessStruct;
			
	//		volumes.push_back(v);

	//		volumeNode = volumeNode->NextSibling(XML_MESHLESS_VOLUME);

	//	}while(volumeNode);
/*
		if(volumes.size() == 1)
		{
			volume = volumes[0];
			volume->Init();

			volumes.clear();
			return meshlessStruct;
		}
		else if(volumes.size() == 2)
		{
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
		return meshlessStruct;*/
	}

	Drawables::Surface* PointCloudHandler::CreateSurfelsFromPlane( D3DXVECTOR3 position, D3DXVECTOR3 normal, D3DXVECTOR2 dimensions, float radius )
	{
/*		D3DXVec3Normalize(&normal, &normal);
		D3DXVECTOR3 tan1 = MathHelper::Perpendicular(normal);
		D3DXVec3Normalize(&tan1, &tan1);
		D3DXVECTOR3 tan2;
		D3DXVec3Cross(&tan2, &normal, &tan1);
		D3DXVec3Normalize(&tan1, &tan1);

		float xMultiplier[] = {-1.0f, -1.0f, 1.0f, 1.0f};
		float yMultiplier[] = {-1.0f, 1.0f, -1.0f, 1.0f};

		float width, height;
		width = dimensions.x / (radius);
		height = dimensions.y / (radius);
		D3DXVECTOR2 ratio = D3DXVECTOR2(1.0f/(width+1), 1.0f/(height+1));
		
		std::vector<Structs::SURFEL_VERTEX> surfaceVertices;
		std::vector<Structs::SURFEL_VERTEX> edgeVertices;


		for(float i = 0.0f; i<=width; i++){
			for(float j = 0.0f; j<=height; j++){
				Structs::SURFEL_VERTEX sv;
				sv.pos = position + (i) * radius * tan2 + (j) * radius * tan1 - dimensions.x * 0.5f * tan2 - dimensions.y * 0.5f * tan1;// - radius * 0.5f * tan2 - radius * 0.5 * tan1;
				sv.normal = normal;
				sv.radius = radius;
				sv.ratio = ratio;
				sv.UV = D3DXVECTOR2((i)*ratio.x, (j)*ratio.y);

				if(i == 0.0f || j == 0.0f || i == width || j == height){
					edgeVertices.push_back(sv);
				}
				else{
					surfaceVertices.push_back(sv);
				}
			}
		}

		Structs::SURFEL_VERTEX *surfelVertices = NULL;
		Structs::SURFEL_VERTEX *edgeSurfelVertices = NULL;

		edgeSurfelVertices = new Structs::SURFEL_VERTEX[edgeVertices.size()];
		surfelVertices = new Structs::SURFEL_VERTEX[surfaceVertices.size()];

		for(unsigned int i =0 ;i<surfaceVertices.size(); i++)
			surfelVertices[i] = surfaceVertices[i];

		for(unsigned int i =0 ;i<edgeVertices.size(); i++)
			edgeSurfelVertices[i] = edgeVertices[i];

		Drawables::Surface* surface = new Drawables::Surface();
		surface->SetSurfaceSurfels(surfelVertices, surfaceVertices.size());
		surface->SetEdgeSurfels(edgeSurfelVertices, edgeVertices.size());

		return surface;*/
		return NULL;
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

	void PointCloudHandler::CreateSurface( Volume * v, D3DXVECTOR3 pos, D3DXVECTOR3 majorAxis, D3DXVECTOR3 minorAxis, D3DXVECTOR3 normal, int U, int V, std::string texture)
	{
		Drawables::Surface *surface = new Drawables::Surface();
		surface->SetPosition(pos);
		
		D3DXVECTOR2 delta = D3DXVECTOR2(0.5f/(float)U, 0.5f/(float)V);
		surface->SetDeltaSurfelUV(delta);

		//surface->AddSurfel(Structs::SURFEL_VERTEX(pos + majorAxis + minorAxis, normal, majorAxis, minorAxis, D3DXVECTOR2(U, V)));

		
		/*
			Clip planes can be
			(1, -1, 0)		(-1, -1, 0)		(1, 1, 0)		(-1, 1, 0)	   (0, 1, 0)		(1, 0, 0)
			 _ _			 _ _				 _			    _			   _
			|  _|			|_	|				| |_		  _| |			  | |			  _ _
			|_| 			  |_|				|_ _|	   	 |_ _|			  |_|			 |_ _|

			(1, -1, 1)		(-1, -1, 1)		(1, 1, 1)		(-1, 1, 1)	   (0, 1, 1)		(1, 0, 1)
			    			 				       _		  _				 _				  _ _
			   _			 _					  |_|		 |_| 			| |				 |_ _|
			  |_| 			|_|   			     	   	     				|_|				 

		*/
		/*surface->AddEdgeSurfel(Structs::SURFEL_EDGE_VERTEX(pos, normal, majorAxis, minorAxis, D3DXVECTOR3(1.0f, 0.0f, 1.0f), 
			D3DXVECTOR2(3*delta.x , 3*delta.y )));*/


		for(int i = -U+1; i<=U-1; i++){
			for (int j = -V+1; j <= V-1; j++)
			{
				surface->AddSurfel(Structs::SURFEL_VERTEX(pos + i*majorAxis + j*minorAxis , normal, majorAxis, minorAxis, D3DXVECTOR2(delta.x * (i+U), delta.y *(j+V))));
			}
		}

		// edges
		for(int i = -U+1; i<=U-1; i++){
			surface->AddEdgeSurfel(Structs::SURFEL_EDGE_VERTEX(pos + i*majorAxis - V*minorAxis, normal, majorAxis, minorAxis, D3DXVECTOR3(1.0f, 0.0f, 1.0f), 
				D3DXVECTOR2(delta.x * (i+U), 0.0)));
			surface->AddEdgeSurfel(Structs::SURFEL_EDGE_VERTEX(pos + i*majorAxis + V*minorAxis, normal, majorAxis, minorAxis, D3DXVECTOR3(1.0f, 0.0f, 0.0f),
				D3DXVECTOR2(delta.x * (i+U), V )));
		}

		for(int i = -V+1; i<=V-1; i++){
			surface->AddEdgeSurfel(Structs::SURFEL_EDGE_VERTEX(pos - U*majorAxis + i*minorAxis, normal, majorAxis, minorAxis, D3DXVECTOR3(0.0f, 1.0f, 1.0f), 
				D3DXVECTOR2(0.0f, delta.y * (i+V))));
			surface->AddEdgeSurfel(Structs::SURFEL_EDGE_VERTEX(pos + U*majorAxis + i*minorAxis, normal, majorAxis, minorAxis, D3DXVECTOR3(0.0f, 1.0f, 0.0f),
				D3DXVECTOR2(U, delta.y * (i+V))));
		}

		//corners
		surface->AddEdgeSurfel(Structs::SURFEL_EDGE_VERTEX(pos + U*majorAxis + V*minorAxis, normal, majorAxis, minorAxis, D3DXVECTOR3(-1.0f, -1.0f, 1.0f),
			D3DXVECTOR2(U, V)));
		surface->AddEdgeSurfel(Structs::SURFEL_EDGE_VERTEX(pos - U*majorAxis + V*minorAxis, normal, majorAxis, minorAxis, D3DXVECTOR3(1.0f, -1.0f, 1.0f),
			D3DXVECTOR2(0.0f, V)));
		surface->AddEdgeSurfel(Structs::SURFEL_EDGE_VERTEX(pos + U*majorAxis - V*minorAxis, normal, majorAxis, minorAxis, D3DXVECTOR3(-1.0f, 1.0f, 1.0f),
			D3DXVECTOR2(U, 0.0f)));
		surface->AddEdgeSurfel(Structs::SURFEL_EDGE_VERTEX(pos - U*majorAxis - V*minorAxis, normal, majorAxis, minorAxis, D3DXVECTOR3(1.0f, 1.0f, 1.0f),
			D3DXVECTOR2(0.0f, 0.0f)));

		surface->SetTexture(texture);

		v->AddSurface(surface);
	}
}

