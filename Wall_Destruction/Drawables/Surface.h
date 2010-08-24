#ifndef SURFACE_H
#define SURFACE_H

#include "Structs.h"
#include "Drawable.h"
#include "CustomEffect.h"
#include "ImpactList.h"
#include <D3DX10.h>
#include <algorithm>
#include <vector>
#include "HavokPhysicsInclude.h"

// forward declarations
class PhyxelGrid;
class ContactListener;

namespace Drawables{
	class Surface : public Drawable
	{
	public:
		Surface(void);
		~Surface(void);

		void SetImpactList(ImpactList* impactlist){
			this->impactList = impactlist;
		}

		void AddSurfel(ProjectStructs::SURFEL *s);

		void AddRigidBody(hkpRigidBody* rigid){rigidBodies.push_back(rigid);}
		std::vector<hkpRigidBody*> GetRigidBodies(){return rigidBodies;}
		
		void AddContactListener(ContactListener* contactListener){
			contactListeners.push_back(contactListener);
		}

		void Draw();
		void DrawSolid();
		void DrawSurfel();
		void DrawWireframe();
		void DrawToReadableBuffer();
		
		void Init(ProjectStructs::MATERIAL_PROPERTIES materialProperties);
		void ResetBuffers();
		void PushSurfelsIntoGrid();
		void Update(float dt);
		void CleanUp();

		void SetWorld(D3DXMATRIX world){this->world = world;}
		D3DXMATRIX GetWorld(){return world;}

		ID3D10Buffer* GetSurfelReadableBuffer(){return readableVertexBuffer;}

		int GetSurfaceSurfelCount(){return surfaceSurfels.size();};

		void AddForce(D3DXVECTOR3 force, D3DXVECTOR3 pos, ProjectStructs::SURFEL* surfel);
		void AddForceToPhyxels(D3DXVECTOR3 force, D3DXVECTOR3 pos, D3DXVECTOR3 direction, ProjectStructs::PHYXEL_NODE *phyxel, ProjectStructs::SURFEL *surfel);

		ProjectStructs::SURFEL* GetSurfaceSurfel(int i){
			return surfaceSurfels[i];
		}

		void SetPosition(D3DXVECTOR3 pos){
			SurfacePos = pos;
		}

		D3DXVECTOR3 GetPosition(){
			return SurfacePos;
		}

		void SetTexture(std::string texture){
			planeTexture = texture;
		}

		void ResampleSurfel(ProjectStructs::SURFEL* surfel, ProjectStructs::IMPACT* impact);
		void SetPhyxelGrid(PhyxelGrid* grid){this->grid = grid;}

		static float RadiusScale;
		static bool isChanged;

	private:
		bool AddForceToPhyxel(D3DXVECTOR3 force, D3DXVECTOR3 pos, D3DXVECTOR3 direction, ProjectStructs::PHYXEL_NODE *phyxel, ProjectStructs::SURFEL* surfel);
		void InitSurfel();
		void InitWireframe();
		void InitSolid();
		void InitGeometryPass();
		void InitCommonSolidAndWireframe();
		void SetUpNeighborEffect();

		void SetUpNeighborDraw();
		void DrawNeighbors();

		// force stuff
		D3DXVECTOR3 force;
		bool forceAdded;

		D3DXVECTOR3 SurfacePos;
		D3DXVECTOR2 DeltaSurfelUV;

		std::vector<ProjectStructs::SURFEL*> surfaceSurfels;
		std::vector<int> newSurfelsForPhyxelGrid;

		D3DXMATRIX world;
		std::string planeTexture;

		ID3D10Buffer *surfelVertexBuffer, *solidVertexBuffer, *readableVertexBuffer, *neighborVertexBuffer;
		
		static ID3D10RasterizerState *SolidRenderState;
		static Helpers::CustomEffect surfelEffect, solidEffect, wireframeEffect, geometryEffect, simpleEffect;
		static ID3D10ShaderResourceView *SurfelTexture, *SurfelWireframeTexture;
		static bool TextureLoaded;

		bool neighborDrawSet;
		int neighborCount;

		ImpactList* impactList;

		std::vector<hkpRigidBody*> rigidBodies;
		std::vector<ContactListener*> contactListeners;
		ProjectStructs::MATERIAL_PROPERTIES materialProperties;
		PhyxelGrid* grid;
	};
}

#endif