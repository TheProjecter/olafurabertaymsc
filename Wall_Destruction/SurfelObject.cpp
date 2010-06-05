#include "SurfelObject.h"
#include "Globals.h"
#include "RandRange.h"

namespace Drawables{
	SurfelObject::SurfelObject(void){}

	SurfelObject::~SurfelObject(void){}

	void SurfelObject::Init(){
		surfelVertexBuffer = NULL;
		solidVertexBuffer = NULL;

		D3DXMatrixTranslation(&world, 0.0f, 0.0f, 0.0f);

		InitGeometryPass();
		InitSurfel();
		InitCommonSolidAndWireframe();
		InitWireframe();
		InitSolid();
		RandomizeSurfels();
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
		geometryEffect.AddVariable("TangentRotation");
		
		D3DXMATRIX tangentRotation;
		D3DXMatrixRotationYawPitchRoll(&tangentRotation, Helpers::Globals::HALF_PI, Helpers::Globals::HALF_PI, Helpers::Globals::HALF_PI);

		geometryEffect.SetMatrix("TangentRotation", tangentRotation);
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

		/*
		D3D10_BUFFER_DESC vbdesc =
		{
			sizeof(SURFEL_VERTEX),
			D3D10_USAGE_DYNAMIC,
			D3D10_BIND_VERTEX_BUFFER | D3D10_BIND_STREAM_OUTPUT,
			D3D10_CPU_ACCESS_WRITE,
			0
		};

		HR(Helpers::Globals::Device->CreateBuffer( &vbdesc, NULL, &dynamicSurfelVertexBuffer));
		
		SURFEL_VERTEX *tmpVertices = NULL;
		HRESULT hr = dynamicSurfelVertexBuffer->Map(D3D10_MAP_WRITE_DISCARD, 0, (void**)&tmpVertices);
		if(SUCCEEDED(hr)){
			CopyMemory(tmpVertices, vertices, sizeof( vertices ) );
			dynamicSurfelVertexBuffer->Unmap();
		}
		*/
		// create the effect
		surfelEffect = Helpers::CustomEffect("Surfels.fx", "SurfelTechnique", CUSTOM_EFFECT_TYPE_PIXEL | CUSTOM_EFFECT_TYPE_GEOMETRY | CUSTOM_EFFECT_TYPE_VERTEX , layout, 3);
		
		surfelEffect.AddVariable("World");
		surfelEffect.AddVariable("View");
		surfelEffect.AddVariable("Projection");
		surfelEffect.AddVariable("TangentRotation");
		surfelEffect.AddTexture("SurfelTexture", "surfel.png");

		D3DXMATRIX tangentRotation;
		D3DXMatrixRotationYawPitchRoll(&tangentRotation, Helpers::Globals::HALF_PI, Helpers::Globals::HALF_PI, Helpers::Globals::HALF_PI);
		
		surfelEffect.SetMatrix("TangentRotation", tangentRotation);
		surfelEffect.SetTexture("SurfelTexture", "surfel.png");
		surfelEffect.SetMatrix("World", world);
	}

	void SurfelObject::RandomizeSurfels(){
		if(surfelVertexBuffer){
			surfelVertexBuffer->Release();
			surfelVertexBuffer = NULL;
		}

		SURFEL_VERTEX vertices[] =
		{
			{D3DXVECTOR3( RandRange::Rand(-5.0f, 5.0f), RandRange::Rand(-5.0f, 5.0f), 0.0f ), D3DXVECTOR3( 0.0f, 0.0f, 1.0f ), D3DXVECTOR2(RandRange::Rand(3.0f, 10.0f), RandRange::Rand(3.0f, 10.0f))},
			{D3DXVECTOR3( RandRange::Rand(-5.0f, 5.0f), RandRange::Rand(-5.0f, 5.0f), 0.0f ), D3DXVECTOR3( 0.0f, 0.0f, 1.0f ), D3DXVECTOR2(RandRange::Rand(3.0f, 10.0f), RandRange::Rand(3.0f, 10.0f))},
		};

		D3D10_BUFFER_DESC bd;
		bd.Usage = D3D10_USAGE_DEFAULT;
		bd.ByteWidth = sizeof( vertices );
		bd.BindFlags = D3D10_BIND_VERTEX_BUFFER | D3D10_BIND_STREAM_OUTPUT;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		D3D10_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = vertices;

		HR(Helpers::Globals::Device->CreateBuffer( &bd, &InitData, &surfelVertexBuffer ));	

		geometryEffect.WriteToGeometryShader(D3D10_PRIMITIVE_TOPOLOGY_POINTLIST, surfelVertexBuffer, 2, solidVertexBuffer);
	}

	void SurfelObject::InitCommonSolidAndWireframe(){	

		D3D10_BUFFER_DESC vbdesc =
		{
			100 * sizeof(SOLID_VERTEX),
			D3D10_USAGE_DEFAULT,
			D3D10_BIND_VERTEX_BUFFER | D3D10_BIND_STREAM_OUTPUT,
			0,
			0
		};

		HR( Helpers::Globals::Device->CreateBuffer( &vbdesc, NULL, &solidVertexBuffer ) );		
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
		wireframeEffect.AddVariable("TangentRotation");
		wireframeEffect.AddTexture("SurfelTexture", "surfel_non_transparent.jpg");

		D3DXMATRIX tangentRotation;
		D3DXMatrixRotationYawPitchRoll(&tangentRotation, Helpers::Globals::HALF_PI, Helpers::Globals::HALF_PI, Helpers::Globals::HALF_PI);

		wireframeEffect.SetMatrix("TangentRotation", tangentRotation);
		wireframeEffect.SetTexture("SurfelTexture", "surfel_non_transparent.jpg");
		wireframeEffect.SetMatrix("World", world);

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
		solidEffect.AddVariable("TangentRotation");
		solidEffect.AddTexture("SurfelTexture", "surfel_non_transparent.jpg");

		D3DXMATRIX tangentRotation;
		D3DXMatrixRotationYawPitchRoll(&tangentRotation, Helpers::Globals::HALF_PI, Helpers::Globals::HALF_PI, Helpers::Globals::HALF_PI);

		solidEffect.SetMatrix("TangentRotation", tangentRotation);
		solidEffect.SetTexture("SurfelTexture", "surfel_non_transparent.jpg");
		solidEffect.SetMatrix("World", world);
	}

	void SurfelObject::Draw(){
		if(drawMethod == SOLID){
			DrawSolid();
		}
		else if(drawMethod == WIREFRAME){
			DrawWireframe();
		}
		else if(drawMethod == SURFEL){
			DrawSurfel();
		}
	}

	void SurfelObject::DrawSolid(){
		solidEffect.SetMatrix("View", Helpers::Globals::AppCamera.View());
		solidEffect.SetMatrix("Projection", Helpers::Globals::AppCamera.Projection());

		solidEffect.PreDraw();

		UINT stride = sizeof( SOLID_VERTEX);
		UINT offset = 0;

		Helpers::Globals::Device->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		Helpers::Globals::Device->IASetVertexBuffers( 0, 1, &solidVertexBuffer, &stride, &offset );

		solidEffect.DrawAuto();
	}

	void SurfelObject::DrawWireframe(){
		wireframeEffect.SetMatrix("View", Helpers::Globals::AppCamera.View());
		wireframeEffect.SetMatrix("Projection", Helpers::Globals::AppCamera.Projection());

		wireframeEffect.PreDraw();

		UINT stride = sizeof( SOLID_VERTEX);
		UINT offset = 0;

		Helpers::Globals::Device->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
		Helpers::Globals::Device->IASetVertexBuffers( 0, 1, &solidVertexBuffer, &stride, &offset );

		wireframeEffect.DrawAuto();
		Helpers::Globals::Device->RSSetState(SolidRenderState);
	}

	void SurfelObject::DrawSurfel(){
	
		surfelEffect.SetMatrix("View", Helpers::Globals::AppCamera.View());
		surfelEffect.SetMatrix("Projection", Helpers::Globals::AppCamera.Projection());

		surfelEffect.PreDraw();
	
		UINT stride = sizeof( SURFEL_VERTEX);
		UINT offset = 0;
		
		Helpers::Globals::Device->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);
		Helpers::Globals::Device->IASetVertexBuffers( 0, 1, &surfelVertexBuffer, &stride, &offset );

		surfelEffect.Draw(2);//(1);
	}

	void SurfelObject::CleanUp(){
		surfelEffect.CleanUp();
		solidEffect.CleanUp();
		wireframeEffect.CleanUp();
		
		surfelVertexBuffer->Release();
		solidVertexBuffer->Release();
	}
}



		


/*#include "SurfelObject.h"
#include "Globals.h"

namespace Drawables{
	SurfelObject::SurfelObject(void){}

	SurfelObject::~SurfelObject(void){}

	void SurfelObject::Init(){

		D3DXMatrixTranslation(&world, 10.0f, 0.0f, 0.0f);

    	InitGeometryPass();
		InitSurfel();
		InitCommonSolidAndWireframe();
		//InitWireframe();
		//InitSolid();
		//DrawToGeometry();
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
		geometryEffect.AddVariable("TangentRotation");
		
		D3DXMATRIX tangentRotation;
		D3DXMatrixRotationYawPitchRoll(&tangentRotation, Helpers::Globals::HALF_PI, Helpers::Globals::HALF_PI, Helpers::Globals::HALF_PI);

		geometryEffect.SetMatrix("TangentRotation", tangentRotation);
		geometryEffect.SetMatrix("World", world);

		SOLID_VERTEX v = {D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(0.0f, 0.0f)};

		geometryEffect.SetupGeometryStreamOut(v, 4);
	}

	void SurfelObject::InitSurfel(){
		// Define the input layout
		D3D10_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 }, // pos
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(D3DXVECTOR3), D3D10_INPUT_PER_VERTEX_DATA, 0 }, // normal
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 2*sizeof(D3DXVECTOR3), D3D10_INPUT_PER_VERTEX_DATA, 0 }, // dimensions
		};

		SURFEL_VERTEX vertices[] =
		{
			{D3DXVECTOR3( 0.0f, 0.0f, 0.0f ), D3DXVECTOR3( 0.0f, 0.0f, 1.0f ), D3DXVECTOR2(5.0f, 10.0f)},
			{D3DXVECTOR3( 1.0f, 5.0f, 0.0f ), D3DXVECTOR3( 0.0f, 0.0f, 1.0f ), D3DXVECTOR2(10.0f, 5.0f)},
		};

	    solidVertexBuffer = geometryEffect.WriteToGeometryShader(D3D10_PRIMITIVE_TOPOLOGY_POINTLIST, vertices, 2);

		D3D10_BUFFER_DESC bd;
		bd.Usage = D3D10_USAGE_DEFAULT;
		bd.ByteWidth = sizeof( vertices );
		bd.BindFlags = D3D10_BIND_VERTEX_BUFFER | D3D10_BIND_STREAM_OUTPUT;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		D3D10_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = vertices;

		HR(Helpers::Globals::Device->CreateBuffer( &bd, &InitData, &surfelVertexBuffer ));	

		// create the effect
		surfelEffect = Helpers::CustomEffect("Surfels.fx", "SurfelTechnique", CUSTOM_EFFECT_TYPE_PIXEL | CUSTOM_EFFECT_TYPE_GEOMETRY | CUSTOM_EFFECT_TYPE_VERTEX , layout, 3);
		
		surfelEffect.AddVariable("World");
		surfelEffect.AddVariable("View");
		surfelEffect.AddVariable("Projection");
		surfelEffect.AddVariable("TangentRotation");
		surfelEffect.AddTexture("SurfelTexture", "surfel.png");

		D3DXMATRIX tangentRotation;
		D3DXMatrixRotationYawPitchRoll(&tangentRotation, Helpers::Globals::HALF_PI, Helpers::Globals::HALF_PI, Helpers::Globals::HALF_PI);
		
		surfelEffect.SetMatrix("TangentRotation", tangentRotation);
		surfelEffect.SetTexture("SurfelTexture", "surfel.png");
		surfelEffect.SetMatrix("World", world);
	}

	void SurfelObject::DrawToGeometry(){
		
		// Set IA parameters
		ID3D10Buffer* pBuffers[1];
		pBuffers[0] = surfelVertexBuffer;

		UINT stride[1] = { sizeof(SURFEL_VERTEX) };
		UINT offset[1] = { 0 };
		Helpers::Globals::Device->IASetVertexBuffers( 0, 1, pBuffers, stride, offset );
		Helpers::Globals::Device->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_POINTLIST );

		// Point to the correct output buffer
		pBuffers[0] = solidVertexBuffer;
		Helpers::Globals::Device->SOSetTargets( 1, pBuffers, offset );

		// Draw
		D3D10_TECHNIQUE_DESC techDesc;
		geometryEffect.GetTechnique()->GetDesc( &techDesc );

		for( UINT p = 0; p < techDesc.Passes; ++p )
		{
			geometryEffect.GetTechnique()->GetPassByIndex( p )->Apply( 0 );

			Helpers::Globals::Device->Draw( 2, 0 );			
		}

		// Get back to normal
		pBuffers[0] = NULL;
		Helpers::Globals::Device->SOSetTargets( 1, pBuffers, offset );
	}

	void SurfelObject::InitCommonSolidAndWireframe(){		
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
		wireframeEffect.AddVariable("TangentRotation");
		wireframeEffect.AddTexture("SurfelTexture", "surfel_non_transparent.jpg");

		D3DXMATRIX tangentRotation;
		D3DXMatrixRotationYawPitchRoll(&tangentRotation, Helpers::Globals::HALF_PI, Helpers::Globals::HALF_PI, Helpers::Globals::HALF_PI);

		wireframeEffect.SetMatrix("TangentRotation", tangentRotation);
		wireframeEffect.SetTexture("SurfelTexture", "surfel_non_transparent.jpg");
		wireframeEffect.SetMatrix("World", world);
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
		solidEffect.AddVariable("TangentRotation");
		solidEffect.AddTexture("SurfelTexture", "surfel_non_transparent.jpg");

		D3DXMATRIX tangentRotation;
		D3DXMatrixRotationYawPitchRoll(&tangentRotation, Helpers::Globals::HALF_PI, Helpers::Globals::HALF_PI, Helpers::Globals::HALF_PI);

		solidEffect.SetMatrix("TangentRotation", tangentRotation);
		solidEffect.SetTexture("SurfelTexture", "surfel_non_transparent.jpg");
		solidEffect.SetMatrix("World", world);
	}

	void SurfelObject::Draw(){
		if(drawMethod == SOLID){
			DrawSolid();
		}
		else if(drawMethod == WIREFRAME){
			DrawWireframe();
		}
		else if(drawMethod == SURFEL){
			DrawSurfel();
		}
	}

	void SurfelObject::DrawSolid(){
		solidEffect.SetMatrix("View", Helpers::Globals::AppCamera.View());
		solidEffect.SetMatrix("Projection", Helpers::Globals::AppCamera.Projection());

		solidEffect.PreDraw();

		UINT stride = sizeof( SOLID_VERTEX);
		UINT offset = 0;

		Helpers::Globals::Device->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
		Helpers::Globals::Device->IASetVertexBuffers( 0, 1, &solidVertexBuffer, &stride, &offset );

		solidEffect.Draw(8, 4);
	}

	void SurfelObject::DrawWireframe(){
		wireframeEffect.SetMatrix("View", Helpers::Globals::AppCamera.View());
		wireframeEffect.SetMatrix("Projection", Helpers::Globals::AppCamera.Projection());

		wireframeEffect.PreDraw();

		UINT stride = sizeof( SOLID_VERTEX);
		UINT offset = 0;

		Helpers::Globals::Device->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
		Helpers::Globals::Device->IASetVertexBuffers( 0, 1, &solidVertexBuffer, &stride, &offset );

		wireframeEffect.Draw(8, 4);
	}

	void SurfelObject::DrawSurfel(){
	
		surfelEffect.SetMatrix("View", Helpers::Globals::AppCamera.View());
		surfelEffect.SetMatrix("Projection", Helpers::Globals::AppCamera.Projection());

		surfelEffect.PreDraw();
	
		UINT stride = sizeof( SURFEL_VERTEX);
		UINT offset = 0;
		
		Helpers::Globals::Device->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);
		Helpers::Globals::Device->IASetVertexBuffers( 0, 1, &surfelVertexBuffer, &stride, &offset );

		surfelEffect.Draw(2);
	}

	void SurfelObject::CleanUp(){
		surfelEffect.CleanUp();
		solidEffect.CleanUp();
		wireframeEffect.CleanUp();
		
		surfelVertexBuffer->Release();
		solidVertexBuffer->Release();
	}
}*/



		
