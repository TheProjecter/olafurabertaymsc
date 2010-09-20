#include "Surface.h"
#include "TextureCreator.h"
#include "DeletedStructHolder.h"
#include "ImpactList.h"
#include "KeyboardHandler.h"
#include "MathHelper.h"
#include "PhysicsWrapper.h"
#include "DebugToFile.h"
#include "FractureManager.h"
#include "Algorithms.h"
#include "RandRange.h"

namespace Drawables{
	
	ID3D10ShaderResourceView *Surface::SurfelTexture, *Surface::SurfelWireframeTexture;
	ID3D10RasterizerState *Surface::SolidRenderState;
	Helpers::CustomEffect Surface::surfelEffect, Surface::solidEffect, Surface::wireframeEffect, Surface::geometryEffect, Surface::simpleEffect;
	float Surface::RadiusScale = 1.0f;
	bool Surface::isChanged = false;
	bool Surface::TextureLoaded = false;

	Surface::Surface(void)
	{
		this->grid = NULL;
	}

	Surface::~Surface(void)
	{

	}

	void Surface::Init(ProjectStructs::MATERIAL_PROPERTIES materialProperties){

		this->materialProperties = materialProperties;

		if(!Surface::TextureLoaded){
			SurfelTexture = TextureCreator::CreateSurfelTexture();
			SurfelWireframeTexture = TextureCreator::CreateSurfelWireframeTexture();

			Surface::TextureLoaded = true;

			InitGeometryPass();
			InitSurfel();
			InitWireframe();
			InitSolid();
			SetUpNeighborEffect();
		}

		surfelEffect.AddTexture("SurfaceTexture", planeTexture);
		surfelEffect.SetTexture("SurfaceTexture", planeTexture);

		wireframeEffect.AddTexture("SurfaceTexture", planeTexture);
		wireframeEffect.SetTexture("SurfaceTexture", planeTexture);

		solidEffect.AddTexture("SurfaceTexture", planeTexture);
		solidEffect.SetTexture("SurfaceTexture", planeTexture);

		surfelVertexBuffer = NULL;
		solidVertexBuffer = NULL;
		neighborVertexBuffer= NULL;

		SetMaterialInfo(0.416384f, 0.142109f);

		neighborDrawSet = false;

		ResetBuffers();
	}

	void Surface::ResetBuffers(){
		DebugToFile::StartTimer();

		float lastScale = RadiusScale;
		RadiusScale = 1.0f;

		neighborDrawSet = false;
		D3D10_BUFFER_DESC bd;
		bd.Usage = D3D10_USAGE_DEFAULT;
		bd.ByteWidth = surfaceSurfels.size() * sizeof( ProjectStructs::SURFEL_VERTEX );
		bd.BindFlags = D3D10_BIND_VERTEX_BUFFER | D3D10_BIND_STREAM_OUTPUT;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;

		ProjectStructs::SURFEL_VERTEX* sVertices = NULL;
		sVertices = new ProjectStructs::SURFEL_VERTEX[surfaceSurfels.size()];

		for(unsigned int i = 0; i<surfaceSurfels.size(); i++){
			sVertices[i] = *surfaceSurfels[i]->vertex;
		}

		D3D10_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = sVertices;
		HR(Helpers::Globals::Device->CreateBuffer( &bd, &InitData, &surfelVertexBuffer ));	

		InitCommonSolidAndWireframe();

		DrawToReadableBuffer();
		DebugToFile::EndTimer("Draw to readable buffer");

		DebugToFile::StartTimer();

		// release the rigid bodies
		for(unsigned int i = 0; i<rigidBodies.size(); i++){
			PhysicsWrapper::RemoveRigidBody(rigidBodies[i]);
		}
		rigidBodies.clear();

//		PhysicsWrapper::AddSurface(this);
		DebugToFile::EndTimer("rigid body stuff");

		DebugToFile::StartTimer();
		PushSurfelsIntoGrid();
		DebugToFile::EndTimer("push surfels to grid");
		RadiusScale = lastScale;

		DrawToReadableBuffer();
	}

	void Surface::PushSurfelsIntoGrid(){
		std::vector<D3DXVECTOR3> surfelVertexList;
		std::vector<ProjectStructs::SURFEL*> surfelList;

		// read from the surfel vertex buffer
		ProjectStructs::SOLID_VERTEX* vertices = 0;
		HR(GetSurfelReadableBuffer()->Map(D3D10_MAP_READ, 0, reinterpret_cast< void** >(&vertices)));
		GetSurfelReadableBuffer()->Unmap();

		int count = 0;
		for(unsigned int i = 0; i < newSurfelsForPhyxelGrid.size(); i++){
			int index = newSurfelsForPhyxelGrid[i];
			surfelVertexList.push_back(vertices[index*6].pos);
			surfelList.push_back(surfaceSurfels[index]);
			surfelVertexList.push_back(vertices[index*6+1].pos);
			surfelList.push_back(surfaceSurfels[index]);
			surfelVertexList.push_back(vertices[index*6+2].pos);
			surfelList.push_back(surfaceSurfels[index]);
			surfelVertexList.push_back(vertices[index*6+3].pos);
			surfelList.push_back(surfaceSurfels[index]);
			surfelVertexList.push_back(vertices[index*6+4].pos);
			surfelList.push_back(surfaceSurfels[index]);
			surfelVertexList.push_back(vertices[index*6+5].pos);
			surfelList.push_back(surfaceSurfels[index]);
		}

		grid->InsertPoints(surfelVertexList, surfelList);

		newSurfelsForPhyxelGrid.clear();
	}

	void Surface::AddSurfel(ProjectStructs::SURFEL *s){
		if(s->vertex->frontFacing == -1){
			D3DXVECTOR3 normalCheck;
			D3DXVec3Cross(&normalCheck, &s->vertex->minorAxis, &s->vertex->majorAxis);
			s->vertex->frontFacing = MathHelper::Sign(normalCheck.x) == MathHelper::Sign(s->vertex->normal.x) &&
				MathHelper::Sign(normalCheck.y) == MathHelper::Sign(s->vertex->normal.y) &&
				MathHelper::Sign(normalCheck.z) == MathHelper::Sign(s->vertex->normal.z) ? 1 : 0;
		}

		if(grid){
			newSurfelsForPhyxelGrid.push_back(surfaceSurfels.size());
		}

		surfaceSurfels.push_back(s);
	} 

	void Surface::AddForce(D3DXVECTOR3 force, D3DXVECTOR3 pos, ProjectStructs::SURFEL* surfel){
		if(!materialProperties.deformable)
			return;

		D3DXVECTOR3 direction;
		D3DXVec3Normalize(&direction, &force);
		direction.x = ceil(direction.x);
		direction.y = ceil(direction.y);
		direction.z = ceil(direction.z);

		for(unsigned int i = 0; i < surfel->intersectingCells.size(); i++){
			AddForceToPhyxels(force, pos, direction, surfel->intersectingCells[i]->phyxel, surfel);			
		}
	}

	void Surface::AddForceToPhyxels(D3DXVECTOR3 force, D3DXVECTOR3 pos, D3DXVECTOR3 direction, ProjectStructs::PHYXEL_NODE *phyxel, ProjectStructs::SURFEL* surfel){
		if(phyxel != NULL || force == MathHelper::GetZeroVector()){
	
			std::vector<ProjectStructs::PHYXEL_NODE*> goToNeighborPhyxels;
			for(unsigned int i = 0; i < phyxel->neighbours.GetSize(); i++){
				if(phyxel->neighbours[i] && !phyxel->neighbours[i]->isChanged && AddForceToPhyxel(force, pos, direction, phyxel->neighbours[i], surfel)){
					goToNeighborPhyxels.push_back(phyxel->neighbours[i]);
				}
			}

			for(unsigned int i = 0; i < goToNeighborPhyxels.size(); i++){
				AddForceToPhyxels(force, pos, direction, goToNeighborPhyxels[i], surfel);
			}
		}
	}

	bool Surface::AddForceToPhyxel(D3DXVECTOR3 force, D3DXVECTOR3 pos, D3DXVECTOR3 direction, ProjectStructs::PHYXEL_NODE *phyxel, ProjectStructs::SURFEL* surfel){

		D3DXVECTOR3 phyxelPos = phyxel->pos;

		D3DXVECTOR3 f = force * FractureManager::CalculateWeight(pos, phyxelPos, phyxel->supportRadius);

		if(D3DXVec3Length(&f) < 0.001f)
			return false;	

		if(phyxel->isChanged){
			phyxel->force.x += f.x;					
			phyxel->force.y += f.y;					
			phyxel->force.z += f.z;	
		}
		else{
			phyxel->force.x = f.x;					
			phyxel->force.y = f.y;					
			phyxel->force.z = f.z;	
		}

		phyxel->isChanged = true;

		ProjectStructs::IMPACT* impact = new ProjectStructs::IMPACT;
		impact->phyxel = phyxel;
		impact->impactPos = pos;
		impact->surfel = surfel;

		//impactList->AddImpact(impact);		

		return true;
	}

	void Surface::InitGeometryPass(){
		// Define the input layout
		D3D10_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 }, // pos
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(D3DXVECTOR3), D3D10_INPUT_PER_VERTEX_DATA, 0 }, // normal
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 2*sizeof(D3DXVECTOR3), D3D10_INPUT_PER_VERTEX_DATA, 0 }, // MajorRadius
			{ "TEXCOORD", 1, DXGI_FORMAT_R32G32B32_FLOAT, 0, 3*sizeof(D3DXVECTOR3), D3D10_INPUT_PER_VERTEX_DATA, 0 }, // MinorRadius
			{ "TEXCOORD", 3, DXGI_FORMAT_R32G32_FLOAT, 0, 4*sizeof(D3DXVECTOR3), D3D10_INPUT_PER_VERTEX_DATA, 0 }, // UV
			{ "TEXCOORD", 4, DXGI_FORMAT_R32G32_FLOAT, 0, 4*sizeof(D3DXVECTOR3) + sizeof(D3DXVECTOR2), D3D10_INPUT_PER_VERTEX_DATA, 0 }, // delta UV
			{ "TEXCOORD", 5, DXGI_FORMAT_R32_UINT, 0, 4*sizeof(D3DXVECTOR3) + 2*sizeof(D3DXVECTOR2), D3D10_INPUT_PER_VERTEX_DATA, 0 }, // frontFacing
			{ "TEXCOORD", 2, DXGI_FORMAT_R32G32B32_FLOAT, 0, 4*sizeof(D3DXVECTOR3) + 2*sizeof(D3DXVECTOR2) + sizeof(UINT32), D3D10_INPUT_PER_VERTEX_DATA, 0 }, //ClipPlane 
		};

		// create the effect
		geometryEffect = Helpers::CustomEffect("SurfelSplatter.fx", "GeometryTechnique", CUSTOM_EFFECT_TYPE_PIXEL | CUSTOM_EFFECT_TYPE_GEOMETRY | CUSTOM_EFFECT_TYPE_VERTEX , layout, 8);

		geometryEffect.AddVariable("World");
		geometryEffect.AddVariable("RadiusScale");

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
			{ "TEXCOORD", 3, DXGI_FORMAT_R32G32_FLOAT, 0, 4*sizeof(D3DXVECTOR3), D3D10_INPUT_PER_VERTEX_DATA, 0 }, // CenterUV
			{ "TEXCOORD", 4, DXGI_FORMAT_R32G32_FLOAT, 0, 4*sizeof(D3DXVECTOR3) + sizeof(D3DXVECTOR2), D3D10_INPUT_PER_VERTEX_DATA, 0 }, // delta UV
			{ "TEXCOORD", 5, DXGI_FORMAT_R32_UINT, 0, 4*sizeof(D3DXVECTOR3) + 2*sizeof(D3DXVECTOR2), D3D10_INPUT_PER_VERTEX_DATA, 0 }, // frontFacing
			{ "TEXCOORD", 2, DXGI_FORMAT_R32G32B32_FLOAT, 0, 4*sizeof(D3DXVECTOR3) + 2*sizeof(D3DXVECTOR2) + sizeof(UINT32), D3D10_INPUT_PER_VERTEX_DATA, 0 }, //ClipPlane 
		};

		surfelEffect = Helpers::CustomEffect("SurfelSplatter.fx", "SurfelTechnique", 0, layout, 8);
		surfelEffect.AddVariable("World");
		surfelEffect.AddVariable("View");
		surfelEffect.AddVariable("Projection");
		surfelEffect.AddVariable("RadiusScale");
		surfelEffect.AddVariable("EWATexture");

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
	}

	void Surface::InitCommonSolidAndWireframe(){	

		D3D10_BUFFER_DESC vbdesc =
		{
			6 * surfaceSurfels.size() * sizeof(ProjectStructs::SOLID_VERTEX),
			D3D10_USAGE_DEFAULT,
			D3D10_BIND_VERTEX_BUFFER | D3D10_BIND_STREAM_OUTPUT,
			0,
			0
		};

		HR( Helpers::Globals::Device->CreateBuffer( &vbdesc, NULL, &solidVertexBuffer ) );		


		D3D10_BUFFER_DESC vbdesc2 =
		{
			6 * surfaceSurfels.size() * sizeof(ProjectStructs::SOLID_VERTEX),
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

		solidEffect.SetTexture("EWATexture", SurfelTexture);
		solidEffect.SetFloatVector("AmbientColor", Helpers::Globals::AppLight.GetAmbientColor());
		solidEffect.SetFloatVector("LightPos", Helpers::Globals::AppLight.GetPosition());
		solidEffect.SetFloatVector("LightDirection", Helpers::Globals::AppLight.GetDirection());
		solidEffect.SetFloat("A", this->A);
		solidEffect.SetFloat("B", this->B);
		solidEffect.SetFloat("rhoOverPi", this->rhoOverPi);
		solidEffect.SetFloatVector("LightColor", Helpers::Globals::AppLight.GetColor());
	}

	void Surface::SetUpNeighborEffect(){
		// Define the input layout
		D3D10_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 }, // pos			
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(D3DXVECTOR3), D3D10_INPUT_PER_VERTEX_DATA, 0 }, //color;
		};

		simpleEffect = Helpers::CustomEffect("Simple.fx", "SimpleTechniqueWithColor", CUSTOM_EFFECT_TYPE_PIXEL | CUSTOM_EFFECT_TYPE_VERTEX , layout, 2);
		simpleEffect.AddVariable("World");
		simpleEffect.AddVariable("View");
		simpleEffect.AddVariable("Projection");
	}

	void Surface::Draw()
	{
		if(Helpers::Globals::SurfelRenderMethod == Helpers::WIREFRAME)
			DrawWireframe();
		else if(Helpers::Globals::SurfelRenderMethod == Helpers::SOLID)
			DrawSolid();
		else if(Helpers::Globals::SurfelRenderMethod == Helpers::SURFEL)
			DrawSurfel();

		if(Helpers::Globals::DRAW_NEIGHBORS && materialProperties.deformable){
			DrawNeighbors();
		}
	}

	struct VertexWithPosAndColor{
		D3DXVECTOR3 pos;
		D3DXVECTOR3 color;
	};

	void Surface::SetUpNeighborDraw(){

		std::vector<VertexWithPosAndColor> verticeList = std::vector<VertexWithPosAndColor>();

		D3DXVECTOR3 color;
		for(unsigned int i = 0; i<surfaceSurfels.size(); i++){
			color = D3DXVECTOR3(RandRange::Rand(0.0f, 1.0f), RandRange::Rand(0.0f, 1.0f), RandRange::Rand(0.0f, 1.0f));

			std::map<float, ProjectStructs::SURFEL*>::iterator itr = surfaceSurfels[i]->neighbors.begin();
			for(; itr != surfaceSurfels[i]->neighbors.end(); itr++){
				VertexWithPosAndColor vpc;
				vpc.color = color;
				vpc.pos = surfaceSurfels[i]->vertex->pos;
				verticeList.push_back(vpc);

				VertexWithPosAndColor vpc2;
				vpc2.color = color;
				vpc2.pos = surfaceSurfels[i]->vertex->pos + (itr->second->vertex->pos - surfaceSurfels[i]->vertex->pos) * 0.5f;
				verticeList.push_back(vpc2);
			}
		}

		neighborCount = verticeList.size();

		VertexWithPosAndColor* vertices = new VertexWithPosAndColor[neighborCount];

		for(int i = 0; i<neighborCount; i++){
			vertices[i] = verticeList[i];
		}

		D3D10_BUFFER_DESC bd;
		bd.Usage = D3D10_USAGE_DEFAULT;
		bd.ByteWidth = neighborCount * sizeof( VertexWithPosAndColor );
		bd.BindFlags = D3D10_BIND_VERTEX_BUFFER | D3D10_BIND_STREAM_OUTPUT;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;

		D3D10_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = vertices;

		HR(Helpers::Globals::Device->CreateBuffer( &bd, &InitData, &neighborVertexBuffer ));	

		neighborDrawSet = true;
	}

	void Surface::DrawNeighbors(){

		if(!neighborDrawSet)
			SetUpNeighborDraw();

		simpleEffect.SetMatrix("View", Helpers::Globals::AppCamera.View());
		simpleEffect.SetMatrix("Projection", Helpers::Globals::AppCamera.Projection());
		simpleEffect.SetMatrix("World", this->world);

		simpleEffect.PreDraw();

		UINT stride = sizeof( VertexWithPosAndColor );
		UINT offset = 0;

		Helpers::Globals::Device->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_LINELIST);
		Helpers::Globals::Device->IASetVertexBuffers( 0, 1, &neighborVertexBuffer, &stride, &offset );
		
		simpleEffect.Draw((int)(neighborCount));		
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

		Helpers::Globals::Device->RSSetState(SolidRenderState);
	}

	void Surface::DrawSurfel(){
	
		surfelEffect.SetFloatVector("CameraPos", Helpers::Globals::AppCamera.Position());
		surfelEffect.SetMatrix("View", Helpers::Globals::AppCamera.View());
		surfelEffect.SetMatrix("World", world);
		surfelEffect.SetTexture("SurfaceTexture", planeTexture);
		surfelEffect.SetFloat("RadiusScale", RadiusScale);
		surfelEffect.SetFloat("A", this->A);
		surfelEffect.SetFloat("B", this->B);
		surfelEffect.SetFloat("rhoOverPi", this->rhoOverPi);

		surfelEffect.PreDraw();

		UINT stride = sizeof( ProjectStructs::SURFEL_VERTEX);
		UINT offset = 0;

		Helpers::Globals::Device->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);
		Helpers::Globals::Device->IASetVertexBuffers( 0, 1, &surfelVertexBuffer, &stride, &offset );

		surfelEffect.Draw(surfaceSurfels.size());
	}

	void Surface::DrawToReadableBuffer(){
		// reset the solid vertex buffers
		geometryEffect.SetFloat("RadiusScale", RadiusScale);
		geometryEffect.SetMatrix("World", world);
		geometryEffect.WriteToGeometryShader(D3D10_PRIMITIVE_TOPOLOGY_POINTLIST, surfelVertexBuffer, surfaceSurfels.size(), solidVertexBuffer);

		Helpers::Globals::Device->CopyResource(readableVertexBuffer, solidVertexBuffer);
	}

	void Surface::Update(float dt){

		bool callResetBuffers = false;
		if(isChanged){
			DrawToReadableBuffer();
		}

		std::vector<ProjectStructs::SURFEL*>::iterator surfelIt, intersectingCellSurfelIt;

		ProjectStructs::SURFEL* surfel;
		for(unsigned int i = 0; i< DeletedStructHolder::GetDeletedSurfelSize(); i++){

			surfel = DeletedStructHolder::GetDeletedSurfel(i);
			surfelIt = find(surfaceSurfels.begin(), surfaceSurfels.end(), surfel);
			if(surfelIt != surfaceSurfels.end()){

				for(unsigned int j = 0; j < surfel->intersectingCells.size(); j++){
					intersectingCellSurfelIt = find(surfel->intersectingCells[j]->surfels.begin(), surfel->intersectingCells[j]->surfels.end(), surfel);
					if(intersectingCellSurfelIt != surfel->intersectingCells[j]->surfels.end()){
						surfel->intersectingCells[j]->surfels.erase(intersectingCellSurfelIt);
					}
				}

				callResetBuffers = true;

				surfaceSurfels.erase(surfelIt);
			}
		}

		if(callResetBuffers){
			ResetBuffers();
		}
	}

	void Surface::CleanUp(){

		if(Surface::TextureLoaded){
			geometryEffect.CleanUp();
			solidEffect.CleanUp();
			surfelEffect.CleanUp();
			wireframeEffect.CleanUp();
			simpleEffect.CleanUp();

			Surface::TextureLoaded = false;
		}

		for(unsigned int i = 0; i<this->surfaceSurfels.size(); i++){
			if(this->surfaceSurfels[i]){
				delete this->surfaceSurfels[i]->vertex;
				this->surfaceSurfels[i]->vertex = NULL;
				delete this->surfaceSurfels[i];
				this->surfaceSurfels[i] = NULL;
			}
		}

		for(unsigned int i = 0; i < this->contactListeners.size(); i++){
			delete this->contactListeners[i];
		}

		this->rigidBodies.clear();
		this->rigidBodies.swap(std::vector<hkpRigidBody*>());

		this->contactListeners.clear();
		this->contactListeners.swap(std::vector<ContactListener*>());

		this->surfaceSurfels.clear();
		this->surfaceSurfels.swap(std::vector<ProjectStructs::SURFEL*>());


		ReleaseCOM(this->surfelVertexBuffer);
		ReleaseCOM(this->solidVertexBuffer);
		ReleaseCOM(this->readableVertexBuffer);
		ReleaseCOM(this->neighborVertexBuffer);
	}
}