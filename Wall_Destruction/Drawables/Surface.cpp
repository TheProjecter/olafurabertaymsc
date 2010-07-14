#include "Surface.h"
#include "TextureCreator.h"
#include "ChangedPhyxels.h"
#include "KeyboardHandler.h"
#include "MathHelper.h"

namespace Drawables{
		
	ID3D10ShaderResourceView *Surface::SurfelTexture, *Surface::SurfelWireframeTexture;
	ID3D10RasterizerState *Surface::SolidRenderState;
	Helpers::CustomEffect Surface::surfelEffect, Surface::surfelEdgeEffect, Surface::solidEffect, Surface::wireframeEffect, Surface::geometryEffect, Surface::geometryEdgeEffect;
	float Surface::RadiusScale = 1.0f, Surface::LastRadiusScale = 1.0f;

	bool Surface::TextureLoaded = false;

	Surface::Surface(void)
	{
		surfelCount = 0;
		edgeCount = 0;
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

		solidEffect.AddTexture("SurfaceTexture", planeTexture);
		solidEffect.SetTexture("SurfaceTexture", planeTexture);

		surfelVertexBuffer = NULL;
		solidVertexBuffer = NULL;

		SetMaterialInfo(0.416384f, 0.142109f);

		D3D10_BUFFER_DESC bd;
		bd.Usage = D3D10_USAGE_DEFAULT;
		bd.ByteWidth = surfelCount * sizeof( ProjectStructs::SURFEL_VERTEX );
		bd.BindFlags = D3D10_BIND_VERTEX_BUFFER | D3D10_BIND_STREAM_OUTPUT;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;

		ProjectStructs::SURFEL_VERTEX* sVertices = NULL;
		sVertices = new ProjectStructs::SURFEL_VERTEX[surfelCount];

		for(int i = 0; i<surfelCount; i++){
			sVertices[i] = surfaceSurfelsVertices[i];
		}

		D3D10_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = sVertices;

		HR(Helpers::Globals::Device->CreateBuffer( &bd, &InitData, &surfelVertexBuffer ));	

		D3D10_BUFFER_DESC ebd;
		ebd.Usage = D3D10_USAGE_DEFAULT;
		ebd.ByteWidth = edgeCount * sizeof( ProjectStructs::SURFEL_EDGE_VERTEX );
		ebd.BindFlags = D3D10_BIND_VERTEX_BUFFER | D3D10_BIND_STREAM_OUTPUT;
		ebd.CPUAccessFlags = 0;
		ebd.MiscFlags = 0;

		ProjectStructs::SURFEL_EDGE_VERTEX* sEdge= NULL;
		sEdge = new ProjectStructs::SURFEL_EDGE_VERTEX[edgeCount];

		for(int i = 0; i<edgeCount; i++){
			sEdge[i] = edgeSurfelsVertices[i];
		}

		D3D10_SUBRESOURCE_DATA edgeInitData;
		edgeInitData.pSysMem = sEdge;
		HR(Helpers::Globals::Device->CreateBuffer( &ebd, &edgeInitData, &surfelEdgeVertexBuffer ));	

		InitCommonSolidAndWireframe();

		geometryEffect.SetFloat("RadiusScale", RadiusScale);
		geometryEffect.SetFloatVector("DeltaUV", DeltaSurfelUV);
		geometryEffect.WriteToGeometryShader(D3D10_PRIMITIVE_TOPOLOGY_POINTLIST, surfelVertexBuffer, surfelCount, solidVertexBuffer);

		geometryEdgeEffect.SetFloat("RadiusScale", RadiusScale);
		geometryEdgeEffect.SetFloatVector("DeltaUV", DeltaSurfelUV);
		geometryEdgeEffect.WriteToGeometryShader(D3D10_PRIMITIVE_TOPOLOGY_POINTLIST, surfelEdgeVertexBuffer, edgeCount, solidEdgeVertexBuffer);

		Helpers::Globals::Device->CopyResource(readableVertexBuffer, solidVertexBuffer);
		Helpers::Globals::Device->CopyResource(readableEdgeVertexBuffer, solidEdgeVertexBuffer);
	}

	void Surface::AddSurfel(ProjectStructs::SURFEL *s){

		surfelCount++;
		surfaceSurfels.push_back(s);
		surfaceSurfelsVertices.push_back(s->vertex);
	} 

	void Surface::AddEdgeSurfel(ProjectStructs::SURFEL_EDGE *s){
		edgeCount++;

		edgeSurfels.push_back(s);
		edgeSurfelsVertices.push_back(s->vertex);
	}

	void Surface::AddForce(D3DXVECTOR3 force, D3DXVECTOR3 pos, int index){
		D3DXVECTOR3 direction;
		D3DXVec3Normalize(&direction, &force);
		direction.x = ceil(direction.x);
		direction.y = ceil(direction.y);
		direction.z = ceil(direction.z);

		for(int i = 0; i < surfaceSurfels[index]->intersectingCells.size(); i++){
			AddForceToPhyxel(force, pos, direction, surfaceSurfels[index]->intersectingCells[i]);
		}
	}

	void Surface::AddForceToPhyxel(D3DXVECTOR3 force, D3DXVECTOR3 pos, D3DXVECTOR3 direction, ProjectStructs::Phyxel_Grid_Cell *cell){
		if(cell && cell->phyxel && !cell->phyxel->isChanged){

			if(pos == cell->phyxel->pos)
				return;

			//if(!MathHelper::Facing(cell->phyxel->pos, pos, direction))
			//	return;

			D3DXVECTOR3 f = force / D3DXVec3Length(&(pos - cell->phyxel->pos));
			
			if(D3DXVec3Length(&force) < 100.0f || (MathHelper::Sign(f.x) != MathHelper::Sign(direction.x) && 
				MathHelper::Sign(f.y) != MathHelper::Sign(direction.y) && 
				MathHelper::Sign(f.z) != MathHelper::Sign(direction.z) )){
				return;
			}		

			cell->phyxel->force.x += f.x;					
			cell->phyxel->force.y += f.y;					
			cell->phyxel->force.z += f.z;			

			ChangedPhyxels::AddPhyxel(cell->phyxel);
			
			for(int i = 0; i<3; i++){
				for(int j = 0; j<3; j++){
					for(int k = 0; k<3; k++){
						if((direction.x < 0 && i == 0) || (direction.x > 0 && i == 2) || (direction.y < 0 && j == 0) || 
							(direction.y > 0 && j == 2) || (direction.z < 0 && k == 0) || (direction.z > 0 && k == 2))
							AddForceToPhyxel(f / 10.0f, cell->phyxel->pos, direction, cell->neighbours(i, j, k));
					}
				}
			}
		}
			
	}

	void Surface::InitGeometryPass(){
		// Define the input layout
		D3D10_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 }, // pos
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(D3DXVECTOR3), D3D10_INPUT_PER_VERTEX_DATA, 0 }, // normal
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 2*sizeof(D3DXVECTOR3), D3D10_INPUT_PER_VERTEX_DATA, 0 }, // MajorRadius
			{ "TEXCOORD", 1, DXGI_FORMAT_R32G32B32_FLOAT, 0, 3*sizeof(D3DXVECTOR3), D3D10_INPUT_PER_VERTEX_DATA, 0 }, // MinorRadius
			{ "TEXCOORD", 2, DXGI_FORMAT_R32G32_FLOAT, 0, 4*sizeof(D3DXVECTOR3), D3D10_INPUT_PER_VERTEX_DATA, 0 }, // UV
		};

		// create the effect
		geometryEffect = Helpers::CustomEffect("SurfelSplatter.fx", "GeometryTechnique", CUSTOM_EFFECT_TYPE_PIXEL | CUSTOM_EFFECT_TYPE_GEOMETRY | CUSTOM_EFFECT_TYPE_VERTEX , layout, 5);

		geometryEffect.AddVariable("World");
		geometryEffect.AddVariable("View");
		geometryEffect.AddVariable("Projection");
		geometryEffect.AddVariable("RadiusScale");
		geometryEffect.AddVariable("DeltaUV");

		geometryEffect.SetMatrix("World", world);

		// Define the input layout
		D3D10_INPUT_ELEMENT_DESC edgeLayout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 }, // pos
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(D3DXVECTOR3), D3D10_INPUT_PER_VERTEX_DATA, 0 }, // normal
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 2*sizeof(D3DXVECTOR3), D3D10_INPUT_PER_VERTEX_DATA, 0 }, // MajorRadius
			{ "TEXCOORD", 1, DXGI_FORMAT_R32G32B32_FLOAT, 0, 3*sizeof(D3DXVECTOR3), D3D10_INPUT_PER_VERTEX_DATA, 0 }, // MinorRadius
			{ "TEXCOORD", 2, DXGI_FORMAT_R32G32B32_FLOAT, 0, 4*sizeof(D3DXVECTOR3), D3D10_INPUT_PER_VERTEX_DATA, 0 }, //ClipPlane 
			{ "TEXCOORD", 3, DXGI_FORMAT_R32G32_FLOAT, 0, 5*sizeof(D3DXVECTOR3), D3D10_INPUT_PER_VERTEX_DATA, 0 }, // UV
		};
		geometryEdgeEffect = Helpers::CustomEffect("SurfelSplatter.fx", "GeometryEdgeTechnique", CUSTOM_EFFECT_TYPE_PIXEL | CUSTOM_EFFECT_TYPE_GEOMETRY | CUSTOM_EFFECT_TYPE_VERTEX , edgeLayout, 6);

		geometryEdgeEffect.AddVariable("World");
		geometryEdgeEffect.AddVariable("View");
		geometryEdgeEffect.AddVariable("Projection");
		geometryEdgeEffect.AddVariable("RadiusScale");
		geometryEdgeEffect.AddVariable("DeltaUV");

		geometryEdgeEffect.SetMatrix("World", world);
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
			6 * surfelCount * sizeof(ProjectStructs::SOLID_VERTEX),
			D3D10_USAGE_DEFAULT,
			D3D10_BIND_VERTEX_BUFFER | D3D10_BIND_STREAM_OUTPUT,
			0,
			0
		};

		HR( Helpers::Globals::Device->CreateBuffer( &vbdesc, NULL, &solidVertexBuffer ) );		

		D3D10_BUFFER_DESC vbedgedesc =
		{
			6 * edgeCount* sizeof(ProjectStructs::SOLID_VERTEX),
			D3D10_USAGE_DEFAULT,
			D3D10_BIND_VERTEX_BUFFER | D3D10_BIND_STREAM_OUTPUT,
			0,
			0
		};
		HR( Helpers::Globals::Device->CreateBuffer( &vbedgedesc, NULL, &solidEdgeVertexBuffer) );		

		D3D10_BUFFER_DESC vbdesc2 =
		{
			6 * surfelCount * sizeof(ProjectStructs::SOLID_VERTEX),
			D3D10_USAGE_STAGING,
			0,
			D3D10_CPU_ACCESS_READ,
			0
		};

		HR( Helpers::Globals::Device->CreateBuffer( &vbdesc2, NULL, &readableVertexBuffer) );		

		D3D10_BUFFER_DESC vbedgedesc2 =
		{
			6 * edgeCount * sizeof(ProjectStructs::SOLID_VERTEX),
			D3D10_USAGE_STAGING,
			0,
			D3D10_CPU_ACCESS_READ,
			0
		};

		HR( Helpers::Globals::Device->CreateBuffer( &vbedgedesc2, NULL, &readableEdgeVertexBuffer) );		
	}

	void Surface::InitWireframe(){
		// Define the input layout
		D3D10_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 }, // pos
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(D3DXVECTOR3), D3D10_INPUT_PER_VERTEX_DATA, 0 }, // normal
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 2*sizeof(D3DXVECTOR3), D3D10_INPUT_PER_VERTEX_DATA, 0 }, // UV
			{ "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, 2*sizeof(D3DXVECTOR3) + sizeof(D3DXVECTOR2), D3D10_INPUT_PER_VERTEX_DATA, 0 }, //EWAUV;
		};

		// create the effect
		wireframeEffect = Helpers::CustomEffect("SurfelSplatter.fx", "WireframeTechnique", CUSTOM_EFFECT_TYPE_PIXEL | CUSTOM_EFFECT_TYPE_VERTEX , layout, 4);
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
		wireframeEffect.AddVariable("DeltaUV");

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
			{ "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, 2*sizeof(D3DXVECTOR3) + sizeof(D3DXVECTOR2), D3D10_INPUT_PER_VERTEX_DATA, 0 }, //EWAUV;
		};

		// create the effect
		solidEffect = Helpers::CustomEffect("SurfelSplatter.fx", "SolidTechnique", CUSTOM_EFFECT_TYPE_PIXEL | CUSTOM_EFFECT_TYPE_VERTEX , layout, 4);
		solidEffect.AddVariable("World");
		solidEffect.AddVariable("View");
		solidEffect.AddVariable("Projection");

		solidEffect.AddVariable("LightPos");
		solidEffect.AddVariable("AmbientColor");
		solidEffect.AddVariable("CameraPos");
		solidEffect.AddVariable("LightDirection");
		solidEffect.AddVariable("A");
		solidEffect.AddVariable("B");
		solidEffect.AddVariable("rhoOverPi");
		solidEffect.AddVariable("LightColor");
		solidEffect.AddVariable("EWATexture");
		solidEffect.AddVariable("DeltaUV");

		solidEffect.SetTexture("EWATexture", SurfelTexture);
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
		if(Helpers::Globals::SurfelRenderMethod == Helpers::WIREFRAME)
			DrawWireframe();
		else if(Helpers::Globals::SurfelRenderMethod == Helpers::SOLID)
			DrawSolid();
		else if(Helpers::Globals::SurfelRenderMethod == Helpers::SURFEL)
			DrawSurfel();
	}

	void Surface::DrawSolid(){
		solidEffect.SetFloatVector("CameraPos", Helpers::Globals::AppCamera.Position());
		solidEffect.SetMatrix("View", Helpers::Globals::AppCamera.View());
		solidEffect.SetMatrix("Projection", Helpers::Globals::AppCamera.Projection());
		solidEffect.SetMatrix("World", world);
		solidEffect.SetTexture("SurfaceTexture", planeTexture);
		solidEffect.SetFloat("A", this->A);
		solidEffect.SetFloat("B", this->B);
		solidEffect.SetFloat("rhoOverPi", this->rhoOverPi);
		solidEffect.PreDraw();

		UINT stride = sizeof( ProjectStructs::SOLID_VERTEX );
		UINT offset = 0;

		Helpers::Globals::Device->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		Helpers::Globals::Device->IASetVertexBuffers( 0, 1, &solidVertexBuffer, &stride, &offset );

		solidEffect.DrawAuto();

		Helpers::Globals::Device->IASetVertexBuffers( 0, 1, &solidEdgeVertexBuffer, &stride, &offset );

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

		UINT stride = sizeof( ProjectStructs::SOLID_VERTEX);
		UINT offset = 0;

		Helpers::Globals::Device->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		Helpers::Globals::Device->IASetVertexBuffers( 0, 1, &solidVertexBuffer, &stride, &offset );

		wireframeEffect.DrawAuto();

		Helpers::Globals::Device->IASetVertexBuffers( 0, 1, &solidEdgeVertexBuffer, &stride, &offset );

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

		surfelEffect.PreDraw();

		UINT stride = sizeof( ProjectStructs::SURFEL_VERTEX);
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

		stride = sizeof( ProjectStructs::SURFEL_EDGE_VERTEX);
		offset = 0;

		Helpers::Globals::Device->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);
		Helpers::Globals::Device->IASetVertexBuffers( 0, 1, &surfelEdgeVertexBuffer, &stride, &offset );

		surfelEdgeEffect.Draw(edgeCount);

	}

	void Surface::Update(float dt){
		if(RadiusScale != LastRadiusScale){
			// reset the solid vertex buffers
			geometryEffect.SetFloat("RadiusScale", RadiusScale);
			geometryEffect.SetFloatVector("DeltaUV", DeltaSurfelUV);
			geometryEffect.WriteToGeometryShader(D3D10_PRIMITIVE_TOPOLOGY_POINTLIST, surfelVertexBuffer, surfelCount, solidVertexBuffer);

			geometryEdgeEffect.SetFloat("RadiusScale", RadiusScale);
			geometryEdgeEffect.SetFloatVector("DeltaUV", DeltaSurfelUV);
			geometryEdgeEffect.WriteToGeometryShader(D3D10_PRIMITIVE_TOPOLOGY_POINTLIST, surfelEdgeVertexBuffer, edgeCount, solidEdgeVertexBuffer);

			Helpers::Globals::Device->CopyResource(readableVertexBuffer, solidVertexBuffer);
			Helpers::Globals::Device->CopyResource(readableEdgeVertexBuffer, solidEdgeVertexBuffer);
		}
	}

	void Surface::CleanUp(){

		if(Surface::TextureLoaded){

			geometryEffect.CleanUp();
			geometryEdgeEffect.CleanUp();
			solidEffect.CleanUp();
			surfelEffect.CleanUp();
			surfelEdgeEffect.CleanUp();
			wireframeEffect.CleanUp();

			Surface::TextureLoaded = false;
		}

		for(unsigned int i = 0; i<this->surfaceSurfels.size(); i++){
			delete this->surfaceSurfels[i];
		}

		for(unsigned int i = 0; i<this->edgeSurfels.size(); i++){
			delete this->edgeSurfels[i];
		}

		this->surfaceSurfels.clear();
		this->surfaceSurfels.swap(std::vector<ProjectStructs::SURFEL*>());

		this->edgeSurfels.clear();
		this->edgeSurfels.swap(std::vector<ProjectStructs::SURFEL_EDGE*>());

		this->surfelVertexBuffer->Release();
		this->surfelVertexBuffer = NULL;
		delete this->surfelVertexBuffer;

		this->solidVertexBuffer->Release();
		this->solidVertexBuffer= NULL;
		delete this->solidVertexBuffer;

		this->readableVertexBuffer->Release();
		this->readableVertexBuffer= NULL;
		delete this->readableVertexBuffer;

		this->readableEdgeVertexBuffer->Release();
		this->readableEdgeVertexBuffer= NULL;
		delete this->readableEdgeVertexBuffer;

		this->solidEdgeVertexBuffer->Release();
		this->solidEdgeVertexBuffer= NULL;
		delete this->solidEdgeVertexBuffer;
	}
}