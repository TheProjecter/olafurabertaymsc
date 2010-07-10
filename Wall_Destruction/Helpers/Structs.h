#ifndef SURFELS_H
#define SURFELS_H

#include <D3DX10.h>
#include <string>
#include <vector>
#include "HavokPhysicsInclude.h"
#include "ThreeInOneArray.h"

namespace ProjectStructs{
// forward declarations
	struct SURFEL;
	struct SURFEL_EDGE;
	struct SURFEL_VERTEX;
	struct SURFEL_EDGE_VERTEX;


	struct NameComparer {
		bool operator()(std::string s1, std::string s2) const {
			return s1 < s2;
		}
	};

	enum DIRECTIONS  {FRONT_BOTTOM_LEFT, FRONT_BOTTOM_MIDDLE, FRONT_BOTTOM_RIGHT, 
		FRONT_CENTER_LEFT, FRONT_CENTER_MIDDLE, FRONT_CENTER_RIGHT, 
		FRONT_TOP_LEFT, FRONT_TOP_MIDDLE, FRONT_TOP_RIGHT, 
		BOTTOM_LEFT, BOTTOM_MIDDLE, BOTTOM_RIGHT, 
		CENTER_LEFT, THIS_NODE,  CENTER_RIGHT, 
		TOP_LEFT, TOP_MIDDLE, TOP_RIGHT, 
		BACK_BOTTOM_LEFT, BACK_BOTTOM_MIDDLE, BACK_BOTTOM_RIGHT, 
		BACK_CENTER_LEFT, BACK_CENTER_MIDDLE, BACK_CENTER_RIGHT, 
		BACK_TOP_LEFT, BACK_TOP_MIDDLE, BACK_TOP_RIGHT, };

	struct PHYXEL_NODE{
		D3DXVECTOR3 pos;
		float radius;
		float mass;
		D3DXVECTOR3 force;
	};

	struct Phyxel_Grid_Cell{
		ThreeInOneArray<Phyxel_Grid_Cell*> neighbours;
		std::vector<SURFEL*> surfels;
		std::vector<SURFEL_EDGE*> edges;
		PHYXEL_NODE* phyxel;
		D3DXVECTOR3 halfWidth;

		Phyxel_Grid_Cell(){
			phyxel = NULL;

			neighbours = ThreeInOneArray<Phyxel_Grid_Cell*>(3, 3, 3);
			for(int i = 0; i<neighbours.GetSize(); i++){
				neighbours[i] = NULL;
			}
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

	struct SURFEL{
		std::vector<Phyxel_Grid_Cell*> intersectingCells;
		SURFEL_VERTEX vertex;

		SURFEL(D3DXVECTOR3 pos, D3DXVECTOR3 normal, D3DXVECTOR3 majorAxis, D3DXVECTOR3 minorAxis, D3DXVECTOR2 UV){
			this->vertex = SURFEL_VERTEX(pos, normal, majorAxis, minorAxis, UV);
			/*intersectingCells = std::vector<Phyxel_Grid_Cell*>(5);*/
		}

		SURFEL(){}
	};

	struct SURFEL_EDGE{
		std::vector<Phyxel_Grid_Cell*> intersectingCells;
		SURFEL_EDGE_VERTEX vertex;
		
		SURFEL_EDGE(D3DXVECTOR3 pos, D3DXVECTOR3 normal, D3DXVECTOR3 majorAxis, D3DXVECTOR3 minorAxis, D3DXVECTOR3 clipPlane, D3DXVECTOR2 UV){
			this->vertex = SURFEL_EDGE_VERTEX(pos, normal, majorAxis, minorAxis, clipPlane, UV);
			/*intersectingCells = std::vector<Phyxel_Grid_Cell*>(5);*/
		}

		SURFEL_EDGE(){}
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
		ProjectStructs::SURFEL_VERTEX surfel;
		ProjectStructs::SURFEL_EDGE_VERTEX surfelEdge;
		D3DXVECTOR3 halfDimensions;
		D3DXVECTOR3 pos;
		bool edge;
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