#ifndef SURFELS_H
#define SURFELS_H

#include <D3D10.h>
#include <D3DX10.h>
#include <string>
#include <vector>
#include "HavokPhysicsInclude.h"
#include "ThreeInOneArray.h"

namespace ProjectStructs{
// forward declarations
	struct CRACK_NODE;
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
		BACK_TOP_LEFT, BACK_TOP_MIDDLE, BACK_TOP_RIGHT};
/*
	struct PHYXEL_NODE{
		D3DXVECTOR3 pos;
		float supportRadius;
		float mass;
		float volume;
		float density;
*/
		/*   __________
			|ux  vx  wx|
			|uy  vy  wy|
			|uz  vz  wz|
			 ----------
		*/
/*		D3DXMATRIX momentMatrix;
		
		bool isChanged;
		D3DXVECTOR3 force;
		ThreeInOneArray<PHYXEL_NODE*> neighbours;
		ThreeInOneArray<float> neighbourWeight;
	};*/

	struct PHYXEL_NODE{
		D3DXVECTOR3 pos;
		D3DXVECTOR3 displacement;
		D3DXVECTOR3 dotDisplacement;
		float supportRadius;
		float mass;
		float volume;
		float density;

		/*   __________
			|ux  vx  wx|
			|uy  vy  wy|
			|uz  vz  wz|
			 ----------
		*/
		D3DXMATRIX displacementGradient;
		D3DXMATRIX momentMatrix;
		D3DXVECTOR3 bodyForce;
		D3DXMATRIX stress;
		D3DXMATRIX strain;
		D3DXMATRIX jacobian;

		bool isChanged;
		D3DXVECTOR3 force;
		ThreeInOneArray<PHYXEL_NODE*> neighbours;
		ThreeInOneArray<float> neighbourWeight;
	};

	struct Phyxel_Grid_Cell{
		ThreeInOneArray<Phyxel_Grid_Cell*> neighbours;
		//std::vector<CRACK_NODE*> cracks;
		std::vector<SURFEL*> surfels;
		std::vector<SURFEL_EDGE*> edges;
		PHYXEL_NODE* phyxel;
		D3DXVECTOR3 halfWidth;
	};

	struct Vertex_Grid_Cell{
		std::vector<CRACK_NODE*> cracks;
		std::vector<SURFEL*> surfels;
		std::vector<SURFEL_EDGE*> edges;
		ID3D10Buffer* buffer;
	};
	
	struct SURFEL_VERTEX{
		// position
		D3DXVECTOR3 pos;
		// normal
		D3DXVECTOR3 normal;
		D3DXVECTOR3 majorAxis;
		D3DXVECTOR3 minorAxis;
		D3DXVECTOR2 UV;
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
	};

	struct SURFEL{
		std::vector<Phyxel_Grid_Cell*> intersectingCells;
		SURFEL_VERTEX vertex;
		std::vector<int> vertexBufferGridCell;
	};

	struct SURFEL_EDGE{
		std::vector<Phyxel_Grid_Cell*> intersectingCells;
		SURFEL_EDGE_VERTEX vertex;
		std::vector<int> vertexBufferGridCell;
	};

	struct SPHERE_VERTEX{
		D3DXVECTOR3 pos;
		D3DXVECTOR3 normal;
		D3DXVECTOR2 UV;
	};

	struct SOLID_VERTEX{
		D3DXVECTOR3 pos;
		D3DXVECTOR3 normal;
		D3DXVECTOR2 UV;
		D3DXVECTOR2 EWAUV;
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

	struct CRACK_NODE{
		SURFEL s1;
		SURFEL s2;
		SURFEL_EDGE e1;
		SURFEL_EDGE e2;

		std::vector<int> vertexBufferGridCell;
	};

	struct MATERIAL_PROPERTIES{
		// material parameters
		bool deformable;
		float density;
		float toughness;
		float poissonRatio;
		float youngsModulus;
		float damping_constant_phi;
		float damping_constant_psi;
	};

	struct MESHLESS_OBJECT_STRUCT{
		std::string name;
		std::string texture;
		D3DXVECTOR3 transform;
		D3DXVECTOR3 scale;
		D3DXVECTOR3 rotation;
		D3DXMATRIX world;

		MATERIAL_PROPERTIES materialProperties;
		
		// lighting parameters
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
		D3DXVECTOR3 lastVelocity;
	};

	class StructHelper{

	public:
		static SOLID_VERTEX CreateSolidVertex(D3DXVECTOR3 pos, D3DXVECTOR3 normal, D3DXVECTOR2 UV, D3DXVECTOR2 EWAUV){
			SOLID_VERTEX solidVertex;
			solidVertex.pos = pos;
			solidVertex.normal = normal;
			solidVertex.UV = UV;
			solidVertex.EWAUV;
			return solidVertex;
		}

		static SOLID_VERTEX* CreateSolidVertexPointer(D3DXVECTOR3 pos, D3DXVECTOR3 normal, D3DXVECTOR2 UV, D3DXVECTOR2 EWAUV){
			SOLID_VERTEX* solidVertex = new SOLID_VERTEX;
			solidVertex->pos = pos;
			solidVertex->normal = normal;
			solidVertex->UV = UV;
			solidVertex->EWAUV;
			return solidVertex;
		}

		static SPHERE_VERTEX CreateSphereVertex(D3DXVECTOR3 pos, D3DXVECTOR3 normal, D3DXVECTOR2 UV){
			SPHERE_VERTEX sphereVertex;
			sphereVertex.pos = pos;
			sphereVertex.normal = normal;
			sphereVertex.UV;
			return sphereVertex;
		}

		static SURFEL_EDGE CreateSurfelEdge(D3DXVECTOR3 pos, D3DXVECTOR3 normal, D3DXVECTOR3 majorAxis, D3DXVECTOR3 minorAxis, D3DXVECTOR3 clipPlane, D3DXVECTOR2 UV){
			SURFEL_EDGE surfelEdge;

			surfelEdge.vertex = CreateSurfelEdgeVertex(pos, normal, majorAxis, minorAxis, clipPlane, UV);

			return surfelEdge;
		}

		static SURFEL_EDGE* CreateSurfelEdgePointer(D3DXVECTOR3 pos, D3DXVECTOR3 normal, D3DXVECTOR3 majorAxis, D3DXVECTOR3 minorAxis, D3DXVECTOR3 clipPlane, D3DXVECTOR2 UV){
			SURFEL_EDGE* surfelEdge = new SURFEL_EDGE;

			surfelEdge->vertex = CreateSurfelEdgeVertex(pos, normal, majorAxis, minorAxis, clipPlane, UV);

			return surfelEdge;
		}

		static SURFEL CreateSurfel(D3DXVECTOR3 pos, D3DXVECTOR3 normal, D3DXVECTOR3 majorAxis, D3DXVECTOR3 minorAxis, D3DXVECTOR2 UV){
			SURFEL surfel;
			surfel.vertex = CreateSurfelVertex(pos, normal, majorAxis, minorAxis, UV);
			return surfel;
		}

		static SURFEL* CreateSurfelPointer(D3DXVECTOR3 pos, D3DXVECTOR3 normal, D3DXVECTOR3 majorAxis, D3DXVECTOR3 minorAxis, D3DXVECTOR2 UV){
			SURFEL* surfel = new SURFEL;
			surfel->vertex = CreateSurfelVertex(pos, normal, majorAxis, minorAxis, UV);
			return surfel;
		}

		static SURFEL_VERTEX CreateSurfelVertex(D3DXVECTOR3 pos, D3DXVECTOR3 normal, D3DXVECTOR3 majorAxis, D3DXVECTOR3 minorAxis, D3DXVECTOR2 UV){
			SURFEL_VERTEX surfelVertex;
			surfelVertex.pos = pos;
			surfelVertex.normal = normal;
			surfelVertex.minorAxis = minorAxis;
			surfelVertex.majorAxis = majorAxis;			
			surfelVertex.UV = UV;

			return surfelVertex;
		}

		static SURFEL_EDGE_VERTEX CreateSurfelEdgeVertex(D3DXVECTOR3 pos, D3DXVECTOR3 normal, D3DXVECTOR3 majorAxis, D3DXVECTOR3 minorAxis, D3DXVECTOR3 clipPlane, D3DXVECTOR2 UV){
			SURFEL_EDGE_VERTEX surfelEdgeVertex;
			
			surfelEdgeVertex.pos = pos;
			surfelEdgeVertex.normal = normal;
			surfelEdgeVertex.minorAxis = minorAxis;
			surfelEdgeVertex.majorAxis = majorAxis;			
			surfelEdgeVertex.clipPlane = clipPlane;
			surfelEdgeVertex.UV = UV;

			return surfelEdgeVertex;
		}


		static Phyxel_Grid_Cell* CreatePhyxelGridCellPointer(){
			Phyxel_Grid_Cell* phyxelGridCell = new Phyxel_Grid_Cell;
			
			phyxelGridCell->phyxel = NULL;

			phyxelGridCell->neighbours = ThreeInOneArray<Phyxel_Grid_Cell*>(3, 3, 3);
			for(unsigned int i = 0; i<phyxelGridCell->neighbours.GetSize(); i++){
				phyxelGridCell->neighbours[i] = NULL;
			}

			return phyxelGridCell;
		}

		/*static PHYXEL_NODE* CreatePhyxelNodePointer(){
			PHYXEL_NODE* phyxelNode = new PHYXEL_NODE;

			phyxelNode->mass = 0.0f;
			phyxelNode->density = 0.0f;
			phyxelNode->volume = 0.0f;

			phyxelNode->force = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		
			phyxelNode->neighbours = ThreeInOneArray<PHYXEL_NODE*>(3, 3, 3);
			phyxelNode->neighbourWeight = ThreeInOneArray<float>(3, 3, 3);
			for(unsigned int i = 0; i<phyxelNode->neighbours.GetSize(); i++){
				phyxelNode->neighbours[i] = NULL;
				phyxelNode->neighbourWeight[i] = 0.0f;
			}

			return phyxelNode;
		}*/

		static PHYXEL_NODE* CreatePhyxelNodePointer(){
			PHYXEL_NODE* phyxelNode = new PHYXEL_NODE;

			phyxelNode->mass = 0.0f;
			phyxelNode->density = 0.0f;
			phyxelNode->volume = 0.0f;

			phyxelNode->force = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			phyxelNode->dotDisplacement = phyxelNode->force;
			phyxelNode->bodyForce = phyxelNode->force;
			phyxelNode->displacement = phyxelNode->force;
		
			D3DXMatrixScaling(&phyxelNode->displacementGradient, 0.0f, 0.0f, 0.0f);
			phyxelNode->displacementGradient._44 = 0.0f;
			
			phyxelNode->strain = phyxelNode->displacementGradient;
			phyxelNode->stress = phyxelNode->strain;
			phyxelNode->momentMatrix = phyxelNode->stress;

			phyxelNode->neighbours = ThreeInOneArray<PHYXEL_NODE*>(3, 3, 3);
			phyxelNode->neighbourWeight = ThreeInOneArray<float>(3, 3, 3);
			for(unsigned int i = 0; i<phyxelNode->neighbours.GetSize(); i++){
				phyxelNode->neighbours[i] = NULL;
				phyxelNode->neighbourWeight[i] = 0.0f;
			}

			return phyxelNode;
		}
	};
}

#endif