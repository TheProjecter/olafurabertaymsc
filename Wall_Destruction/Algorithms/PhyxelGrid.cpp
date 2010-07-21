#include "PhyxelGrid.h"
#include "MathHelper.h"
#include "FractureManager.h"
#include <algorithm>

PhyxelGrid::PhyxelGrid(void)
{
}

PhyxelGrid::~PhyxelGrid(void)
{
}

PhyxelGrid::PhyxelGrid(int dimensions, D3DXVECTOR3 minCoords, D3DXVECTOR3 maxCoords, D3DXVECTOR3 Pos, ProjectStructs::MATERIAL_PROPERTIES materialProperties){

	this->materialProperties = materialProperties;

	this->dimensions = dimensions;
	this->Min = minCoords;
	this->Max = maxCoords;
	this->Center = (this->Max - this->Min) * 0.5f;

	this->SmallestHalfWidth = Center / (float)dimensions;

	this->Position = Pos;

	this->cells = ThreeInOneArray<ProjectStructs::Phyxel_Grid_Cell*>(
		(int)floor((Max - Min).x/(2.0f*SmallestHalfWidth.x) + 1), 
		(int)floor((Max - Min).y/(2.0f*SmallestHalfWidth.y) + 1 ), 
		(int)floor((Max - Min).z/(2.0f*SmallestHalfWidth.z) + 1));

	for(unsigned int i = 0; i<cells.GetSize(); i++){
		cells[i] = NULL;
	}

	this->halfIndex = D3DXVECTOR3(this->cells.GetWidth() * 0.5f, this->cells.GetHeight() * 0.5f, this->cells.GetDepth() * 0.5f);
		
	D3DXMatrixTranslation(&World, Position.x, Position.y, Position.z);

	D3DXMatrixTranslation(&this->invWorld, -Min.x - Center.x, -Min.y - Center.y, -Min.z- Center.z);

	allocatedCells = 0;
	deallocatedCells = 0;
	allocatedPhyxels = 0;
	deallocatedPhyxels = 0;
}

void PhyxelGrid::InsertPoints(std::vector<D3DXVECTOR3> surfacePoints, std::vector<ProjectStructs::SURFEL*> surfels){
	for(unsigned int i = 0; i<surfacePoints.size(); i++){
		PopulateNode( surfacePoints[i], surfels[i], NULL);
	}
}

void PhyxelGrid::InsertEdges(std::vector<D3DXVECTOR3> edgePoints, std::vector<ProjectStructs::SURFEL_EDGE*> edges){
	for(unsigned int i = 0; i<edgePoints.size(); i++){
		PopulateNode( edgePoints[i], NULL, edges[i]);
	}
}

void PhyxelGrid::PopulateNeighbors( D3DXVECTOR3 index )
{
	D3DXVECTOR3 tmp = index;
	
	int count = 0;
	for(int i = 0; i<3; i++){
		for(int j = 0; j<3; j++){
			for(int k = 0; k<3; k++){
				tmp = index;
				
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
	if(!cells.ValidIndex(neighbor) || cells(neighbor) == NULL || cells(index) == NULL){
		return;
	}

	if(cells(index)->neighbours[(int)indexToNeighborDir] == NULL && cells.ValidIndex(neighbor)){
		if(cells(neighbor)->neighbours[(int)BACK_TOP_RIGHT - (int)indexToNeighborDir]  == NULL){
			cells(index)->neighbours[(int)indexToNeighborDir] = cells(neighbor);
			cells(index)->phyxel->neighbours[(int)indexToNeighborDir] = cells(neighbor)->phyxel;
			cells(index)->phyxel->neighbourWeight[(int)indexToNeighborDir] = FractureManager::CalculateWeight(cells(index)->phyxel, cells(neighbor)->phyxel);

			cells(neighbor)->neighbours[(int)BACK_TOP_RIGHT - (int)indexToNeighborDir] = cells(index);		
			cells(neighbor)->phyxel->neighbours[(int)BACK_TOP_RIGHT - (int)indexToNeighborDir] = cells(index)->phyxel;		
			cells(neighbor)->phyxel->neighbourWeight[(int)BACK_TOP_RIGHT - (int)indexToNeighborDir] = FractureManager::CalculateWeight(cells(neighbor)->phyxel, cells(index)->phyxel);
		}
	}
}

D3DXVECTOR3 PhyxelGrid::GetPositionOfIndex(int x, int y, int z, bool relative){
	D3DXVECTOR3 pos = D3DXVECTOR3();

	pos.x = (x - this->halfIndex.x) * 2.0f * SmallestHalfWidth.x + SmallestHalfWidth.x;//cells[i]->halfWidth;
	pos.y = (y - this->halfIndex.y) * 2.0f * SmallestHalfWidth.y +  SmallestHalfWidth.y;//cells[i]->halfWidth;
	pos.z = (z - this->halfIndex.z) * 2.0f * SmallestHalfWidth.z + SmallestHalfWidth.z;//cells[i]->halfWidth;

	if(!relative)
		pos += Position;

	return pos;
}

void PhyxelGrid::Init( ChangedPhyxels *changedPhyxels )
{
	std::vector<D3DXVECTOR3> indices;
	std::vector<D3DXVECTOR3> populatedIndices;

	// put in phyxels that are inside of the volume
	for(unsigned int i = 0; i < this->cells.GetWidth(); i++){
		for(unsigned int j = 0; j < this->cells.GetHeight(); j++){
			for(unsigned int k = 0; k < this->cells.GetDepth(); k++){				
				// if the cell is populated, we need to calculate the mass of the phyxel
				if(cells(i, j, k) && (cells(i, j, k)->surfels.size() != 0 || cells(i, j, k)->edges.size() != 0)) {
					populatedIndices.push_back(D3DXVECTOR3((float)i, (float)j, (float)k));
					continue;
				}

				// go up, down, forward, back, left and right to check how many surfaces we go through			
				int posXCount = 0;
				int negXCount = 0;
				int posYCount = 0;
				int negYCount = 0;
				int posZCount = 0;
				int negZCount = 0;
				// positive X
				for(int posX = (int)i; posX < (int)this->cells.GetWidth(); posX++){
					if(cells(posX, j, k) && (cells(posX, j, k)->surfels.size() != 0 || cells(posX, j, k)->edges.size() != 0) )
						posXCount++;
				}

				for(int negX = (int)i; negX >= 0; negX--){
					if(cells(negX, j, k)  && (cells(negX, j, k)->surfels.size() != 0 || cells(negX, j, k)->edges.size() != 0) )
						negXCount++;
				}

				// positive Y
				for(int posY = (int)j; posY < (int)this->cells.GetHeight(); posY++){
					if(cells(i, posY, k)  && (cells(i, posY, k)->surfels.size() != 0 || cells(i, posY, k)->edges.size() != 0) )
						posYCount++;
				}

				for(int negY = (int)j; negY >= 0; negY--){
					if(cells(i, negY, k) && (cells(i, negY, k)->surfels.size() != 0 || cells(i, negY, k)->edges.size() != 0) )
						negYCount++;
				}

				// positive Z
				for(int posZ = (int)k; posZ < (int)this->cells.GetDepth(); posZ++){
					if(cells(i, j, posZ) && (cells(i, j, posZ)->surfels.size() != 0 || cells(i, j, posZ)->edges.size() != 0) )
						posZCount++;
				}

				for(int negZ = (int)k; negZ >= 0; negZ--){
					if(cells(i, j, negZ) && (cells(i, j, negZ)->surfels.size() != 0 || cells(i, j, negZ)->edges.size() != 0) )
						negZCount++;
				}

				if( posXCount == 1 && posYCount == 1 && posZCount == 1 && negXCount == 1 && negYCount == 1 && negZCount		== 1){
					// ok, the node is in the volume...
					indices.push_back(D3DXVECTOR3((float)i, (float)j, (float)k));
				}				
			}
		}

		for(unsigned int i = 0; i < indices.size(); i++){
			InitCell(indices[i]);
		}

		for(unsigned int i = 0; i< populatedIndices.size(); i++){
			PopulateNeighbors(populatedIndices[i]);
			changedPhyxels->AddDrawablePhyxel(cells(populatedIndices[i])->phyxel);
		}

		for(unsigned int i = 0; i < indices.size(); i++){
			PopulateNeighbors(indices[i]);
			changedPhyxels->AddDrawablePhyxel(cells(indices[i])->phyxel);
		}

		for(unsigned int i = 0; i< populatedIndices.size(); i++){
			cells(populatedIndices[i])->phyxel->mass = 4.0f / 3.0f * Helpers::Globals::PI * pow(cells(populatedIndices[i])->phyxel->supportRadius, 3) * cells(populatedIndices[i])->phyxel->density;
			cells(populatedIndices[i])->phyxel->volume = cells(populatedIndices[i])->phyxel->mass / cells(populatedIndices[i])->phyxel->density;

			D3DXMATRIX momentMatrix;
			float x, y, z, weight;

			for(unsigned int j = 0; j<cells(populatedIndices[i])->neighbours.GetSize(); j++){
				if(!cells(populatedIndices[i])->neighbours[j])
					continue;

				x = cells(populatedIndices[i])->phyxel->pos.x;
				y = cells(populatedIndices[i])->phyxel->pos.y;
				z = cells(populatedIndices[i])->phyxel->pos.z;
				weight = cells(populatedIndices[i])->phyxel->neighbourWeight[j];
				
				momentMatrix._11 = weight * x*x;    momentMatrix._12 = weight * y*x;	momentMatrix._13 = weight * z*x;	momentMatrix._14 = 0.0f;
				momentMatrix._21 = weight * y*x;    momentMatrix._22 = weight * y*y;	momentMatrix._23 = weight * z*y;	momentMatrix._24 = 0.0f;
				momentMatrix._31 = weight * z*x;    momentMatrix._32 = weight * y*z;	momentMatrix._33 = weight * z*z;	momentMatrix._34 = 0.0f;
				momentMatrix._41 = 0.0f;			momentMatrix._42 = 0.0f;			momentMatrix._43 = 0.0f;			momentMatrix._44 = 0.0f;

				cells(populatedIndices[i])->phyxel->momentMatrix += momentMatrix;
			}

			D3DXMatrixInverse(&cells(populatedIndices[i])->phyxel->momentMatrix, NULL, &cells(populatedIndices[i])->phyxel->momentMatrix);
		}

		for(unsigned int i = 0; i < indices.size(); i++){
			cells(indices[i])->phyxel->mass = 4.0f / 3.0f * Helpers::Globals::PI * pow(cells(indices[i])->phyxel->supportRadius, 3) * cells(indices[i])->phyxel->density;
			cells(indices[i])->phyxel->volume = cells(indices[i])->phyxel->mass / cells(indices[i])->phyxel->density;
			D3DXMATRIX momentMatrix;
			float x, y, z, weight;

			for(unsigned int j = 0; j<cells(indices[i])->neighbours.GetSize(); j++){
				if(!cells(indices[i])->neighbours[j])
					continue;

				x = cells(indices[i])->phyxel->pos.x;
				y = cells(indices[i])->phyxel->pos.y;
				z = cells(indices[i])->phyxel->pos.z;
				weight = cells(indices[i])->phyxel->neighbourWeight[j];

				momentMatrix._11 = weight * x*x;    momentMatrix._12 = weight * y*x;	momentMatrix._13 = weight * z*x;	momentMatrix._14 = 0.0f;
				momentMatrix._21 = weight * y*x;    momentMatrix._22 = weight * y*y;	momentMatrix._23 = weight * z*y;	momentMatrix._24 = 0.0f;
				momentMatrix._31 = weight * z*x;    momentMatrix._32 = weight * y*z;	momentMatrix._33 = weight * z*z;	momentMatrix._34 = 0.0f;
				momentMatrix._41 = 0.0f;			momentMatrix._42 = 0.0f;			momentMatrix._43 = 0.0f;			momentMatrix._44 = 0.0f;

				cells(indices[i])->phyxel->momentMatrix += momentMatrix;
			}
			D3DXMatrixInverse(&cells(indices[i])->phyxel->momentMatrix, NULL, &cells(indices[i])->phyxel->momentMatrix);
		}
 	}

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

	ReleaseCOM(this->vertexBuffer);
	ReleaseCOM(this->indexBuffer);
	ReleaseCOM(this->otherIndexBuffer);

	unsigned int maximum = cells.GetSize();
	for(unsigned int i = 0; i< maximum; i++){
		CleanCell(i);
	}

	this->cells.Clear();
}

void PhyxelGrid::CleanCell( unsigned int index )
{
	if(this->cells[index] != NULL){
		this->cells[index]->surfels.clear();
		this->cells[index]->edges.clear();
		//this->cells[index]->cracks.clear();

		this->cells[index]->surfels.swap(std::vector<ProjectStructs::SURFEL*>());
		this->cells[index]->edges.swap(std::vector<ProjectStructs::SURFEL_EDGE*>());
		//this->cells[index]->cracks.swap(std::vector<ProjectStructs::CRACK_NODE*>());

		this->cells[index]->phyxel->neighbours.Clear();
		this->cells[index]->phyxel->neighbourWeight.Clear();
		delete this->cells[index]->phyxel;

		this->cells[index]->neighbours.Clear();

		deallocatedPhyxels++;
		deallocatedCells++;
	}

	delete this->cells[index];
}


void PhyxelGrid::PopulateNode( D3DXVECTOR3 surfelPos, ProjectStructs::SURFEL *surfel, ProjectStructs::SURFEL_EDGE *edge)
{
	D3DXVECTOR3 index = GetIndexOfPosition(surfelPos);
	D3DXVECTOR3 centerIndex = GetIndexOfPosition(surfel == NULL ? edge->vertex.pos : surfel->vertex.pos);

	// fill the nodes between
	D3DXVECTOR3 tmpIndex = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	for(float i = max(min(centerIndex.x, index.x), 0.0f); i <= max(centerIndex.x, index.x); i++){
		for(float j = max(min(centerIndex.y, index.y), 0.0f); j <= max(centerIndex.y, index.y); j++){
			for(float k = max(min(centerIndex.z, index.z), 0.0f); k <= max(centerIndex.z, index.z); k++){
				tmpIndex.x = i;
				tmpIndex.y = j;
				tmpIndex.z = k;

				PopulateNodeAndCheckNormal(tmpIndex, surfelPos, surfel, edge);
			}
		}
	}
}

void PhyxelGrid::InitCell( D3DXVECTOR3 &index )
{
	if(cells(index) != NULL)
		return;

	cells(index) = ProjectStructs::StructHelper::CreatePhyxelGridCellPointer();
	cells(index)->halfWidth = SmallestHalfWidth;
	cells(index)->surfels = std::vector<ProjectStructs::SURFEL*>();
	cells(index)->edges = std::vector<ProjectStructs::SURFEL_EDGE*>();

	ProjectStructs::PHYXEL_NODE *phyxel = ProjectStructs::StructHelper::CreatePhyxelNodePointer();
	phyxel->pos = GetPositionOfIndex((int)index.x, (int)index.y, (int)index.z, false);
	//phyxel->displacement = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	phyxel->supportRadius = max(this->SmallestHalfWidth.x, max(this->SmallestHalfWidth.y, this->SmallestHalfWidth.z)) * 3;
	phyxel->density = materialProperties.density;

	phyxel->force = D3DXVECTOR3(0.0f, 0.0f, 0.0);
	phyxel->isChanged = false;

	cells(index)->phyxel = phyxel;

	allocatedPhyxels++;
	allocatedCells++;
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

void PhyxelGrid::InitCellAndPushSurfels( D3DXVECTOR3 index, ProjectStructs::SURFEL * surfel, ProjectStructs::SURFEL_EDGE * edge )
{
	Phyxel_Grid_Cell *cell = cells(index);

	InitCell(index);

	if(surfel != NULL){
		// check if the surfelPos is inside of the grid cell

		if(find(cells(index)->surfels.begin(), cells(index)->surfels.end(), surfel) == cells(index)->surfels.end())
			cells(index)->surfels.push_back(surfel);

		if(find(surfel->intersectingCells.begin(), surfel->intersectingCells.end(), cells(index)) == surfel->intersectingCells.end())
			surfel->intersectingCells.push_back(cells(index));
	}
	else{
		// check if the surfelPos is inside of the grid cell
		if(find(cells(index)->edges.begin(), cells(index)->edges.end(), edge) == cells(index)->edges.end())
			cells(index)->edges.push_back(edge);

		if(find(edge->intersectingCells.begin(), edge->intersectingCells.end(), cells(index)) == edge->intersectingCells.end())
			edge->intersectingCells.push_back(cells(index));
	}	
}

void PhyxelGrid::PopulateNodeAndCheckNormal( D3DXVECTOR3 &index, D3DXVECTOR3 surfelPos, ProjectStructs::SURFEL * surfel, ProjectStructs::SURFEL_EDGE * edge )
{
	if(cells.ValidIndex(index)){
		if(MathHelper::Facing(GetPositionOfIndex((int)index.x, (int)index.y, (int)index.z, false), surfelPos, surfel == NULL ? edge->vertex.normal: surfel->vertex.normal)){
			InitCellAndPushSurfels(index, surfel, edge);
		}		
		else{
			// go to the node behind this one and populate it

			D3DXVECTOR3 normalizedNormal;
			D3DXVec3Normalize(&normalizedNormal, surfel == NULL ? &edge->vertex.normal : &surfel->vertex.normal);

			index -= D3DXVECTOR3(normalizedNormal.x, normalizedNormal.y, normalizedNormal.z );

			if(cells.ValidIndex(index)){						
				InitCellAndPushSurfels(index, surfel, edge);			
			}
		}
	}
}