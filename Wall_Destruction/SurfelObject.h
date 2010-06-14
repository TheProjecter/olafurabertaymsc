#ifndef SURFEL_OBJECT_H
#define SURFEL_OBJECT_H

#include <D3DX10.h>
#include <vector>
#include "Drawable.h"
#include "Structs.h"
#include "CustomEffect.h"
#include "HavokPhysicsInclude.h"

// http://www.nealen.net/projects/mls/asapmls.pdf
namespace Drawables{


	class SurfelObject : public Drawable
	{
	public:
		SurfelObject(void);
		SurfelObject(std::vector<Structs::SURFEL_VERTEX> surfels);
		~SurfelObject(void);

		void Draw();
		void Update(float dt);
		void Init();
		void CleanUp();

		void SetWorld(D3DXMATRIX world){ this->world = world;}
		void SetSurfelsSolidTexture(std::string tex){this->surfelsSolidTexture = tex;}

		void ResetSurfels(std::vector<Structs::SURFEL_VERTEX> newSurfels);
		void ResetSurfels();

		std::vector<Structs::SURFEL_VERTEX> GetSurfels(){return surfels;}

		ID3D10Buffer* GetReadableVertexBuffer(){return readableVertexBuffer;}
	
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

		std::vector<Structs::SURFEL_VERTEX> surfels;
		std::vector<hkpRigidBody*> surfelRigidBodies;
		std::string surfelsSolidTexture;

		Helpers::CustomEffect surfelEffect, wireframeEffect, solidEffect, geometryEffect;
		ID3D10Buffer *surfelVertexBuffer, *solidVertexBuffer, *readableVertexBuffer;
		
		ID3D10RasterizerState *SolidRenderState;

		D3DXMATRIX world;
	};
}

#endif
