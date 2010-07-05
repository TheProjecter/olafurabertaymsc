#ifndef SURFELS_H
#define SURFELS_H

#include <D3DX10.h>
#include <string>
#include <vector>
#include "HavokPhysicsInclude.h"
//#include "CSGTree.h"


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
		D3DXVECTOR3 majorAxis;
		D3DXVECTOR3 minorAxis;
		D3DXVECTOR2 UV;

		SURFEL_VERTEX(D3DXVECTOR3 pos, D3DXVECTOR3 normal, D3DXVECTOR3 majorAxis, D3DXVECTOR3 minorAxis, D3DXVECTOR2 UV){
			this->pos = pos;
			this->normal = normal;
			this->minorAxis = minorAxis;
			this->majorAxis = majorAxis;			
			this->UV = UV;
		}

		SURFEL_VERTEX(){
		}
	};


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
	struct SURFEL_EDGE_VERTEX{
		// position
		D3DXVECTOR3 pos;
		// normal
		D3DXVECTOR3 normal;
		D3DXVECTOR3 majorAxis;
		D3DXVECTOR3 minorAxis;
		D3DXVECTOR3 clipPlane;
		D3DXVECTOR2 UV;

		SURFEL_EDGE_VERTEX(D3DXVECTOR3 pos, D3DXVECTOR3 normal, D3DXVECTOR3 majorAxis, D3DXVECTOR3 minorAxis, D3DXVECTOR3 clipPlane, D3DXVECTOR2 UV){
			this->pos = pos;
			this->normal = normal;
			this->minorAxis = minorAxis;
			this->majorAxis = majorAxis;			
			this->clipPlane = clipPlane;
			this->UV = UV;
		}

		SURFEL_EDGE_VERTEX(){
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

	struct POINT_CLOUD_VERTEX {
		D3DXVECTOR3 pos;
		D3DXVECTOR3 normal;
	};

	struct PHYXEL_NODE{
		D3DXVECTOR3 pos;
		float radius;
		float mass;
	};

	struct SIMPLE_VERTEX{
		D3DXVECTOR3 pos;
		float supportRadius;
		float mass;
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

	// Octree
	struct SurfelNode{
		Structs::SURFEL_VERTEX surfel;;
		SurfelNode *nextSurfel;
	} ;

	struct OctreeNode {
		D3DXVECTOR3 center; // Center point of octree node 
		float halfWidth; // Half the width of the node volume
		
		OctreeNode *pChild[8]; // Pointers to the eight children nodes
		OctreeNode *parent;
		SurfelNode *pObjList; // Linked list of objects contained at this node

		bool childrenContainObjects;
		bool checked;
	};
}

#endif