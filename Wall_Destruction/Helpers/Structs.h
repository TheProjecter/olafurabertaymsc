#ifndef SURFELS_H
#define SURFELS_H

#include <D3D10.h>
#include <D3DX10.h>
#include <string>
#include <map>
#include <vector>
#include "HavokPhysicsInclude.h"
#include "ThreeInOneArray.h"

class ContactListener;

namespace ProjectStructs{

// forward declarations
	struct CRACK_NODE;
	struct SURFEL;
	struct SURFEL_VERTEX;
	struct Phyxel_Grid_Cell;

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

	struct PHYXEL_NODE{
		D3DXVECTOR3 pos;
		D3DXVECTOR3 displacement;
		D3DXVECTOR3 totalDisplacement;
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
		std::vector<PHYXEL_NODE*> addedNodes;
		ThreeInOneArray<float> neighbourWeight;
		Phyxel_Grid_Cell* parent;
	};

	struct Phyxel_Grid_Cell{
		ThreeInOneArray<Phyxel_Grid_Cell*> neighbours;
		std::vector<CRACK_NODE*> cracks;
		std::vector<SURFEL*> surfels;
		PHYXEL_NODE* phyxel;
		D3DXVECTOR3 halfWidth;
	};

	struct Vertex_Grid_Cell{
		std::vector<CRACK_NODE*> cracks;
		std::vector<SURFEL*> surfels;
		ID3D10Buffer *surfelVertexBuffer, *solidVertexBuffer, *readableVertexBuffer, *neighborVertexBuffer;
		UINT neighborCount;
		bool changed;
	};

	/*
	Clip planes can be
	(0, 1, 0)		(1, 0, 0)		(0, 1, 1)		(1, 0, 1)
	 _								     _			 _ _
	| |			      _ _				| |			|_ _|
	|_|			     |_ _|				|_|				 

	(1, -1, 1)		(-1, -1, 1)		(1, 1, 1)		(-1, 1, 1)	   
	    			 				       _		  _				
	   _			 _					  |_|		 |_| 			
	  |_| 			|_|   			     	   	     				

*/
	struct SURFEL_VERTEX{
		// position
		D3DXVECTOR3 pos;
		// normal
		D3DXVECTOR3 normal;
		D3DXVECTOR3 majorAxis;
		D3DXVECTOR3 minorAxis;
		D3DXVECTOR2 UV;
		D3DXVECTOR2 deltaUV;
		int frontFacing;
		D3DXVECTOR3 clipPlane;
	};

	struct SURFEL{
		std::vector<Phyxel_Grid_Cell*> intersectingCells;

		std::map<float, SURFEL*> neighbors;
		std::map<SURFEL*, float> inverseNeighbors;

		std::map<float, SURFEL*> oldNeighbors;
		std::map<SURFEL*, float> oldInverseNeighbors;
		
		std::vector<PHYXEL_NODE*> displacedPhyxels;

		D3DXVECTOR3 displacement;
		D3DXVECTOR3 initialPosition;
		int displacementCount;
		D3DXVECTOR3 lastDisplacement;
	
		hkpRigidBody* rigidBody;
		bool hasRigidBody;

		SURFEL_VERTEX* vertex;
		Vertex_Grid_Cell* vertexGridCell;
		ContactListener* contactListener;

		bool isChanged;
		bool isChecked;
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
		//float impacted;
	};

	struct POINT_CLOUD_VERTEX {
		D3DXVECTOR3 pos;
		D3DXVECTOR3 normal;
	};

	struct SIMPLE_VERTEX{
		D3DXVECTOR3 pos;
	};

	struct SIMPLE_TEXTURE_VERTEX{
		D3DXVECTOR3 pos;
		D3DXVECTOR2 UV;
	};

	struct CRACK_NODE{
		SURFEL* s1;
		SURFEL* s2;
	};

	struct MATERIAL_PROPERTIES{
		// material parameters
		bool deformable;
		std::string texture;
		float density;
		float toughness;
		float poissonRatio;
		float youngsModulus;
		float damping_constant_phi;
		float damping_constant_psi;
		float phyxelGridSize;
		float minimunSurfelSize;
		float rho;
		float sigma;
		float vertexGridSize;
		float mass;
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

	struct IMPACT{
		D3DXVECTOR3 impactPos;
		SURFEL* surfel;
		PHYXEL_NODE* phyxel;
	};

	struct CRACK{
		IMPACT* impact;
		D3DXVECTOR3 eigenVector;
	};

	struct Point_Grid_Cell{
		ThreeInOneArray<Point_Grid_Cell*> neighbors;
		std::vector<SURFEL*> surfels;
		D3DXVECTOR3 pos;
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

		static SURFEL* CreateSurfelEdgePointer(D3DXVECTOR3 pos, D3DXVECTOR3 normal, D3DXVECTOR3 majorAxis, D3DXVECTOR3 minorAxis, D3DXVECTOR3 clipPlane, D3DXVECTOR2 UV, D3DXVECTOR2 deltaUV){
			SURFEL* surfelEdge = new SURFEL;

			surfelEdge->vertex = CreateSurfelVertex(pos, normal, majorAxis, minorAxis, UV, deltaUV);
			surfelEdge->vertex->clipPlane = clipPlane;
			surfelEdge->displacement = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			surfelEdge->lastDisplacement = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			surfelEdge->initialPosition = pos;
			surfelEdge->displacementCount = 0;
			surfelEdge->rigidBody = NULL;
			surfelEdge->hasRigidBody = false;
			surfelEdge->isChanged = true;
			surfelEdge->isChecked = false;
			surfelEdge->contactListener = NULL;
			surfelEdge->vertexGridCell = NULL;

			return surfelEdge;
		}

		static SURFEL* CreateSurfelPointer(D3DXVECTOR3 pos, D3DXVECTOR3 normal, D3DXVECTOR3 majorAxis, D3DXVECTOR3 minorAxis, D3DXVECTOR2 UV, D3DXVECTOR2 deltaUV){
			SURFEL* surfel = new SURFEL;
			
			surfel->vertex = CreateSurfelVertex(pos, normal, majorAxis, minorAxis, UV, deltaUV);
			surfel->displacement = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			surfel->lastDisplacement = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			surfel->initialPosition = pos;
			surfel->displacementCount = 0;
			surfel->rigidBody = NULL;
			surfel->hasRigidBody = false;
			surfel->isChanged = true;
			surfel->isChecked = false;
			surfel->contactListener = NULL;
			surfel->vertexGridCell = NULL;

			return surfel;
		}

		static SURFEL_VERTEX* CreateSurfelVertex(D3DXVECTOR3 pos, D3DXVECTOR3 normal, D3DXVECTOR3 majorAxis, D3DXVECTOR3 minorAxis, D3DXVECTOR2 UV, D3DXVECTOR2 deltaUV){
			SURFEL_VERTEX* surfelVertex = new SURFEL_VERTEX;
			surfelVertex->pos = pos;
			surfelVertex->normal = normal;
			surfelVertex->minorAxis = minorAxis;
			surfelVertex->majorAxis = majorAxis;			
			surfelVertex->UV = UV;
			surfelVertex->deltaUV = deltaUV;
			surfelVertex->frontFacing = -1;
			surfelVertex->clipPlane = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

			return surfelVertex;
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

		static PHYXEL_NODE* CreatePhyxelNodePointer(){
			PHYXEL_NODE* phyxelNode = new PHYXEL_NODE;

			phyxelNode->mass = 0.0f;
			phyxelNode->density = 0.0f;
			phyxelNode->volume = 0.0f;

			phyxelNode->force = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			phyxelNode->dotDisplacement = phyxelNode->force;
			phyxelNode->bodyForce = phyxelNode->force;
			phyxelNode->displacement = phyxelNode->force;
			phyxelNode->totalDisplacement= phyxelNode->force;
		
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

			phyxelNode->parent = NULL;

			return phyxelNode;
		}

		static CRACK_NODE* CreateCrackNodePointer(){
			CRACK_NODE* crackNode = new CRACK_NODE;
	
			crackNode->s1 = NULL;
			crackNode->s2 = NULL;

			return crackNode;
		}

		static Point_Grid_Cell* CreatePointGridCell(){
			Point_Grid_Cell* cell = new Point_Grid_Cell;
			cell->neighbors = ThreeInOneArray<Point_Grid_Cell*>(3, 3, 3);
			for(unsigned int i = 0; i < cell->neighbors.GetSize(); i++){
				cell->neighbors[i] = NULL;
			}
			return cell;
		}
	};
}

#endif