#include "PhyxelGrid.h"
#include "MathHelper.h"
#include <algorithm>

PhyxelGrid::PhyxelGrid(void)
{
}

PhyxelGrid::~PhyxelGrid(void)
{
}

PhyxelGrid::PhyxelGrid(int dimensions, D3DXVECTOR3 minCoords, D3DXVECTOR3 maxCoords, D3DXVECTOR3 Pos){

	indices = NULL;
	this->Min = minCoords;
	this->Max = maxCoords;
	this->Center = (this->Max - this->Min) * 0.5f;

	this->SmallestHalfWidth = Center / dimensions;

	this->Position = Pos;

	this->cells = ThreeInOneArray<ProjectStructs::Phyxel_Grid_Cell*>(
		floor((Max - Min).x/(2.0f*SmallestHalfWidth.x))+1, 
		floor((Max - Min).y/(2.0f*SmallestHalfWidth.y))+1, 
		floor((Max - Min).z/(2.0f*SmallestHalfWidth.z))+1);

	for(unsigned int i = 0; i<cells.GetSize(); i++){
		cells[i] = NULL;
	}

	this->halfIndex = D3DXVECTOR3(this->cells.GetWidth() * 0.5f, this->cells.GetHeight() * 0.5f, this->cells.GetDepth() * 0.5f);
		
	D3DXMatrixTranslation(&World, Position.x, Position.y, Position.z);

	D3DXMatrixTranslation(&this->invWorld, -Min.x - Center.x, -Min.y - Center.y, -Min.z- Center.z);
}

void PhyxelGrid::InsertSurface(Surface *surface){

	D3DXVECTOR3 index ;

	for(int j = 0; j < surface->GetSurfaceSurfelCount(); j++){
		PopulateNode(surface->GetSurfaceSurfel(j)->vertex.pos, surface->GetSurfaceSurfel(j), NULL);
		
		D3DXVECTOR3 major, minor;

		major = surface->GetSurfaceSurfel(j)->vertex.majorAxis;
		major.x = abs(major.x);
		major.y = abs(major.y);
		major.z = abs(major.z);

		minor = surface->GetSurfaceSurfel(j)->vertex.minorAxis;
		minor.x = abs(minor.x);
		minor.y = abs(minor.y);
		minor.z = abs(minor.z);

		PopulateNode(surface->GetSurfaceSurfel(j)->vertex.pos + major, surface->GetSurfaceSurfel(j), NULL);
		PopulateNode(surface->GetSurfaceSurfel(j)->vertex.pos - major, surface->GetSurfaceSurfel(j), NULL);
		PopulateNode(surface->GetSurfaceSurfel(j)->vertex.pos + minor, surface->GetSurfaceSurfel(j), NULL);
		PopulateNode(surface->GetSurfaceSurfel(j)->vertex.pos - minor, surface->GetSurfaceSurfel(j), NULL);

		PopulateNode(surface->GetSurfaceSurfel(j)->vertex.pos + major + minor, surface->GetSurfaceSurfel(j), NULL);
		PopulateNode(surface->GetSurfaceSurfel(j)->vertex.pos + major - minor, surface->GetSurfaceSurfel(j), NULL);
		PopulateNode(surface->GetSurfaceSurfel(j)->vertex.pos - major + minor, surface->GetSurfaceSurfel(j), NULL);
		PopulateNode(surface->GetSurfaceSurfel(j)->vertex.pos - major - minor, surface->GetSurfaceSurfel(j), NULL);
	}

	for(int j = 0; j < surface->GetEdgeSurfelCount(); j++){
		PopulateNode(surface->GetEdgeSurfel(j)->vertex.pos, NULL, surface->GetEdgeSurfel(j));

		bool posMaj = MathHelper::PositiveMajor(surface->GetEdgeSurfel(j));
		bool posMin = MathHelper::PositiveMinor(surface->GetEdgeSurfel(j));
		bool negMaj = MathHelper::NegativeMajor(surface->GetEdgeSurfel(j));
		bool negMin = MathHelper::NegativeMinor(surface->GetEdgeSurfel(j));

		D3DXVECTOR3 clipplane = surface->GetEdgeSurfel(j)->vertex.clipPlane;
		D3DXVECTOR3 major, minor;

		major = surface->GetEdgeSurfel(j)->vertex.majorAxis;
		major.x = abs(major.x);
		major.y = abs(major.y);
		major.z = abs(major.z);

		minor = surface->GetEdgeSurfel(j)->vertex.minorAxis;
		minor.x = abs(minor.x);
		minor.y = abs(minor.y);
		minor.z = abs(minor.z);

		if(posMaj)
			PopulateNode(surface->GetEdgeSurfel(j)->vertex.pos + major, NULL, surface->GetEdgeSurfel(j));
		if(negMaj)
			PopulateNode(surface->GetEdgeSurfel(j)->vertex.pos - major, NULL, surface->GetEdgeSurfel(j));
		if(posMin)
			PopulateNode(surface->GetEdgeSurfel(j)->vertex.pos + minor, NULL, surface->GetEdgeSurfel(j));
		if(negMin)
			PopulateNode(surface->GetEdgeSurfel(j)->vertex.pos - minor, NULL, surface->GetEdgeSurfel(j));


		if(posMaj && posMin)
			PopulateNode(surface->GetEdgeSurfel(j)->vertex.pos + major + minor, NULL, surface->GetEdgeSurfel(j));
		if(posMaj && negMin)
			PopulateNode(surface->GetEdgeSurfel(j)->vertex.pos + major - minor, NULL, surface->GetEdgeSurfel(j));
		if(negMaj && posMin)
			PopulateNode(surface->GetEdgeSurfel(j)->vertex.pos - major + minor, NULL, surface->GetEdgeSurfel(j));
		if(negMaj && negMin)
			PopulateNode(surface->GetEdgeSurfel(j)->vertex.pos - major - minor, NULL, surface->GetEdgeSurfel(j));
	}
}

void PhyxelGrid::PopulateNeighbors( D3DXVECTOR3 index )
{
	D3DXVECTOR3 tmp = index;
	
	int count = 0;
	for(int i = 0; i<3; i++){
		for(int j = 0; j<3; j++){
			for(int k = 0; k<3; k++){
				if(i == 1 && j == 1 && k == 1)
					continue;

				tmp.x += (i-1);
				tmp.y += (j-1);
				tmp.z += (k-1);

				PopulateNeighbor(index, tmp, (DIRECTIONS)count++);
			}
		}
	}
}

void PhyxelGrid::PopulateNeighbor( D3DXVECTOR3 index, D3DXVECTOR3 neighbor, DIRECTIONS indexToNeighborDir)
{
	// only populate a neighbor if the neighbor has been filled
	if(cells(index)->neighbours[(int)indexToNeighborDir] == NULL && cells.ValidIndex(neighbor)){
		if(cells(neighbor) != NULL && cells(neighbor)->neighbours[(int)BACK_TOP_RIGHT - (int)indexToNeighborDir]  == NULL){
			cells(index)->neighbours[(int)indexToNeighborDir] = cells(neighbor);
			cells(neighbor)->neighbours[(int)BACK_TOP_RIGHT - (int)indexToNeighborDir] = cells(index);		
		}
	}
}

D3DXVECTOR3 PhyxelGrid::GetPositionOfIndex(int x, int y, int z, bool relative){
	D3DXVECTOR3 pos = D3DXVECTOR3();

	pos.x = (x - this->halfIndex.x) * 2.0f * SmallestHalfWidth.x + 2*SmallestHalfWidth.x;//cells[i]->halfWidth;
	pos.y = (y - this->halfIndex.y) * 2.0f * SmallestHalfWidth.y +  2*SmallestHalfWidth.y;//cells[i]->halfWidth;
	pos.z = (z - this->halfIndex.z) * 2.0f * SmallestHalfWidth.z + 2*SmallestHalfWidth.z;//cells[i]->halfWidth;

	if(!relative)
		pos += Position;

	return pos;
}

void PhyxelGrid::Init(){

	D3D10_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 }, // pos
	};

	effect = Helpers::CustomEffect("Simple.fx", "SimpleTechnique", CUSTOM_EFFECT_TYPE_PIXEL | CUSTOM_EFFECT_TYPE_VERTEX, layout, 1);
	effect.AddVariable("World");
	effect.AddVariable("View");
	effect.AddVariable("Projection");
	effect.AddVariable("Color");

	effect.SetMatrix("Projection", Helpers::Globals::AppCamera.Projection());
	effect.SetFloatVector("Color", D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f));		

	ProjectStructs::SIMPLE_VERTEX vertices[8];

	vertices[0].pos = D3DXVECTOR3(-SmallestHalfWidth.x, -SmallestHalfWidth.y, -SmallestHalfWidth.z);
	vertices[1].pos = D3DXVECTOR3(-SmallestHalfWidth.x, -SmallestHalfWidth.y, SmallestHalfWidth.z);
	vertices[2].pos = D3DXVECTOR3(SmallestHalfWidth.x, -SmallestHalfWidth.y, SmallestHalfWidth.z);
	vertices[3].pos = D3DXVECTOR3(SmallestHalfWidth.x, -SmallestHalfWidth.y, -SmallestHalfWidth.z);

	vertices[4].pos = D3DXVECTOR3(-SmallestHalfWidth.x, SmallestHalfWidth.y, -SmallestHalfWidth.z);
	vertices[5].pos = D3DXVECTOR3(-SmallestHalfWidth.x, SmallestHalfWidth.y, SmallestHalfWidth.z);
	vertices[6].pos = D3DXVECTOR3(SmallestHalfWidth.x, SmallestHalfWidth.y, SmallestHalfWidth.z);
	vertices[7].pos = D3DXVECTOR3(SmallestHalfWidth.x, SmallestHalfWidth.y, -SmallestHalfWidth.z);

	D3D10_BUFFER_DESC bd;
	bd.Usage = D3D10_USAGE_DEFAULT;
	bd.ByteWidth = 8 * sizeof( ProjectStructs::SIMPLE_VERTEX);
	bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	D3D10_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = vertices;

	HR(Helpers::Globals::Device->CreateBuffer( &bd, &InitData, &vertexBuffer));	

	// Create indices
	unsigned int firstIndices[] = { 
		0, 1, 5, 6, 7, 3, 2, 6
	};

	unsigned int secondIndices[] = { 
		2, 1, 5, 4, 7, 3, 0, 4
	};

	D3D10_BUFFER_DESC firstBufferDesc;
	firstBufferDesc.Usage           = D3D10_USAGE_DEFAULT;
	firstBufferDesc.ByteWidth       = 8 * sizeof( unsigned int );
	firstBufferDesc.BindFlags       = D3D10_BIND_INDEX_BUFFER;
	firstBufferDesc.CPUAccessFlags  = 0;
	firstBufferDesc.MiscFlags       = 0;

	D3D10_SUBRESOURCE_DATA firstIndexInitData;
	firstIndexInitData.pSysMem = firstIndices;
	firstIndexInitData.SysMemPitch = 0;
	firstIndexInitData.SysMemSlicePitch = 0;
	HR(Helpers::Globals::Device->CreateBuffer( &firstBufferDesc, &firstIndexInitData, &indexBuffer ));

	D3D10_BUFFER_DESC secondBufferDesc;
	secondBufferDesc.Usage           = D3D10_USAGE_DEFAULT;
	secondBufferDesc.ByteWidth       = 8 * sizeof( unsigned int );
	secondBufferDesc.BindFlags       = D3D10_BIND_INDEX_BUFFER;
	secondBufferDesc.CPUAccessFlags  = 0;
	secondBufferDesc.MiscFlags       = 0;

	D3D10_SUBRESOURCE_DATA secondIndexInitData;
	secondIndexInitData.pSysMem = secondIndices;
	secondIndexInitData.SysMemPitch = 0;
	secondIndexInitData.SysMemSlicePitch = 0;
	HR(Helpers::Globals::Device->CreateBuffer( &secondBufferDesc, &secondIndexInitData, &otherIndexBuffer));
}

void PhyxelGrid::Draw(){

	effect.SetMatrix("View", Helpers::Globals::AppCamera.View());

	effect.PreDraw();

	UINT stride = sizeof( ProjectStructs::SIMPLE_VERTEX);
	UINT offset = 0;

	Helpers::Globals::Device->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP);
	Helpers::Globals::Device->IASetVertexBuffers( 0, 1, &vertexBuffer, &stride, &offset );

	for(unsigned int i = 0; i < cells.GetSize(); i++){
		
		if(cells[i] != NULL){
			
			INDEX indices = cells.GetIndices(i);
			tmpWorld = World;
			D3DXVECTOR3 pos = GetPositionOfIndex(indices.x, indices.y, indices.z, true);

			tmpWorld._41 += pos.x;
			tmpWorld._42 += pos.y;
			tmpWorld._43 += pos.z;

			effect.SetMatrix("World", tmpWorld);

			Helpers::Globals::Device->IASetIndexBuffer( indexBuffer, DXGI_FORMAT_R32_UINT, 0 );
			effect.DrawIndexed(8);

			Helpers::Globals::Device->IASetIndexBuffer( otherIndexBuffer, DXGI_FORMAT_R32_UINT, 0 );
			effect.DrawIndexed(8);
		}		
	}
}

void PhyxelGrid::CleanUp(){
	this->effect.CleanUp();

	this->vertexBuffer->Release();
	this->vertexBuffer = NULL;
	delete this->vertexBuffer;

	this->indexBuffer->Release();
	this->indexBuffer = NULL;
	delete this->indexBuffer;

	this->otherIndexBuffer->Release();
	this->otherIndexBuffer= NULL;
	delete this->otherIndexBuffer;

	for(unsigned int i = 0; i< cells.GetSize(); i++){
		CleanCell(i);

	}
	this->cells.Clear();

	delete [] indices;
}

void PhyxelGrid::PopulateNode( D3DXVECTOR3 surfelPos, ProjectStructs::SURFEL *surfel, ProjectStructs::SURFEL_EDGE *edge)
{
	D3DXVECTOR3 index = GetIndexOfPosition(surfelPos);
	D3DXVECTOR3 centerIndex = GetIndexOfPosition(surfel == NULL ? edge->vertex.pos : surfel->vertex.pos);

	//if(abs(index.x - centerIndex.x) > 1.0f || abs(index.y - centerIndex.y) > 1.0f || abs(index.z - centerIndex.z) > 1.0f){
		// fill the nodes between
		int x = index.x - centerIndex.x == 0.0f ? 0.0f : (index.x - centerIndex.x) / abs(index.x - centerIndex.x);
		int y = index.y - centerIndex.y == 0.0f ? 0.0f : (index.y - centerIndex.y) / abs(index.y - centerIndex.y);
		int z = index.z - centerIndex.z == 0.0f ? 0.0f : (index.z - centerIndex.z) / abs(index.z - centerIndex.z);
		
		D3DXVECTOR3 tmpIndex = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

		for(int i = min(centerIndex.x, index.x); i <= max(centerIndex.x, index.x); i++){
			for(int j = min(centerIndex.y, index.y); j <= max(centerIndex.y, index.y); j++){
				for(int k = min(centerIndex.z, index.z); k <= max(centerIndex.z, index.z); k++){
					tmpIndex.x = i;
					tmpIndex.y = j;
					tmpIndex.z = k;

					PopulateNodeAndCheckNormal(tmpIndex, surfelPos, surfel, edge);
				}
			}
		}
	//}

	//PopulateNodeAndCheckNormal(index, surfelPos, surfel, edge);

}

void PhyxelGrid::InitCell( D3DXVECTOR3 &index )
{
	cells(index) = new Phyxel_Grid_Cell();
	cells(index)->halfWidth = SmallestHalfWidth;
	cells(index)->surfels = std::vector<ProjectStructs::SURFEL*>();
	cells(index)->edges = std::vector<ProjectStructs::SURFEL_EDGE*>();

	ProjectStructs::PHYXEL_NODE *phyxel = new ProjectStructs::PHYXEL_NODE();
	phyxel->pos = GetPositionOfIndex(index.x, index.y, index.z, false);
	phyxel->radius = 10.0f; // TODO change
	phyxel->mass = 4.0f/3.0f * Helpers::Globals::PI * pow(phyxel->radius, 3) * 20;
	phyxel->force = D3DXVECTOR3(0.0f, 0.0f, 0.0);

	cells(index)->phyxel = phyxel;

	PopulateNeighbors(index);
}

D3DXVECTOR3 PhyxelGrid::GetIndexOfPosition( D3DXVECTOR3 surfelPos )
{
	D3DXVec3TransformCoord(&tmp, &surfelPos, &invWorld);
	D3DXVECTOR3 index = tmp + Center;
	index.x = floor(index.x/(2.0f*SmallestHalfWidth.x));
	index.y = floor(index.y/(2.0f*SmallestHalfWidth.y));
	index.z = floor(index.z/(2.0f*SmallestHalfWidth.z));

	return index;
}

void PhyxelGrid::CleanCell( unsigned int index )
{
	if(this->cells[index] != NULL){
		this->cells[index]->surfels.clear();
		this->cells[index]->edges.clear();

		this->cells[index]->surfels.swap(std::vector<ProjectStructs::SURFEL*>());
		this->cells[index]->edges.swap(std::vector<ProjectStructs::SURFEL_EDGE*>());

		if(this->cells[index]->phyxel){
			delete this->cells[index]->phyxel;
		}

		this->cells[index]->neighbours.Clear();

		delete this->cells[index];
	}
}

bool PhyxelGrid::InitCellAndPushSurfels( D3DXVECTOR3 index, ProjectStructs::SURFEL * surfel, ProjectStructs::SURFEL_EDGE * edge )
{
	bool justCreatedCell = false;
	Phyxel_Grid_Cell *cell = cells(index);
	if(cells(index) == NULL){
		InitCell(index);
		justCreatedCell = true;
	}
	if(surfel != NULL){
		if(find(cells(index)->surfels.begin(), cells(index)->surfels.end(), surfel) == cells(index)->surfels.end())
			cells(index)->surfels.push_back(surfel);

		if(find(surfel->intersectingCells.begin(), surfel->intersectingCells.end(), cells(index)) == surfel->intersectingCells.end())
			surfel->intersectingCells.push_back(cells(index));
	}
	else{
		if(find(cells(index)->edges.begin(), cells(index)->edges.end(), edge) == cells(index)->edges.end())
			cells(index)->edges.push_back(edge);

		if(find(edge->intersectingCells.begin(), edge->intersectingCells.end(), cells(index)) == edge->intersectingCells.end())
			edge->intersectingCells.push_back(cells(index));
	}	
	
	return justCreatedCell;
}

void PhyxelGrid::PopulateNodeAndCheckNormal( D3DXVECTOR3 &index, D3DXVECTOR3 surfelPos, ProjectStructs::SURFEL * surfel, ProjectStructs::SURFEL_EDGE * edge )
{
	if(cells.ValidIndex(index)){
		D3DXVECTOR3 nodeCenter = GetPositionOfIndex(index.x, index.y, index.z, false);

		float angle = D3DXVec3Dot(&(nodeCenter - surfelPos), surfel == NULL ? &edge->vertex.normal: &surfel->vertex.normal);

		if(angle < 0.0f){
			InitCellAndPushSurfels(index, surfel, edge);
		}		
		else{
			//printf("hmm...");
			// go to the node behind this one and populate it

			D3DXVECTOR3 normalizedNormal;
			D3DXVec3Normalize(&normalizedNormal, surfel == NULL ? &edge->vertex.normal : &surfel->vertex.normal);

			index -= D3DXVECTOR3(normalizedNormal.x, normalizedNormal.y, normalizedNormal.z );

			if(cells.ValidIndex(index)){						
				InitCellAndPushSurfels(index, surfel, edge);			
			}
			else{
				printf("hmm...");
			}
		}
	}
}