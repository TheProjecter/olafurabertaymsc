#include "Surface.h"
#include "TextureCreator.h"

namespace Drawables{
		
	ID3D10ShaderResourceView *Surface::SurfelTexture, *Surface::SurfelWireframeTexture;
	ID3D10RasterizerState *Surface::SolidRenderState;
	Helpers::CustomEffect Surface::surfelEffect, Surface::surfelEdgeEffect, Surface::solidEffect, Surface::wireframeEffect, Surface::geometryEffect, Surface::geometryEdgeEffect;
	float Surface::RadiusScale = 1.0f;
	
	bool Surface::TextureLoaded = false;

	Surface::Surface(void)
	{
		maxSurfaceCount = 6;
		maxSurfaceEdgeCount = 6;
		surfelCount = 0;
		edgeCount = 0;
		surfaceSurfels = new Structs::SURFEL_VERTEX[6];
		edgeSurfels = new Structs::SURFEL_EDGE_VERTEX[6];
	}

	Surface::~Surface(void)
	{
	}

	void Surface::Init(){

		if(!Surface::TextureLoaded){
			SurfelTexture = TextureCreator::CreateSurfelTexture();
			SurfelWireframeTexture = TextureCreator::CreateSurfelWireframeTexture();

			Surface::TextureLoaded = true;

			InitGeometryPass();
			InitSurfel();
			InitWireframe();
			InitSolid();
		}


		surfelEffect.AddTexture("SurfaceTexture", planeTexture);
		surfelEffect.SetTexture("SurfaceTexture", planeTexture);

		surfelEdgeEffect.AddTexture("SurfaceTexture", planeTexture);
		surfelEdgeEffect.SetTexture("SurfaceTexture", planeTexture);

		wireframeEffect.AddTexture("SurfaceTexture", planeTexture);
		wireframeEffect.SetTexture("SurfaceTexture", planeTexture);

		surfelVertexBuffer = NULL;
		solidVertexBuffer = NULL;

		SetMaterialInfo(0.416384f, 0.142109f);

		D3D10_BUFFER_DESC bd;
		bd.Usage = D3D10_USAGE_DEFAULT;
		bd.ByteWidth = surfelCount * sizeof( Structs::SURFEL_VERTEX );
		bd.BindFlags = D3D10_BIND_VERTEX_BUFFER | D3D10_BIND_STREAM_OUTPUT;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		D3D10_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = surfaceSurfels;

		HR(Helpers::Globals::Device->CreateBuffer( &bd, &InitData, &surfelVertexBuffer ));	

		D3D10_BUFFER_DESC ebd;
		ebd.Usage = D3D10_USAGE_DEFAULT;
		ebd.ByteWidth = edgeCount * sizeof( Structs::SURFEL_EDGE_VERTEX );
		ebd.BindFlags = D3D10_BIND_VERTEX_BUFFER | D3D10_BIND_STREAM_OUTPUT;
		ebd.CPUAccessFlags = 0;
		ebd.MiscFlags = 0;
		D3D10_SUBRESOURCE_DATA edgeInitData;
		edgeInitData.pSysMem = edgeSurfels;
		HR(Helpers::Globals::Device->CreateBuffer( &ebd, &edgeInitData, &surfelEdgeVertexBuffer ));	

		InitCommonSolidAndWireframe();

		geometryEffect.WriteToGeometryShader(D3D10_PRIMITIVE_TOPOLOGY_POINTLIST, surfelVertexBuffer, surfelCount, solidVertexBuffer);

		Helpers::Globals::Device->CopyResource(readableVertexBuffer, solidVertexBuffer);
		
	}

	void Surface::AddSurfel(Structs::SURFEL_VERTEX s){

		if(surfelCount == maxSurfaceCount){
			maxSurfaceCount += 6;
			Structs::SURFEL_VERTEX *newSurfaces = new Structs::SURFEL_VERTEX[maxSurfaceCount];

			for(int i = 0; i<surfelCount; i++){
				newSurfaces[i] = surfaceSurfels[i];
			}

			delete [] surfaceSurfels;
			surfaceSurfels = newSurfaces;
		}

		surfaceSurfels[surfelCount++] = s;
	} 

	void Surface::AddEdgeSurfel(Structs::SURFEL_EDGE_VERTEX s){

		if(edgeCount == maxSurfaceEdgeCount){
			maxSurfaceEdgeCount += 6;
			Structs::SURFEL_EDGE_VERTEX *newSurfaces = new Structs::SURFEL_EDGE_VERTEX[maxSurfaceEdgeCount];

			for(int i = 0; i<edgeCount; i++){
				newSurfaces[i] = edgeSurfels[i];
			}

			delete [] edgeSurfels;
			edgeSurfels = newSurfaces;
		}

		edgeSurfels[edgeCount++] = s;
	}

	void Surface::InitGeometryPass(){
		// Define the input layout
		D3D10_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 }, // pos
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(D3DXVECTOR3), D3D10_INPUT_PER_VERTEX_DATA, 0 }, // normal
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 2*sizeof(D3DXVECTOR3), D3D10_INPUT_PER_VERTEX_DATA, 0 }, // MajorRadius
			{ "TEXCOORD", 1, DXGI_FORMAT_R32G32B32_FLOAT, 0, 3*sizeof(D3DXVECTOR3), D3D10_INPUT_PER_VERTEX_DATA, 0 }, // MinorRadius
		};

		// create the effect
		geometryEffect = Helpers::CustomEffect("Surfels.fx", "GeometryTechnique", CUSTOM_EFFECT_TYPE_PIXEL | CUSTOM_EFFECT_TYPE_GEOMETRY | CUSTOM_EFFECT_TYPE_VERTEX , layout, 4);

		geometryEffect.AddVariable("World");
		geometryEffect.AddVariable("View");
		geometryEffect.AddVariable("Projection");

		geometryEffect.SetMatrix("World", world);
	}

	void Surface::InitSurfel(){
		// Define the input layout
		D3D10_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 }, // pos
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(D3DXVECTOR3), D3D10_INPUT_PER_VERTEX_DATA, 0 }, // normal
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 2*sizeof(D3DXVECTOR3), D3D10_INPUT_PER_VERTEX_DATA, 0 }, // MajorRadius
			{ "TEXCOORD", 1, DXGI_FORMAT_R32G32B32_FLOAT, 0, 3*sizeof(D3DXVECTOR3), D3D10_INPUT_PER_VERTEX_DATA, 0 }, // MinorRadius
			{ "TEXCOORD", 2, DXGI_FORMAT_R32G32_FLOAT, 0, 4*sizeof(D3DXVECTOR3), D3D10_INPUT_PER_VERTEX_DATA, 0 }, // CenterUV
		};

		surfelEffect = Helpers::CustomEffect("SurfelSplatter.fx", "SurfelTechnique", 0, layout, 5);
		surfelEffect.AddVariable("World");
		surfelEffect.AddVariable("View");
		surfelEffect.AddVariable("Projection");
		surfelEffect.AddVariable("RadiusScale");
		surfelEffect.AddVariable("EWATexture");
		surfelEffect.AddVariable("DeltaUV");

		surfelEffect.AddVariable("LightPos");
		surfelEffect.AddVariable("AmbientColor");
		surfelEffect.AddVariable("CameraPos");
		surfelEffect.AddVariable("LightDirection");
		surfelEffect.AddVariable("A");
		surfelEffect.AddVariable("B");
		surfelEffect.AddVariable("rhoOverPi");
		surfelEffect.AddVariable("LightColor");		

		surfelEffect.SetFloatVector("AmbientColor", Helpers::Globals::AppLight.GetAmbientColor());
		surfelEffect.SetFloatVector("LightPos", Helpers::Globals::AppLight.GetPosition());
		surfelEffect.SetFloatVector("LightDirection", Helpers::Globals::AppLight.GetDirection());
		surfelEffect.SetFloatVector("LightColor", Helpers::Globals::AppLight.GetColor());
		surfelEffect.SetMatrix("Projection", Helpers::Globals::AppCamera.Projection());
		surfelEffect.SetTexture("EWATexture", SurfelTexture);

		// Define the input layout
		D3D10_INPUT_ELEMENT_DESC edgeLayout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 }, // pos
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(D3DXVECTOR3), D3D10_INPUT_PER_VERTEX_DATA, 0 }, // normal
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 2*sizeof(D3DXVECTOR3), D3D10_INPUT_PER_VERTEX_DATA, 0 }, // MajorRadius
			{ "TEXCOORD", 1, DXGI_FORMAT_R32G32B32_FLOAT, 0, 3*sizeof(D3DXVECTOR3), D3D10_INPUT_PER_VERTEX_DATA, 0 }, // MinorRadius
			{ "TEXCOORD", 2, DXGI_FORMAT_R32G32B32_FLOAT, 0, 4*sizeof(D3DXVECTOR3), D3D10_INPUT_PER_VERTEX_DATA, 0 }, // ClipPlane
			{ "TEXCOORD", 3, DXGI_FORMAT_R32G32_FLOAT, 0, 5*sizeof(D3DXVECTOR3), D3D10_INPUT_PER_VERTEX_DATA, 0 }, // CenterUV
		};

		surfelEdgeEffect = Helpers::CustomEffect("SurfelSplatter.fx", "SurfelEdgeTechnique", 0, edgeLayout, 6);
		surfelEdgeEffect.AddVariable("World");
		surfelEdgeEffect.AddVariable("View");
		surfelEdgeEffect.AddVariable("Projection");
		surfelEdgeEffect.AddVariable("RadiusScale");
		surfelEdgeEffect.AddVariable("EWATexture");
		surfelEdgeEffect.AddVariable("DeltaUV");

		surfelEdgeEffect.AddVariable("LightPos");
		surfelEdgeEffect.AddVariable("AmbientColor");
		surfelEdgeEffect.AddVariable("CameraPos");
		surfelEdgeEffect.AddVariable("LightDirection");
		surfelEdgeEffect.AddVariable("A");
		surfelEdgeEffect.AddVariable("B");
		surfelEdgeEffect.AddVariable("rhoOverPi");
		surfelEdgeEffect.AddVariable("LightColor");		

		surfelEdgeEffect.SetMatrix("Projection", Helpers::Globals::AppCamera.Projection());
		surfelEdgeEffect.SetFloatVector("AmbientColor", Helpers::Globals::AppLight.GetAmbientColor());
		surfelEdgeEffect.SetFloatVector("LightPos", Helpers::Globals::AppLight.GetPosition());
		surfelEdgeEffect.SetFloatVector("LightDirection", Helpers::Globals::AppLight.GetDirection());
		surfelEdgeEffect.SetFloatVector("LightColor", Helpers::Globals::AppLight.GetColor());
		
		surfelEdgeEffect.SetTexture("EWATexture", SurfelTexture);
	}

	void Surface::InitCommonSolidAndWireframe(){	

		D3D10_BUFFER_DESC vbdesc =
		{
			500000 * sizeof(Structs::SOLID_VERTEX),
			D3D10_USAGE_DEFAULT,
			D3D10_BIND_VERTEX_BUFFER | D3D10_BIND_STREAM_OUTPUT,
			0,
			0
		};

		HR( Helpers::Globals::Device->CreateBuffer( &vbdesc, NULL, &solidVertexBuffer ) );		

		D3D10_BUFFER_DESC vbdesc2 =
		{
			500000 * sizeof(Structs::SOLID_VERTEX),
			D3D10_USAGE_STAGING,
			0,
			D3D10_CPU_ACCESS_READ,
			0
		};

		HR( Helpers::Globals::Device->CreateBuffer( &vbdesc2, NULL, &readableVertexBuffer) );		
	}

	void Surface::InitWireframe(){
		// Define the input layout
		D3D10_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 }, // pos
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(D3DXVECTOR3), D3D10_INPUT_PER_VERTEX_DATA, 0 }, // normal
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 2*sizeof(D3DXVECTOR3), D3D10_INPUT_PER_VERTEX_DATA, 0 }, // UV
		};

		// create the effect
		wireframeEffect = Helpers::CustomEffect("SurfelSplatter.fx", "WireframeTechnique", CUSTOM_EFFECT_TYPE_PIXEL | CUSTOM_EFFECT_TYPE_VERTEX , layout, 3);
		wireframeEffect.AddVariable("World");
		wireframeEffect.AddVariable("View");
		wireframeEffect.AddVariable("Projection");

		wireframeEffect.AddVariable("LightPos");
		wireframeEffect.AddVariable("AmbientColor");
		wireframeEffect.AddVariable("CameraPos");
		wireframeEffect.AddVariable("LightDirection");
		wireframeEffect.AddVariable("A");
		wireframeEffect.AddVariable("B");
		wireframeEffect.AddVariable("rhoOverPi");
		wireframeEffect.AddVariable("LightColor");

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

	void Surface::InitSolid(){
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
		solidEffect.AddTexture("SurfaceTexture", "Textures\\Floor.jpg");
		solidEffect.AddVariable("EWATexture");

		solidEffect.AddVariable("LightPos");
		solidEffect.AddVariable("AmbientColor");
		solidEffect.AddVariable("CameraPos");
		solidEffect.AddVariable("LightDirection");
		solidEffect.AddVariable("A");
		solidEffect.AddVariable("B");
		solidEffect.AddVariable("rhoOverPi");
		solidEffect.AddVariable("LightColor");		

		solidEffect.SetTexture("EWATexture", SurfelTexture);
		solidEffect.SetTexture("SurfaceTexture", "Textures\\Floor.jpg");
		solidEffect.SetMatrix("World", world);
		solidEffect.SetFloatVector("AmbientColor", Helpers::Globals::AppLight.GetAmbientColor());
		solidEffect.SetFloatVector("LightPos", Helpers::Globals::AppLight.GetPosition());
		solidEffect.SetFloatVector("LightDirection", Helpers::Globals::AppLight.GetDirection());
		solidEffect.SetFloat("A", this->A);
		solidEffect.SetFloat("B", this->B);
		solidEffect.SetFloat("rhoOverPi", this->rhoOverPi);
		solidEffect.SetFloatVector("LightColor", Helpers::Globals::AppLight.GetColor());
	}


	void Surface::Draw()
	{
/*		if(Helpers::Globals::SurfelDrawMethod == Helpers::SOLID){
			DrawSolid();
		}*/

		if(Helpers::Globals::SurfelRenderMethod == Helpers::WIREFRAME){
			DrawWireframe();
		}
		else if(Helpers::Globals::SurfelRenderMethod == Helpers::SOLID){
			DrawSurfel();
		}
	}

	void Surface::DrawSolid(){
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

	void Surface::DrawWireframe(){
		wireframeEffect.SetFloatVector("CameraPos", Helpers::Globals::AppCamera.Position());
		wireframeEffect.SetMatrix("View", Helpers::Globals::AppCamera.View());
		wireframeEffect.SetMatrix("Projection", Helpers::Globals::AppCamera.Projection());
		wireframeEffect.SetMatrix("World", world);
		wireframeEffect.SetTexture("SurfaceTexture", planeTexture);
		wireframeEffect.SetFloat("A", this->A);
		wireframeEffect.SetFloat("B", this->B);
		wireframeEffect.SetFloat("rhoOverPi", this->rhoOverPi);

		wireframeEffect.PreDraw();

		UINT stride = sizeof( Structs::SOLID_VERTEX);
		UINT offset = 0;

		Helpers::Globals::Device->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
		Helpers::Globals::Device->IASetVertexBuffers( 0, 1, &solidVertexBuffer, &stride, &offset );

		wireframeEffect.DrawAuto();
		Helpers::Globals::Device->RSSetState(SolidRenderState);
	}

	void Surface::DrawSurfel(){
		
		surfelEffect.SetFloatVector("CameraPos", Helpers::Globals::AppCamera.Position());
		surfelEffect.SetMatrix("View", Helpers::Globals::AppCamera.View());
		surfelEffect.SetMatrix("World", world);
		surfelEffect.SetFloat("RadiusScale", RadiusScale);
		surfelEffect.SetFloatVector("DeltaUV", DeltaSurfelUV);
		surfelEffect.SetTexture("SurfaceTexture", planeTexture);
		surfelEffect.SetFloat("A", this->A);
		surfelEffect.SetFloat("B", this->B);
		surfelEffect.SetFloat("rhoOverPi", this->rhoOverPi);
		surfelEffect.SetFloatVector("DeltaUV", DeltaSurfelUV);

		surfelEffect.PreDraw();

		UINT stride = sizeof( Structs::SURFEL_VERTEX);
		UINT offset = 0;

		Helpers::Globals::Device->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);
		Helpers::Globals::Device->IASetVertexBuffers( 0, 1, &surfelVertexBuffer, &stride, &offset );

		surfelEffect.Draw(surfelCount);

		surfelEdgeEffect.SetFloatVector("CameraPos", Helpers::Globals::AppCamera.Position());
		surfelEdgeEffect.SetMatrix("View", Helpers::Globals::AppCamera.View());
		surfelEdgeEffect.SetMatrix("World", world);
		surfelEdgeEffect.SetFloat("RadiusScale", RadiusScale);
		surfelEdgeEffect.SetFloatVector("DeltaUV", DeltaSurfelUV);
		surfelEdgeEffect.SetTexture("SurfaceTexture", planeTexture);
		surfelEdgeEffect.SetFloat("A", this->A);
		surfelEdgeEffect.SetFloat("B", this->B);
		surfelEdgeEffect.SetFloat("rhoOverPi", this->rhoOverPi);
		surfelEdgeEffect.SetFloatVector("DeltaUV", DeltaSurfelUV);

		surfelEdgeEffect.PreDraw();

		stride = sizeof( Structs::SURFEL_EDGE_VERTEX);
		offset = 0;

		Helpers::Globals::Device->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);
		Helpers::Globals::Device->IASetVertexBuffers( 0, 1, &surfelEdgeVertexBuffer, &stride, &offset );

		surfelEdgeEffect.Draw(edgeCount);

	}

	void Surface::Update(){

	}

	void Surface::CleanUp(){
		this->surfelVertexBuffer->Release();
		this->solidVertexBuffer->Release();
		this->readableVertexBuffer->Release();

		if(Surface::TextureLoaded){

			this->geometryEffect.CleanUp();
			this->solidEffect.CleanUp();
			this->surfelEffect.CleanUp();
			this->wireframeEffect.CleanUp();

			Surface::TextureLoaded = false;
		}

	}
}