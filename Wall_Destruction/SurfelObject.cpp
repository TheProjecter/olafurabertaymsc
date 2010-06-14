#include "SurfelObject.h"
#include "Globals.h"
#include "RandRange.h"

namespace Drawables{
	SurfelObject::SurfelObject(void){}

	SurfelObject::SurfelObject(std::vector<Structs::SURFEL_VERTEX> surfels){
		this->surfels = surfels;
	}

	SurfelObject::~SurfelObject(void){}

	void SurfelObject::Init(){
		surfelVertexBuffer = NULL;
		solidVertexBuffer = NULL;

		SetMaterialInfo(0.416384f, 0.142109f);

		InitGeometryPass();
		InitSurfel();
		InitCommonSolidAndWireframe();
		InitWireframe();
		InitSolid();
		ResetSurfels(surfels);
	}

	void SurfelObject::InitGeometryPass(){
		// Define the input layout
		D3D10_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 }, // pos
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(D3DXVECTOR3), D3D10_INPUT_PER_VERTEX_DATA, 0 }, // normal
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 2*sizeof(D3DXVECTOR3), D3D10_INPUT_PER_VERTEX_DATA, 0 }, // dimensions
		};

		// create the effect
		geometryEffect = Helpers::CustomEffect("Surfels.fx", "GeometryTechnique", CUSTOM_EFFECT_TYPE_PIXEL | CUSTOM_EFFECT_TYPE_GEOMETRY | CUSTOM_EFFECT_TYPE_VERTEX , layout, 3);

		geometryEffect.AddVariable("World");
		geometryEffect.AddVariable("View");
		geometryEffect.AddVariable("Projection");
	
		geometryEffect.SetMatrix("World", world);
	}

	void SurfelObject::InitSurfel(){
		// Define the input layout
		D3D10_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 }, // pos
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(D3DXVECTOR3), D3D10_INPUT_PER_VERTEX_DATA, 0 }, // normal
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 2*sizeof(D3DXVECTOR3), D3D10_INPUT_PER_VERTEX_DATA, 0 }, // dimensions
		};

		// create the effect
		surfelEffect = Helpers::CustomEffect("Surfels.fx", "SurfelTechnique", CUSTOM_EFFECT_TYPE_PIXEL | CUSTOM_EFFECT_TYPE_GEOMETRY | CUSTOM_EFFECT_TYPE_VERTEX , layout, 3);
		surfelEffect.AddVariable("World");
		surfelEffect.AddVariable("View");
		surfelEffect.AddVariable("Projection");
		surfelEffect.AddTexture("SurfelTexture", "Textures\\surfel.png");

		surfelEffect.AddVariable("LightPos");
		surfelEffect.AddVariable("AmbientColor");
		surfelEffect.AddVariable("CameraPos");
		surfelEffect.AddVariable("LightDirection");
		surfelEffect.AddVariable("A");
		surfelEffect.AddVariable("B");
		surfelEffect.AddVariable("rhoOverPi");
		surfelEffect.AddVariable("LightColor");		

		surfelEffect.SetTexture("SurfelTexture", "Textures\\surfel.png");
		surfelEffect.SetMatrix("World", world);
		surfelEffect.SetFloatVector("AmbientColor", Helpers::Globals::AppLight.GetAmbientColor());
		surfelEffect.SetFloatVector("LightPos", Helpers::Globals::AppLight.GetPosition());
		surfelEffect.SetFloatVector("LightDirection", Helpers::Globals::AppLight.GetDirection());
		surfelEffect.SetFloat("A", this->A);
		surfelEffect.SetFloat("B", this->B);
		surfelEffect.SetFloat("rhoOverPi", this->rhoOverPi);
		surfelEffect.SetFloatVector("LightColor", Helpers::Globals::AppLight.GetColor());
	}

	void SurfelObject::ResetSurfels(std::vector<Structs::SURFEL_VERTEX> newSurfels){
		if(surfelVertexBuffer){
			surfelVertexBuffer->Release();
			surfelVertexBuffer = NULL;
		}
		
		this->surfels = newSurfels;

		Structs::SURFEL_VERTEX* vertices = NULL;
		vertices = new Structs::SURFEL_VERTEX[newSurfels.size()];

		for(UINT i = 0; i<newSurfels.size();i++){
			vertices[i] = newSurfels[i];
		}
			
		D3D10_BUFFER_DESC bd;
		bd.Usage = D3D10_USAGE_DEFAULT;
		bd.ByteWidth = newSurfels.size() * sizeof( Structs::SURFEL_VERTEX );
		bd.BindFlags = D3D10_BIND_VERTEX_BUFFER | D3D10_BIND_STREAM_OUTPUT;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		D3D10_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = vertices;

		HR(Helpers::Globals::Device->CreateBuffer( &bd, &InitData, &surfelVertexBuffer ));	

		geometryEffect.WriteToGeometryShader(D3D10_PRIMITIVE_TOPOLOGY_POINTLIST, surfelVertexBuffer, newSurfels.size(), solidVertexBuffer);

		Helpers::Globals::Device->CopyResource(readableVertexBuffer, solidVertexBuffer);

		delete vertices;
	}

	void SurfelObject::ResetSurfels(){
		ResetSurfels(surfels);
	}

	void SurfelObject::InitCommonSolidAndWireframe(){	

		D3D10_BUFFER_DESC vbdesc =
		{
			100 * sizeof(Structs::SOLID_VERTEX),
			D3D10_USAGE_DEFAULT,
			D3D10_BIND_VERTEX_BUFFER | D3D10_BIND_STREAM_OUTPUT,
			0,
			0
		};

		HR( Helpers::Globals::Device->CreateBuffer( &vbdesc, NULL, &solidVertexBuffer ) );		

		D3D10_BUFFER_DESC vbdesc2 =
		{
			100 * sizeof(Structs::SOLID_VERTEX),
			D3D10_USAGE_STAGING,
			0,
			D3D10_CPU_ACCESS_READ,
			0
		};

		HR( Helpers::Globals::Device->CreateBuffer( &vbdesc2, NULL, &readableVertexBuffer) );		
	}

	void SurfelObject::InitWireframe(){
		// Define the input layout
		D3D10_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 }, // pos
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(D3DXVECTOR3), D3D10_INPUT_PER_VERTEX_DATA, 0 }, // normal
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 2*sizeof(D3DXVECTOR3), D3D10_INPUT_PER_VERTEX_DATA, 0 }, // UV
		};

		// create the effect
		wireframeEffect = Helpers::CustomEffect("Surfels.fx", "WireframeTechnique", CUSTOM_EFFECT_TYPE_PIXEL | CUSTOM_EFFECT_TYPE_VERTEX , layout, 3);
		wireframeEffect.AddVariable("World");
		wireframeEffect.AddVariable("View");
		wireframeEffect.AddVariable("Projection");
		wireframeEffect.AddTexture("SurfelTexture", surfelsSolidTexture);

		wireframeEffect.AddVariable("LightPos");
		wireframeEffect.AddVariable("AmbientColor");
		wireframeEffect.AddVariable("CameraPos");
		wireframeEffect.AddVariable("LightDirection");
		wireframeEffect.AddVariable("A");
		wireframeEffect.AddVariable("B");
		wireframeEffect.AddVariable("rhoOverPi");
		wireframeEffect.AddVariable("LightColor");		


		wireframeEffect.SetTexture("SurfelTexture", surfelsSolidTexture);
		wireframeEffect.SetMatrix("World", world);
		wireframeEffect.SetFloatVector("AmbientColor", Helpers::Globals::AppLight.GetAmbientColor());
		wireframeEffect.SetFloatVector("LightPos", Helpers::Globals::AppLight.GetPosition());
		wireframeEffect.SetFloatVector("LightDirection", Helpers::Globals::AppLight.GetDirection());
		wireframeEffect.SetFloat("A", this->A);
		wireframeEffect.SetFloat("B", this->B);
		wireframeEffect.SetFloat("rhoOverPi", this->rhoOverPi);
		wireframeEffect.SetFloatVector("LightColor", Helpers::Globals::AppLight.GetColor());
		
		// setup the solid renderstate since we want to reset the renderstate when the wireframe surfel has finished
		D3D10_RASTERIZER_DESC solidRasterizer;
		solidRasterizer.FillMode = D3D10_FILL_SOLID;
		Helpers::Globals::Device->CreateRasterizerState(&solidRasterizer, &SolidRenderState);
	}

	void SurfelObject::InitSolid(){
		// Define the input layout
		D3D10_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 }, // pos
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(D3DXVECTOR3), D3D10_INPUT_PER_VERTEX_DATA, 0 }, // normal
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 2*sizeof(D3DXVECTOR3), D3D10_INPUT_PER_VERTEX_DATA, 0 }, // UV
		};

		// create the effect
		solidEffect= Helpers::CustomEffect("Surfels.fx", "SolidTechnique", CUSTOM_EFFECT_TYPE_PIXEL | CUSTOM_EFFECT_TYPE_VERTEX , layout, 3);

		solidEffect.AddVariable("World");
		solidEffect.AddVariable("View");
		solidEffect.AddVariable("Projection");
		solidEffect.AddTexture("SurfelTexture", surfelsSolidTexture);

		solidEffect.AddVariable("LightPos");
		solidEffect.AddVariable("AmbientColor");
		solidEffect.AddVariable("CameraPos");
		solidEffect.AddVariable("LightDirection");
		solidEffect.AddVariable("A");
		solidEffect.AddVariable("B");
		solidEffect.AddVariable("rhoOverPi");
		solidEffect.AddVariable("LightColor");		

		solidEffect.SetTexture("SurfelTexture", surfelsSolidTexture);
		solidEffect.SetMatrix("World", world);
		solidEffect.SetFloatVector("AmbientColor", Helpers::Globals::AppLight.GetAmbientColor());
		solidEffect.SetFloatVector("LightPos", Helpers::Globals::AppLight.GetPosition());
		solidEffect.SetFloatVector("LightDirection", Helpers::Globals::AppLight.GetDirection());
		solidEffect.SetFloat("A", this->A);
		solidEffect.SetFloat("B", this->B);
		solidEffect.SetFloat("rhoOverPi", this->rhoOverPi);
		solidEffect.SetFloatVector("LightColor", Helpers::Globals::AppLight.GetColor());
	}

	void SurfelObject::Draw(){
		if(Helpers::Globals::SurfelDrawMethod == Helpers::SOLID){
			DrawSolid();
		}
		else if(Helpers::Globals::SurfelDrawMethod == Helpers::WIREFRAME){
			DrawWireframe();
		}
		else if(Helpers::Globals::SurfelDrawMethod == Helpers::SURFEL){
			DrawSurfel();
		}
	}

	void SurfelObject::DrawSolid(){
		solidEffect.SetFloatVector("CameraPos", Helpers::Globals::AppCamera.Position());
		solidEffect.SetMatrix("View", Helpers::Globals::AppCamera.View());
		solidEffect.SetMatrix("Projection", Helpers::Globals::AppCamera.Projection());

		solidEffect.PreDraw();

		UINT stride = sizeof( Structs::SOLID_VERTEX);
		UINT offset = 0;

		Helpers::Globals::Device->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		Helpers::Globals::Device->IASetVertexBuffers( 0, 1, &solidVertexBuffer, &stride, &offset );

		solidEffect.DrawAuto();
	}

	void SurfelObject::DrawWireframe(){
		wireframeEffect.SetFloatVector("CameraPos", Helpers::Globals::AppCamera.Position());
		wireframeEffect.SetMatrix("View", Helpers::Globals::AppCamera.View());
		wireframeEffect.SetMatrix("Projection", Helpers::Globals::AppCamera.Projection());

		wireframeEffect.PreDraw();

		UINT stride = sizeof( Structs::SOLID_VERTEX);
		UINT offset = 0;

		Helpers::Globals::Device->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
		Helpers::Globals::Device->IASetVertexBuffers( 0, 1, &solidVertexBuffer, &stride, &offset );

		wireframeEffect.DrawAuto();
		Helpers::Globals::Device->RSSetState(SolidRenderState);
	}

	void SurfelObject::DrawSurfel(){
	
		surfelEffect.SetFloatVector("CameraPos", Helpers::Globals::AppCamera.Position());
		surfelEffect.SetMatrix("View", Helpers::Globals::AppCamera.View());
		surfelEffect.SetMatrix("Projection", Helpers::Globals::AppCamera.Projection());

		surfelEffect.PreDraw();
	
		UINT stride = sizeof( Structs::SURFEL_VERTEX);
		UINT offset = 0;
		
		Helpers::Globals::Device->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);
		Helpers::Globals::Device->IASetVertexBuffers( 0, 1, &surfelVertexBuffer, &stride, &offset );

		surfelEffect.Draw(surfels.size());
	}

	void SurfelObject::CleanUp(){
		geometryEffect.CleanUp();
		surfelEffect.CleanUp();
		solidEffect.CleanUp();
		wireframeEffect.CleanUp();
		
		surfelVertexBuffer->Release();
		solidVertexBuffer->Release();

		surfels.clear();
	}
}
