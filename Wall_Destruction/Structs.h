#ifndef SURFELS_H
#define SURFELS_H

#include <D3DX10.h>
#include <string>
#include <vector>
#include "HavokPhysicsInclude.h"


namespace Structs{

	struct NameComparer {
		bool operator()( std::string s1, std::string s2 ) const {
			return s1 < s2;
		}
	};

	struct SURFEL_VERTEX{
		// position
		D3DXVECTOR3 pos;
		// normal
		D3DXVECTOR3 normal;
		// dimensions.x : width
		// dimensions.y : height
		D3DXVECTOR2 dimensions;

		SURFEL_VERTEX(){
			this->dimensions = D3DXVECTOR2(0.0f, 0.0f);
			this->normal = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			this->pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		}

		SURFEL_VERTEX(D3DXVECTOR3 pos, D3DXVECTOR3 normal, D3DXVECTOR2 dim){
			this->dimensions = dim;
			this->normal = normal;
			this->pos = pos;
		}
	};

	struct SOLID_VERTEX{
		D3DXVECTOR3 pos;
		D3DXVECTOR3 normal;
		D3DXVECTOR2 UV;

		SOLID_VERTEX(D3DXVECTOR3 pos, D3DXVECTOR3 normal, D3DXVECTOR2 UV){
			this->UV = UV;
			this->normal = normal;
			this->pos = pos;
		}
		SOLID_VERTEX(){}
	};

	struct SIMULATION_NODE{
		D3DXVECTOR3 pos;
	};

	struct MESHLESS_OBJECT_STRUCT{
		std::string name;
		bool deformable;
		std::string texture;
		D3DXVECTOR3 transform;
		D3DXVECTOR3 scale;
		D3DXVECTOR3 rotation;
		D3DXMATRIX world;

		float rho;
		float sigma;

		std::vector<SURFEL_VERTEX> surfels;
	};

	struct PROJECTILE{
		D3DXMATRIX world;
		D3DXVECTOR3 velocity;
		D3DXVECTOR3 position;
		float life;
		hkpRigidBody* rigidBody;
	};

	struct CAMERA_DIRECTION_LINE_STRUCT{
		D3DXVECTOR3 position;
		bool IsNearCamera;
	};
}

#endif