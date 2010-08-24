#include "VertexBufferGrid.h"
#include "d3dApp.h"
#include "TextureCreator.h"
#include "PhysicsWrapper.h"
#include "RandRange.h"
#include "KeyboardHandler.h"
#include "DebugToFile.h"
#include "MathHelper.h"

Helpers::CustomEffect VertexBufferGrid::surfelEffect, VertexBufferGrid::solidEffect, 
	VertexBufferGrid::wireframeEffect, VertexBufferGrid::geometryEffect, VertexBufferGrid::simpleEffect;

ID3D10ShaderResourceView *VertexBufferGrid::SurfelTexture;
ID3D10RasterizerState *VertexBufferGrid::SolidRenderState;

float VertexBufferGrid::RadiusScale = 1.0f, VertexBufferGrid::LastRadiusScale = 1.0f;
bool VertexBufferGrid::isChanged = false;
bool VertexBufferGrid::vertexBufferGridInitialized = false;

int VertexBufferGrid::CellToDraw = 0;

UINT VertexBufferGrid::offset = 0, VertexBufferGrid::solidStride = sizeof(ProjectStructs::SOLID_VERTEX), 
	VertexBufferGrid::wireframeStride = sizeof(ProjectStructs::SOLID_VERTEX), VertexBufferGrid::surfelStride = sizeof(ProjectStructs::SURFEL_VERTEX), 
	VertexBufferGrid::neighborStride = sizeof(VertexWithPosAndColor);
	
VertexBufferGrid::VertexBufferGrid(void)
{
}

VertexBufferGrid::~VertexBufferGrid(void)
{
}

VertexBufferGrid::VertexBufferGrid(D3DXVECTOR3 Min, D3DXVECTOR3 Max, D3DXVECTOR3 Pos, ProjectStructs::MATERIAL_PROPERTIES materialProperties){

	this->materialProperties = materialProperties;

	this->Min = Min;
	this->Max = Max;
	this->Center = (this->Max - this->Min) * 0.5f;

	if(materialProperties.vertexGridSize < 0.0f){
		materialProperties.vertexGridSize = D3DXVec3Length(&(Max - Min));
	}

	this->SmallestHalfWidth = materialProperties.vertexGridSize * D3DXVECTOR3(1.0f, 1.0f, 1.0f);

	this->Position = Pos;

	this->cells = ThreeInOneArray<bool>(
		(int)floor(Center.x / SmallestHalfWidth.x + 1), 
		(int)floor(Center.y / SmallestHalfWidth.y + 1), 
		(int)floor(Center.z / SmallestHalfWidth.z + 1));

	this->halfIndex = D3DXVECTOR3(this->cells.GetWidth() * 0.5f, this->cells.GetHeight() * 0.5f, this->cells.GetDepth() * 0.5f);

	D3DXMatrixTranslation(&World, Position.x, Position.y, Position.z);

	D3DXMatrixTranslation(&this->invWorld, -Min.x - Center.x, -Min.y - Center.y, -Min.z - Center.z);

	phyxelGrid = NULL;

	populatedCellList = NULL;
	populatedCellCount = 0;
}

void VertexBufferGrid::InitCells(){
	std::map<int, Vertex_Grid_Cell*>::iterator cellIterator;

	for(cellIterator = populatedCells.begin(); cellIterator != populatedCells.end(); cellIterator++){
		InitCell(cellIterator->second);
	}
	changedCells.clear();
}

void VertexBufferGrid::InitCell(Vertex_Grid_Cell* cell){

	if(cell->surfels.size() == 0)
		return;

	LastRadiusScale = VertexBufferGrid::RadiusScale;
	VertexBufferGrid::RadiusScale = 1.0f;

	if(!vertexBufferGridInitialized ){
		SurfelTexture = TextureCreator::CreateSurfelTexture();

		InitSolid();
		InitSurfel();
		InitGeometryPass();
		InitWireframe();
		SetUpNeighborEffect();

		vertexBufferGridInitialized = true;
	}

	surfelEffect.AddTexture("SurfaceTexture", materialProperties.texture);
	wireframeEffect.AddTexture("SurfaceTexture", materialProperties.texture);
	solidEffect.AddTexture("SurfaceTexture", materialProperties.texture);

	SetMaterialInfo(materialProperties.sigma, materialProperties.rho);
	SetupSurfelVertexBuffer(cell);

	InitCommonSolidAndWireframe(cell);

	DrawToReadableBuffer(cell);

	ProcessChangedSurfels(cell);

}

void VertexBufferGrid::ResetCell(Vertex_Grid_Cell* cell){
	if(!cell)
		return;
	else if(cell->surfels.size() == 0){
		populatedCells.erase(inversePopulatedCells[cell]);
		inversePopulatedCells.erase(cell);
		populatedCellList = NULL;
		//delete cell;
		cell = NULL;
		return;
	}

	SetupSurfelVertexBuffer(cell);
	
	ReleaseCOM(cell->readableVertexBuffer);
	ReleaseCOM(cell->solidVertexBuffer);

	InitCommonSolidAndWireframe(cell);

	DrawToReadableBuffer(cell);

	ProcessChangedSurfels(cell);
}

void VertexBufferGrid::ProcessChangedSurfels(Vertex_Grid_Cell* cell){
	std::vector<D3DXVECTOR3> surfelVertexList;
	std::vector<ProjectStructs::SURFEL*> surfelList;

	DrawToReadableBuffer(cell);

	// read from the surfel vertex buffer
	ProjectStructs::SOLID_VERTEX* vertices = 0;
	HR(cell->readableVertexBuffer->Map(D3D10_MAP_READ, 0, reinterpret_cast< void** >(&vertices)));
	cell->readableVertexBuffer->Unmap();

	for(unsigned int i = 0; i < cell->surfels.size(); i++){
		if(cell->surfels[i]->isChanged){
			surfelVertexList.push_back(vertices[i*6].pos);
			surfelList.push_back(cell->surfels[i]);
			surfelVertexList.push_back(vertices[i*6+1].pos);
			surfelList.push_back(cell->surfels[i]);
			surfelVertexList.push_back(vertices[i*6+2].pos);
			surfelList.push_back(cell->surfels[i]);
			//surfelVertexList.push_back(vertices[i*6+3].pos);
			//surfelList.push_back(cell->surfels[i]);
			surfelVertexList.push_back(vertices[i*6+4].pos);
			surfelList.push_back(cell->surfels[i]);
			//surfelVertexList.push_back(vertices[i*6+5].pos);
			//surfelList.push_back(cell->surfels[i]);

			if(phyxelGrid) 
				PhyxelGrid::CleanIntersectingCells(cell->surfels[i]);

			if(cell->surfels[i]->hasRigidBody){
				PhysicsWrapper::RemoveRigidBodyWithoutLockingWorld(cell->surfels[i]->rigidBody);
				cell->surfels[i]->hasRigidBody = false;
			}

			if(phyxelGrid) 
				phyxelGrid->InsertPoint(surfelVertexList, cell->surfels[i]);

			PhysicsWrapper::AddSurfels(surfelVertexList, surfelList, materialProperties, Position, false);

			surfelVertexList.clear();
			surfelList.clear();
		}
	}
/*
	if(phyxelGrid) 
		phyxelGrid->InsertPoints(surfelVertexList, surfelList);
*/
	

}

void VertexBufferGrid::InitGeometryPass(){
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

	geometryEffect.SetMatrix("World", World);
}

void VertexBufferGrid::InitSurfel(){
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

void VertexBufferGrid::InitCommonSolidAndWireframe(Vertex_Grid_Cell* cell){	

	D3D10_BUFFER_DESC vbdesc =
	{
		6 * cell->surfels.size() * sizeof(ProjectStructs::SOLID_VERTEX),
		D3D10_USAGE_DEFAULT,
		D3D10_BIND_VERTEX_BUFFER | D3D10_BIND_STREAM_OUTPUT,
		0,
		0
	};

	HR( Helpers::Globals::Device->CreateBuffer( &vbdesc, NULL, &cell->solidVertexBuffer ) );		


	D3D10_BUFFER_DESC vbdesc2 =
	{
		6 * cell->surfels.size() * sizeof(ProjectStructs::SOLID_VERTEX),
		D3D10_USAGE_STAGING,
		0,
		D3D10_CPU_ACCESS_READ,
		0
	};

	HR( Helpers::Globals::Device->CreateBuffer( &vbdesc2, NULL, &cell->readableVertexBuffer) );		
}

void VertexBufferGrid::DrawToReadableBuffer(Vertex_Grid_Cell* cell){
	// reset the solid vertex buffers
	geometryEffect.SetFloat("RadiusScale", RadiusScale);
	geometryEffect.SetMatrix("World", World);
	geometryEffect.WriteToGeometryShader(D3D10_PRIMITIVE_TOPOLOGY_POINTLIST, cell->surfelVertexBuffer, cell->surfels.size(), cell->solidVertexBuffer);

	Helpers::Globals::Device->CopyResource(cell->readableVertexBuffer, cell->solidVertexBuffer);
}

void VertexBufferGrid::InitWireframe(){
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
	wireframeEffect.SetFloatVector("LightColor", Helpers::Globals::AppLight.GetColor());

	// setup the solid renderstate since we want to reset the renderstate when the wireframe surfel has finished
	D3D10_RASTERIZER_DESC solidRasterizer;
	solidRasterizer.FillMode = D3D10_FILL_SOLID;

	Helpers::Globals::Device->CreateRasterizerState(&solidRasterizer, &SolidRenderState);
}

void VertexBufferGrid::InitSolid(){

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
	solidEffect.SetFloatVector("LightColor", Helpers::Globals::AppLight.GetColor());
}

void VertexBufferGrid::SetUpNeighborEffect(){
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


void VertexBufferGrid::Update(){
	if(Helpers::KeyboardHandler::IsSingleKeyDown(DIK_LCONTROL)){
		for(int i = 0; i<populatedCellCount; i++){
			populatedCellList[i]->neighborCount = 0;
			ReleaseCOM(populatedCellList[i]->neighborVertexBuffer);
		}
	}
	if(Helpers::KeyboardHandler::IsSingleKeyDown(DIK_T) && materialProperties.deformable){
		if(!Helpers::Globals::DRAW_ONLY_ONE_CELL_AT_A_TIME){
			for(int i = 0; i<populatedCellCount; i++){
				populatedCellList[i]->changed = true;
				changedCells.push_back(populatedCellList[i]);
			}
		}
		else{
			populatedCellList[CellToDraw]->changed = true;
			changedCells.push_back(populatedCellList[CellToDraw]);
		}

	}

	if(changedCells.size() != 0){
		PhysicsWrapper::LockWorld();
		for(int i = 0; i<changedCells.size(); i++){
			if(changedCells[i]->changed){
				ResetCell(changedCells[i]);
				changedCells[i]->changed = false;
			}
		}
		PhysicsWrapper::UnLockWorld();

		changedCells.clear();
	}

	if(isChanged){
		VertexBufferGrid::RadiusScale = VertexBufferGrid::LastRadiusScale;
		if(populatedCellList == NULL){
			SetupPopulatedCellList();

			if(materialProperties.deformable){
				for(int i = 0; i<populatedCellCount; i++){
					SetUpNeighborDraw(populatedCellList[i]);
				}
			}
		}

		for(int i = 0; i < populatedCellCount; i++){
			DrawToReadableBuffer(populatedCellList[i]);
		}
	}
}

void VertexBufferGrid::InsertPoints(std::vector<ProjectStructs::SURFEL*> surfels){
	for(unsigned int i = 0; i<surfels.size(); i++){
		PopulateNode(surfels[i]);
	}
}

D3DXVECTOR3 VertexBufferGrid::GetPositionOfIndex(int x, int y, int z, bool relative){
	D3DXVECTOR3 pos = D3DXVECTOR3();

	pos.x = (x - this->halfIndex.x) * 2.0f * SmallestHalfWidth.x + SmallestHalfWidth.x;//cells[i]->halfWidth;
	pos.y = (y - this->halfIndex.y) * 2.0f * SmallestHalfWidth.y +  SmallestHalfWidth.y;//cells[i]->halfWidth;
	pos.z = (z - this->halfIndex.z) * 2.0f * SmallestHalfWidth.z + SmallestHalfWidth.z;//cells[i]->halfWidth;

	if(!relative)
		pos += Position;

	return pos;
}

void VertexBufferGrid::CleanUp(){
	std::map<int, ProjectStructs::Vertex_Grid_Cell*>::const_iterator populatedCellsIterator;

	for(int i = 0; i < cells.GetSize(); i++){
		if(cells[i] && populatedCells[i] != NULL)
			CleanUpCell(populatedCells[i]);
	}

	this->populatedCells.clear();
	this->inversePopulatedCells.clear();
}

bool VertexBufferGrid::PopulateNode(ProjectStructs::SURFEL *surfel)
{
	D3DXVECTOR3 index = GetIndexOfPosition(surfel->vertex->pos);
	int i;

	// insert a new thing in the map
	i = cells.GetIndex((unsigned int)index.x, (unsigned int)index.y, (unsigned int)index.z);

	if(i == -1)
	{
		return false;
	}

	cells[i] = true;
	if(populatedCells.find(i) == populatedCells.end()){
		populatedCells[i] = new ProjectStructs::Vertex_Grid_Cell;
		populatedCells[i]->neighborVertexBuffer = NULL;
		populatedCells[i]->readableVertexBuffer = NULL;
		populatedCells[i]->solidVertexBuffer = NULL;
		populatedCells[i]->surfelVertexBuffer = NULL;

		inversePopulatedCells[populatedCells[i]] = i;

		changedCells.push_back(populatedCells[i]);
		
		populatedCellCount = 0;
		populatedCellList = NULL;
	}

	bool listContainsSurfel = false;
	for(int j = 0; j<populatedCells[i]->surfels.size() && !listContainsSurfel; j++){
		listContainsSurfel = MathHelper::Intersection(populatedCells[i]->surfels[j], surfel);
		/*D3DXVec3Length(&(populatedCells[i]->surfels[j]->vertex->pos - surfel->vertex->pos)) < 0.001f) && (surfel->vertex->clipPlane == populatedCells[i]->surfels[j]->vertex->clipPlane) && 
			(surfel->vertex->normal == populatedCells[i]->surfels[j]->vertex->normal);*/
	}

	if(!listContainsSurfel){
		populatedCells[i]->surfels.push_back(surfel);	
		surfel->vertexGridCell = populatedCells[i];
		surfel->vertexGridCell->changed = true;
		
		changedCells.push_back(surfel->vertexGridCell);

	//	DebugToFile::Debug("inserted surfel 0x%x at (%.3f, %.3f, %.3f) into vertexbuffergrid 0x%x (%f, %f, %f) [%d]", surfel, surfel->vertex->pos.x, surfel->vertex->pos.y, surfel->vertex->pos.z, surfel->vertexGridCell, index.x, index.y, index.z, i);

		return true;
	}
	else{
		return false;
	}
}

D3DXVECTOR3 VertexBufferGrid::GetIndexOfPosition( D3DXVECTOR3 surfelPos )
{
	D3DXVec3TransformCoord(&tmp, &surfelPos, &invWorld);
	D3DXVECTOR3 index = tmp + Center;
	index.x = floor(index.x/(2.0f*SmallestHalfWidth.x));
	index.y = floor(index.y/(2.0f*SmallestHalfWidth.y));
	index.z = floor(index.z/(2.0f*SmallestHalfWidth.z));

	return index;
}

void VertexBufferGrid::SetupPopulatedCellList(){
	if(populatedCellList != NULL)
		delete [] populatedCellList;

	populatedCellList = new ProjectStructs::Vertex_Grid_Cell*[populatedCells.size()];
	this->populatedCellCount = 0; 

	for(std::map<int, Vertex_Grid_Cell*>::iterator cellIterator = populatedCells.begin(); 
		cellIterator != populatedCells.end(); cellIterator++){
		populatedCellList[populatedCellCount++] = cellIterator->second;
	}
}

void VertexBufferGrid::SetUpNeighborDraw(Vertex_Grid_Cell* cell){

	std::vector<VertexWithPosAndColor> verticeList = std::vector<VertexWithPosAndColor>();

	D3DXVECTOR3 color;
	for(unsigned int i = 0; i<cell->surfels.size(); i++){
		color = D3DXVECTOR3(RandRange::Rand(0.0f, 1.0f), RandRange::Rand(0.0f, 1.0f), RandRange::Rand(0.0f, 1.0f));

		std::map<float, ProjectStructs::SURFEL*>::iterator itr = cell->surfels[i]->neighbors.begin();
		for(; itr != cell->surfels[i]->neighbors.end(); itr++){
			VertexWithPosAndColor vpc;
			vpc.color = color;
			vpc.pos = cell->surfels[i]->vertex->pos;
			verticeList.push_back(vpc);

			VertexWithPosAndColor vpc2;
			vpc2.color = color;
			vpc2.pos = cell->surfels[i]->vertex->pos + (itr->second->vertex->pos - cell->surfels[i]->vertex->pos) * 0.5f;
			verticeList.push_back(vpc2);
		}
	}

	cell->neighborCount = verticeList.size();

	if(cell->neighborCount == 0)
		return;

	VertexWithPosAndColor* vertices = new VertexWithPosAndColor[cell->neighborCount];

	for(int i = 0; i<cell->neighborCount; i++){
		vertices[i] = verticeList[i];
	}

	D3D10_BUFFER_DESC bd;
	bd.Usage = D3D10_USAGE_DEFAULT;
	bd.ByteWidth = cell->neighborCount * sizeof( VertexWithPosAndColor );
	bd.BindFlags = D3D10_BIND_VERTEX_BUFFER | D3D10_BIND_STREAM_OUTPUT;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	D3D10_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = vertices;

	HR(Helpers::Globals::Device->CreateBuffer( &bd, &InitData, &cell->neighborVertexBuffer ));	
}

void VertexBufferGrid::Draw(D3DXMATRIX world){
	if(populatedCellList == NULL){
		SetupPopulatedCellList();
	}

	this->World = world;

	if(Helpers::Globals::SurfelRenderMethod == Helpers::SOLID){
		solidEffect.SetFloatVector("CameraPos", Helpers::Globals::AppCamera.Position());
		solidEffect.SetMatrix("View", Helpers::Globals::AppCamera.View());
		solidEffect.SetMatrix("Projection", Helpers::Globals::AppCamera.Projection());
		solidEffect.SetMatrix("World", World);
		solidEffect.SetTexture("SurfaceTexture", materialProperties.texture);
		solidEffect.SetFloat("A", this->A);
		solidEffect.SetFloat("B", this->B);
		solidEffect.SetFloat("rhoOverPi", this->rhoOverPi);
	}
	else if(Helpers::Globals::SurfelRenderMethod == Helpers::WIREFRAME){
		wireframeEffect.SetFloatVector("CameraPos", Helpers::Globals::AppCamera.Position());
		wireframeEffect.SetMatrix("View", Helpers::Globals::AppCamera.View());
		wireframeEffect.SetMatrix("Projection", Helpers::Globals::AppCamera.Projection());
		wireframeEffect.SetMatrix("World", World);
		wireframeEffect.SetTexture("SurfaceTexture", materialProperties.texture);
		wireframeEffect.SetFloat("A", this->A);
		wireframeEffect.SetFloat("B", this->B);
		wireframeEffect.SetFloat("rhoOverPi", this->rhoOverPi);
	}
	else{
		surfelEffect.SetFloatVector("CameraPos", Helpers::Globals::AppCamera.Position());
		surfelEffect.SetMatrix("View", Helpers::Globals::AppCamera.View());
		surfelEffect.SetMatrix("World", World);
		surfelEffect.SetTexture("SurfaceTexture", materialProperties.texture);
		surfelEffect.SetFloat("RadiusScale", RadiusScale);
		surfelEffect.SetFloat("A", this->A);
		surfelEffect.SetFloat("B", this->B);
		surfelEffect.SetFloat("rhoOverPi", this->rhoOverPi);
	}

	if(Helpers::Globals::DRAW_NEIGHBORS && materialProperties.deformable){
		simpleEffect.SetMatrix("View", Helpers::Globals::AppCamera.View());
		simpleEffect.SetMatrix("Projection", Helpers::Globals::AppCamera.Projection());
		simpleEffect.SetMatrix("World", this->World);
	}

	if(materialProperties.deformable && Helpers::Globals::DRAW_ONLY_ONE_CELL_AT_A_TIME){

		if(Helpers::KeyboardHandler::IsSingleKeyDown(DIK_P) && CellToDraw < populatedCellCount){
			CellToDraw++;
			
			Helpers::Globals::DebugInformation.AddText(DEBUG_TYPE, "Showing cell 0x%x", populatedCellList[CellToDraw]);
			for(int i = 0; i<populatedCellList[CellToDraw]->surfels.size(); i++){
				DebugToFile::Debug("Contains surfels: 0x%x", populatedCellList[CellToDraw]->surfels[i]);
			}
		}
		else if(Helpers::KeyboardHandler::IsSingleKeyDown(DIK_O) && CellToDraw > 0)
		{
			CellToDraw--;

			Helpers::Globals::DebugInformation.AddText(DEBUG_TYPE, "Showing cell 0x%x", populatedCellList[CellToDraw]);
			for(int i = 0; i<populatedCellList[CellToDraw]->surfels.size(); i++){
				DebugToFile::Debug("Contains surfels: 0x%x", populatedCellList[CellToDraw]->surfels[i]);
			}

		}

		DrawCell(populatedCellList[CellToDraw]);
		for(int i = 0; i<populatedCellList[CellToDraw]->surfels.size(); i++){
			for(int j = 0; j < populatedCellList[CellToDraw]->surfels[i]->intersectingCells.size(); j++){
				phyxelGrid->DrawSingleCell(populatedCellList[CellToDraw]->surfels[i]->intersectingCells[j]);
			}
		}
	}
	else{
	//for(std::map<int, Vertex_Grid_Cell*>::iterator cellIterator = populatedCells.begin(); 
	//	cellIterator != populatedCells.end(); cellIterator++){
		for(int i = 0; i<populatedCellCount; i+=10){
			DrawCell(populatedCellList[i]);
			
			if(i+9 < populatedCellCount)
			{
				DrawCell(populatedCellList[i+1]);
				DrawCell(populatedCellList[i+2]);
				DrawCell(populatedCellList[i+3]);
				DrawCell(populatedCellList[i+4]);
				DrawCell(populatedCellList[i+5]);
				DrawCell(populatedCellList[i+6]);
				DrawCell(populatedCellList[i+7]);
				DrawCell(populatedCellList[i+8]);
				DrawCell(populatedCellList[i+9]);
			}
			else if(i+8 < populatedCellCount)
			{
				DrawCell(populatedCellList[i+1]);
				DrawCell(populatedCellList[i+2]);
				DrawCell(populatedCellList[i+3]);
				DrawCell(populatedCellList[i+4]);
				DrawCell(populatedCellList[i+5]);
				DrawCell(populatedCellList[i+6]);
				DrawCell(populatedCellList[i+7]);
				DrawCell(populatedCellList[i+8]);
			}
			else if(i+7 < populatedCellCount)
			{
				DrawCell(populatedCellList[i+1]);
				DrawCell(populatedCellList[i+2]);
				DrawCell(populatedCellList[i+3]);
				DrawCell(populatedCellList[i+4]);
				DrawCell(populatedCellList[i+5]);
				DrawCell(populatedCellList[i+6]);
				DrawCell(populatedCellList[i+7]);
			}
			else if(i+6 < populatedCellCount)
			{
				DrawCell(populatedCellList[i+1]);
				DrawCell(populatedCellList[i+2]);
				DrawCell(populatedCellList[i+3]);
				DrawCell(populatedCellList[i+4]);
				DrawCell(populatedCellList[i+5]);
				DrawCell(populatedCellList[i+6]);
			}
			else if(i+5 < populatedCellCount)
			{
				DrawCell(populatedCellList[i+1]);
				DrawCell(populatedCellList[i+2]);
				DrawCell(populatedCellList[i+3]);
				DrawCell(populatedCellList[i+4]);
				DrawCell(populatedCellList[i+5]);
			}
			else if(i+4 < populatedCellCount)
			{
				DrawCell(populatedCellList[i+1]);
				DrawCell(populatedCellList[i+2]);
				DrawCell(populatedCellList[i+3]);
				DrawCell(populatedCellList[i+4]);
			}
			else if(i+3 < populatedCellCount)
			{
				DrawCell(populatedCellList[i+1]);
				DrawCell(populatedCellList[i+2]);
				DrawCell(populatedCellList[i+3]);
			}
			else if(i+2 < populatedCellCount)
			{
				DrawCell(populatedCellList[i+1]);
				DrawCell(populatedCellList[i+2]);
			}
			else if(i+1 < populatedCellCount)
			{
				DrawCell(populatedCellList[i+1]);
			}
		}
	}
}

void VertexBufferGrid::DrawCell(ProjectStructs::Vertex_Grid_Cell* cell){

	if(Helpers::Globals::DRAW_NEIGHBORS && materialProperties.deformable){
		DrawNeighbors(cell);
	}

	if(Helpers::Globals::SurfelRenderMethod == Helpers::WIREFRAME)
		DrawWireframe(cell);
	else if(Helpers::Globals::SurfelRenderMethod == Helpers::SOLID)
		DrawSolid(cell);
	else if(Helpers::Globals::SurfelRenderMethod == Helpers::SURFEL)
		DrawSurfel(cell);
}

void VertexBufferGrid::DrawWireframe(Vertex_Grid_Cell* cell){
	Helpers::Globals::Device->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	Helpers::Globals::Device->IASetVertexBuffers( 0, 1, &cell->solidVertexBuffer, &VertexBufferGrid::wireframeStride, &VertexBufferGrid::offset );

	wireframeEffect.PreDraw();
	wireframeEffect.DrawAuto();

	Helpers::Globals::Device->RSSetState(SolidRenderState);
}

void VertexBufferGrid::DrawSolid(Vertex_Grid_Cell* cell){

	Helpers::Globals::Device->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Helpers::Globals::Device->IASetVertexBuffers( 0, 1, &cell->solidVertexBuffer, &VertexBufferGrid::solidStride, &VertexBufferGrid::offset );
		
	solidEffect.PreDraw();
	solidEffect.DrawAuto();
}

void VertexBufferGrid::DrawSurfel(Vertex_Grid_Cell* cell){

	Helpers::Globals::Device->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);
	Helpers::Globals::Device->IASetVertexBuffers( 0, 1, &cell->surfelVertexBuffer, &VertexBufferGrid::surfelStride, &VertexBufferGrid::offset );

	surfelEffect.PreDraw();
	surfelEffect.Draw(cell->surfels.size());
}

void VertexBufferGrid::DrawNeighbors(Vertex_Grid_Cell* cell){

	if(!cell->neighborVertexBuffer || cell->neighborCount == 0){
		SetUpNeighborDraw(cell);
	}

	Helpers::Globals::Device->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_LINELIST);

	Helpers::Globals::Device->IASetVertexBuffers( 0, 1, &cell->neighborVertexBuffer, &VertexBufferGrid::neighborStride, &VertexBufferGrid::offset );

	simpleEffect.PreDraw();
	simpleEffect.Draw((int)(cell->neighborCount));		
}

void VertexBufferGrid::ResetSurfel( ProjectStructs::SURFEL *surfel )
{
	surfel->vertexGridCell->surfels.erase(find(surfel->vertexGridCell->surfels.begin(), surfel->vertexGridCell->surfels.end(), surfel));
	if(surfel->vertexGridCell->surfels.size() == 0){
		CleanUpCell(surfel->vertexGridCell);
	}
	if(PopulateNode(surfel))
		changedCells.push_back(surfel->vertexGridCell);
}

void VertexBufferGrid::CleanUpCell( ProjectStructs::Vertex_Grid_Cell* cell)
{
	ReleaseCOM(cell->neighborVertexBuffer);
	ReleaseCOM(cell->readableVertexBuffer);
	ReleaseCOM(cell->solidVertexBuffer);
	ReleaseCOM(cell->surfelVertexBuffer);

	for(int i = 0; i<cell->surfels.size(); i++){
		delete cell->surfels[i]->contactListener;
		delete cell->surfels[i]->vertex;
		delete cell->surfels[i];
	}

	populatedCellList = NULL;

	populatedCells.erase(inversePopulatedCells[cell]);
	inversePopulatedCells.erase(cell);

	delete cell;
	cell = NULL;
}

void VertexBufferGrid::SetupSurfelVertexBuffer( Vertex_Grid_Cell* cell )
{
	D3D10_BUFFER_DESC bd;
	bd.Usage = D3D10_USAGE_DEFAULT;
	bd.ByteWidth = cell->surfels.size() * sizeof( ProjectStructs::SURFEL_VERTEX );
	bd.BindFlags = D3D10_BIND_VERTEX_BUFFER | D3D10_BIND_STREAM_OUTPUT;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	ProjectStructs::SURFEL_VERTEX* sVertices = NULL;
	sVertices = new ProjectStructs::SURFEL_VERTEX[cell->surfels.size()];

	for(unsigned int i = 0; i<cell->surfels.size(); i++){
		sVertices[i] = *cell->surfels[i]->vertex;
	}

	D3D10_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = sVertices;
	HR(Helpers::Globals::Device->CreateBuffer( &bd, &InitData, &cell->surfelVertexBuffer ));
}