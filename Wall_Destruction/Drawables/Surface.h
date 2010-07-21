#ifndef SURFACE_H
#define SURFACE_H

#include "Structs.h"
#include "Drawable.h"
#include "CustomEffect.h"
#include "ChangedPhyxels.h"
#include <D3DX10.h>

namespace Drawables{
	class Surface : public Drawable
	{
	public:
		Surface(void);
		~Surface(void);

		void SetChangedPhyxelsObject(ChangedPhyxels* changedPhyxels){
			this->changedPhyxels = changedPhyxels;
		}

		void AddSurfel(ProjectStructs::SURFEL *s);
		void AddEdgeSurfel(ProjectStructs::SURFEL_EDGE *s);

		void Draw();
		void DrawSolid();
		void DrawSurfel();
		void DrawWireframe();
		
		void Init();
		void Update(float dt);
		void CleanUp();

		void SetWorld(D3DXMATRIX world){this->world = world;}
		D3DXMATRIX GetWorld(){return world;}

		ID3D10Buffer* GetSurfelReadableBuffer(){return readableVertexBuffer;}
		ID3D10Buffer* GetEdgeReadableBuffer(){return readableEdgeVertexBuffer;}

		int GetSurfaceSurfelCount(){return surfelCount;};
		int GetEdgeSurfelCount(){return edgeCount;};

		void AddForce(D3DXVECTOR3 force, D3DXVECTOR3 pos, int surfelIndex, int edgeIndex);
		void AddForceToPhyxels(D3DXVECTOR3 force, D3DXVECTOR3 pos, D3DXVECTOR3 direction, ProjectStructs::PHYXEL_NODE *phyxel);

		ProjectStructs::SURFEL* GetSurfaceSurfel(int i){
			return surfaceSurfels[i];
		}

		ProjectStructs::SURFEL_EDGE* GetEdgeSurfel(int i){
			return edgeSurfels[i];
		}

		void SetPosition(D3DXVECTOR3 pos){
			SurfacePos = pos;
		}

		D3DXVECTOR3 GetPosition(){
			return SurfacePos;
		}

		void SetDeltaSurfelUV(D3DXVECTOR2 delta){
			DeltaSurfelUV = delta;
		}

		void SetTexture(std::string texture){
			planeTexture = texture;
		}

		static float RadiusScale;
		static bool isChanged;

	private:
		bool AddForceToPhyxel(D3DXVECTOR3 force, D3DXVECTOR3 pos, D3DXVECTOR3 direction, ProjectStructs::PHYXEL_NODE *phyxel);
		void InitSurfel();
		void InitWireframe();
		void InitSolid();
		void InitGeometryPass();
		void InitCommonSolidAndWireframe();

		// force stuff
		D3DXVECTOR3 force;
		bool forceAdded;

		D3DXVECTOR3 SurfacePos;
		D3DXVECTOR2 DeltaSurfelUV;

		std::vector<ProjectStructs::SURFEL*> surfaceSurfels;
		std::vector<ProjectStructs::SURFEL_EDGE*> edgeSurfels;

		std::vector<ProjectStructs::SURFEL_VERTEX> surfaceSurfelsVertices;
		std::vector<ProjectStructs::SURFEL_EDGE_VERTEX> edgeSurfelsVertices;

		int surfelCount, edgeCount, maxSurfaceCount, maxSurfaceEdgeCount;
		D3DXMATRIX world;
		std::string planeTexture;

		ID3D10Buffer *surfelVertexBuffer, *surfelEdgeVertexBuffer, *solidVertexBuffer, *solidEdgeVertexBuffer, *readableVertexBuffer, *readableEdgeVertexBuffer;
		
		static ID3D10RasterizerState *SolidRenderState;
		static Helpers::CustomEffect surfelEffect, surfelEdgeEffect, solidEffect, wireframeEffect, geometryEffect, geometryEdgeEffect;
		static ID3D10ShaderResourceView *SurfelTexture, *SurfelWireframeTexture;
		static bool TextureLoaded;

		ChangedPhyxels* changedPhyxels;
	};
}

#endif