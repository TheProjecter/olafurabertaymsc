#ifndef POINT_CLOUD_HANDLER_H
#define POINT_CLOUD_HANDLER_H

#include <D3DX10.h>
#include <string>
#include <vector>

#include "Structs.h"
#include "ThreeInOneArray.h"
#include "MeshlessObject.h"
#include "Volume.h"
#include "Surface.h"
#include "CSGTree.h"

#include "tinyxml.h"

namespace PointCloud{
	class PointCloudHandler
	{
	public:
		// Processing methods return an array of surfel vertices

		// Load a xml file
		//static std::vector<ThreeInOneArray<Structs::POINT_CLOUD_VERTEX>> ProcessPointCloud(std::string file, std::string *textureFile);
		static Structs::MESHLESS_OBJECT_STRUCT ProcessPointCloud(std::string file, CSGTree *&surfaceTree, Volume *&surface);


		//static Structs::SURFEL_VERTEX* ProcessPointCloud(ThreeInOneArray<Structs::POINT_CLOUD_VERTEX> points);
	private:
		static Drawables::Surface* CreateSurfelsFromPlane(D3DXVECTOR3 position, D3DXVECTOR3 normal, D3DXVECTOR2 dimensions, float radius);
		static void CreateSurface( Volume * v, D3DXVECTOR3 pos, D3DXVECTOR3 majorAxis, D3DXVECTOR3 minorAxis, D3DXVECTOR3 normal, int U, int V, std::string texture);

		static D3DXVECTOR3 GetVector3(TiXmlElement *element);
		static D3DXVECTOR2 GetVector2(TiXmlElement *element);

		//static ThreeInOneArray<Structs::POINT_CLOUD_VERTEX> CreateSurfelPlane(D3DXVECTOR3 from, D3DXVECTOR2 fromUV, D3DXVECTOR3 to, D3DXVECTOR2 toUV, D3DXVECTOR3 normal, D3DXVECTOR3 pointstep);

		static const string XML_MESHLESS_ROOT;
		static const string XML_MESHLESS_NAME;
		static const string XML_MESHLESS_DEFORMABLE;
		static const string XML_MESHLESS_TEXTURE;
		static const string XML_MESHLESS_WORLDTRANSLATION;
		static const string XML_MESHLESS_WORLDSCALE;
		static const string XML_MESHLESS_WORLDYAWPITCHROLL;
		static const string XML_MESHLESS_MATERIAL;

		static const string XML_MESHLESS_VOLUME;
		static const string XML_MESHLESS_POINTSET;
		static const string XML_MESHLESS_POSITION;
		static const string XML_MESHLESS_DIMENSIONS;
		static const string XML_MESHLESS_NORMAL;

		static const string XML_MESHLESS_X;
		static const string XML_MESHLESS_Y;
		static const string XML_MESHLESS_Z;
		static const string XML_MESHLESS_U;
		static const string XML_MESHLESS_V;
		static const string XML_MESHLESS_TRUE;
		static const string XML_MESHLESS_FALSE;
		static const string XML_MESHLESS_MATERIAL_SIGMA;
		static const string XML_MESHLESS_MATERIAL_RHO;
		static const string XML_MESHLESS_SURFEL_RADIUS;

		static const string XML_MESHLESS_SURFEL;
		static const string XML_MESHLESS_SURFEL_POSITION;
		static const string XML_MESHLESS_SURFEL_MAJOR_AXIS;
		static const string XML_MESHLESS_SURFEL_MINOR_AXIS;
		static const string XML_MESHLESS_SURFEL_NORMAL;
		static const string XML_MESHLESS_SURFEL_COUNT;

	};
}

#endif