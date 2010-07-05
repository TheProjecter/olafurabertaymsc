#ifndef SURFACE_H
#define SURFACE_H

#include "Structs.h"
#include "Drawable.h"
#include "CustomEffect.h"
#include <D3DX10.h>

namespace Drawables{
	class Surface : public Drawable
	{
	public:
		Surface(void);
		~Surface(void);

		void AddSurfel(Structs::SURFEL_VERTEX s);
		void AddEdgeSurfel(Structs::SURFEL_EDGE_VERTEX s);

		void Draw();
		void DrawSolid();
		void DrawSurfel();
		void DrawWireframe();
		
		void Init();
		void Update();
		void CleanUp();

		void SetSurfaceSurfels(Structs::SURFEL_VERTEX *surfaceSurfels, int count){
			this->surfaceSurfels = surfaceSurfels; 
			surfelCount = count;
		}

		void SetEdgeSurfels(Structs::SURFEL_EDGE_VERTEX *edgeSurfels, int edgeCount){
			this->edgeSurfels = edgeSurfels; 
			this->edgeCount = edgeCount;
		}

		void SetWorld(D3DXMATRIX world){this->world = world;}
		D3DXMATRIX GetWorld(){return world;}

		ID3D10Buffer* GetReadableBuffer(){return readableVertexBuffer;}

		int GetSurfaceSurfelCount(){return surfelCount;};
		int GetEdgeSurfelCount(){return edgeCount;};

		Structs::SURFEL_VERTEX GetSurfaceSurfel(int i){
			return surfaceSurfels[i];
		}

		Structs::SURFEL_EDGE_VERTEX GetEdgeSurfel(int i){
			return edgeSurfels[i];
		}

		void SetPosition(D3DXVECTOR3 pos){
			SurfacePos = pos;
		}

		void SetDeltaSurfelUV(D3DXVECTOR2 delta){
			DeltaSurfelUV = delta;
		}

		void SetTexture(std::string texture){
			planeTexture = texture;
		}

		static float RadiusScale;

	private:
		void InitSurfel();
		void InitWireframe();
		void InitSolid();
		void InitGeometryPass();
		void InitCommonSolidAndWireframe();

		D3DXVECTOR3 SurfacePos;
		D3DXVECTOR2 DeltaSurfelUV;
		Structs::SURFEL_VERTEX *surfaceSurfels;
		Structs::SURFEL_EDGE_VERTEX *edgeSurfels;
		int surfelCount, edgeCount, maxSurfaceCount, maxSurfaceEdgeCount;
		D3DXMATRIX world;
		std::string planeTexture;

		Structs::SURFEL_VERTEX *surfaces;
		Structs::SURFEL_EDGE_VERTEX *edges;

		ID3D10Buffer *surfelVertexBuffer, *surfelEdgeVertexBuffer, *solidVertexBuffer, *readableVertexBuffer;
		
		static ID3D10RasterizerState *SolidRenderState;
		static Helpers::CustomEffect surfelEffect, surfelEdgeEffect, solidEffect, wireframeEffect, geometryEffect, geometryEdgeEffect;
		static ID3D10ShaderResourceView *SurfelTexture, *SurfelWireframeTexture;
		static bool TextureLoaded;
	};
}

#endif