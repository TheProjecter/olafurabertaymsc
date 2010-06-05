#ifndef SURFEL_OBJECT_H
#define SURFEL_OBJECT_H

#include <D3DX10.h>
#include <vector>
#include "Drawable.h"
#include "CustomEffect.h"

// http://www.nealen.net/projects/mls/asapmls.pdf
namespace Drawables{
	struct SURFEL_VERTEX{
		// position
		D3DXVECTOR3 pos;
		// normal
		D3DXVECTOR3 normal;
		// dimensions.x : width
		// dimensions.y : height
		D3DXVECTOR2 dimensions;
	};

	struct SOLID_VERTEX{
		D3DXVECTOR3 pos;
		D3DXVECTOR3 normal;
		D3DXVECTOR2 UV;
	};

	enum DrawMethod {SURFEL, WIREFRAME, SOLID};

	class SurfelObject : Drawable
	{
	public:
		SurfelObject(void);
		~SurfelObject(void);

		void SetDrawMethod(DrawMethod method) { drawMethod = method; };

		void Init();
		void Draw();
		void CleanUp();
		void RandomizeSurfels();
	
	private:
		void DrawSurfel();
		void DrawWireframe();
		void DrawSolid();
		void DrawToGeometry();

		void InitSurfel();
		void InitWireframe();
		void InitSolid();
		void InitGeometryPass();

		void InitCommonSolidAndWireframe();

		std::vector<SURFEL_VERTEX> surfels;
		DrawMethod drawMethod;

		Helpers::CustomEffect surfelEffect, wireframeEffect, solidEffect, geometryEffect;
		ID3D10Buffer *surfelVertexBuffer, *solidVertexBuffer, *dynamicSurfelVertexBuffer;
		
		ID3D10RasterizerState *SolidRenderState;

		D3DXMATRIX world;
	};
}

#endif
