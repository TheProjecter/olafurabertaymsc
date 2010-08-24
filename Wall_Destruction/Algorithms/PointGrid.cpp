#include "PointGrid.h"
#include "MathHelper.h"
#include "FractureManager.h"
#include "KeyboardHandler.h"
#include "RandRange.h"
#include "DebugToFile.h"
#include <algorithm>

PointGrid::PointGrid(void)
{
}

PointGrid::~PointGrid(void)
{
}

PointGrid::PointGrid(D3DXVECTOR3 minCoords, D3DXVECTOR3 maxCoords, D3DXVECTOR3 Pos, float dimensions, float smallestHalfWidth){

	this->Min = minCoords;
	this->Max = maxCoords;

	this->HalfDimensions = (this->Max - this->Min) * 0.5f;
	this->HalfDimensions.x = abs(this->HalfDimensions.x);
	this->HalfDimensions.y = abs(this->HalfDimensions.y);
	this->HalfDimensions.z = abs(this->HalfDimensions.z);

	this->SmallestHalfWidth = 2.0f * HalfDimensions / dimensions;// / (float)10.0f;
	if(smallestHalfWidth < SmallestHalfWidth.x || 
		smallestHalfWidth < SmallestHalfWidth.y || 
		smallestHalfWidth < SmallestHalfWidth.z){
			this->SmallestHalfWidth = smallestHalfWidth * D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	}else{
		SmallestHalfWidth.x = min(SmallestHalfWidth.x, min(SmallestHalfWidth.y, min(SmallestHalfWidth.z, smallestHalfWidth)));

		if(this->SmallestHalfWidth.x == 0.0f)
			this->SmallestHalfWidth.x = 1.0f;

		SmallestHalfWidth.y = SmallestHalfWidth.x;
		SmallestHalfWidth.z = SmallestHalfWidth.x;
	}

	this->Position = Pos;

	this->cells = ThreeInOneArray<ProjectStructs::Point_Grid_Cell*>(
				(int)floor(HalfDimensions.x / SmallestHalfWidth.x + 1),
				(int)floor(HalfDimensions.y / SmallestHalfWidth.y + 1),
				(int)floor(HalfDimensions.z / SmallestHalfWidth.z + 1));

//	this->cells = ThreeInOneArray<ProjectStructs::Point_Grid_Cell*>(xDim, yDim, zDim);

	for(unsigned int i = 0; i<cells.GetSize(); i++){
		cells[i] = NULL;
	}

	this->halfIndex = D3DXVECTOR3(this->cells.GetWidth() * 0.5f, this->cells.GetHeight() * 0.5f, this->cells.GetDepth() * 0.5f);
		
	D3DXMatrixTranslation(&World, Position.x, Position.y, Position.z);

	D3DXVECTOR3 center = (this->Max + this->Min) * 0.5f;
	//D3DXMatrixTranslation(&this->invWorld, center.x, center.y, center.z);
	D3DXMatrixTranslation(&this->invWorld, -Min.x - HalfDimensions.x, -Min.y - HalfDimensions.y, -Min.z - HalfDimensions.z);
	//D3DXMatrixInverse(&invWorld, NULL, &invWorld);
	//D3DXMatrixTranspose(&invWorld, &invWorld);


	Helpers::Globals::DebugInformation.AddText(DEBUG_TYPE, "Created point grid (%d x %d x %d)", cells.GetWidth(), cells.GetHeight(), cells.GetDepth());
	DebugToFile::Debug("PointGrid Min (%.3f, %.3f, %.3f), Max (%.3f, %.3f, %.3f)", Min.x, Min.y, Min.z, Max.x, Max.y, Max.z);
	DebugToFile::Debug("PointGrid Position (%.3f, %.3f, %.3f), Dimensions (%.3f, %.3f, %.3f)", Pos.x, Pos.y, Pos.z, HalfDimensions.x, HalfDimensions.y, HalfDimensions.z);
}

std::vector<ProjectStructs::SURFEL*> PointGrid::InsertPoints(std::vector<ProjectStructs::SURFEL*> surfels){
	
	for(unsigned int i = 0; i<surfels.size(); i++){
		InitCell(surfels[i]);
	}	
/*
	D3DXVECTOR3 nearestIndex, secondNearestIndex;
	D3DXVECTOR3 firstIndex, secondIndex, nextSecondIndex, delta, direction;
	float shortestDistance = FLT_MAX;
	ProjectStructs::SURFEL *currentSurfel = NULL, *closestSurfel = NULL, *nextClosestSurfel = NULL;

	unsigned int originalSurfelCount = surfels.size();
	for(unsigned int i = 0; i<originalSurfelCount; i++){

		currentSurfel = surfels[i];
		shortestDistance = FLT_MAX;
		firstIndex = GetIndexOfPosition(currentSurfel->vertex->pos);
		closestSurfel = NULL;
		nextClosestSurfel = NULL;

		// find the closest surfel
		for(unsigned int j = 0; j<originalSurfelCount; j++){
			if(i == j)
				continue;

			secondIndex = GetIndexOfPosition(surfels[j]->vertex->pos);

			if(D3DXVec3Length(&(firstIndex - secondIndex)) < shortestDistance){
				shortestDistance = D3DXVec3Length(&(firstIndex - secondIndex));
				
				closestSurfel= surfels[j];
			}
		}

		shortestDistance = FLT_MAX;
		float lastAngle = FLT_MAX;

		// find the next closest surfel
		for(unsigned int j = 0; j<originalSurfelCount; j++){
			if(i == j || surfels[j] == closestSurfel)
				continue;

			nextSecondIndex = GetIndexOfPosition(surfels[j]->vertex->pos);
			float currentAngle = abs(Helpers::Globals::HALF_PI - abs(MathHelper::GetAngleBetweenVectors(surfels[i]->vertex->pos - surfels[j]->vertex->pos, surfels[i]->vertex->pos - closestSurfel->vertex->pos)));

			if(lastAngle == FLT_MAX || currentAngle <= lastAngle){
				shortestDistance = D3DXVec3Length(&(firstIndex - nextSecondIndex));
				lastAngle = currentAngle;
				nextClosestSurfel = surfels[j];
			}
		}
		
		int count = 1;
		ProjectStructs::SURFEL* surfelBeingUsed = closestSurfel;
		while(count >= 0){
			if(surfelBeingUsed->vertex->pos.x < currentSurfel->vertex->pos.x || 
				surfelBeingUsed->vertex->pos.y < currentSurfel->vertex->pos.y || 
				surfelBeingUsed->vertex->pos.z < currentSurfel->vertex->pos.z )
				delta = currentSurfel->vertex->pos - surfelBeingUsed->vertex->pos;
			else
				delta = surfelBeingUsed->vertex->pos - currentSurfel->vertex->pos;

			if(delta.x < 0.0f)
				delta.x *= -1;
			if(delta.y < 0.0f)
				delta.y *= -1;
			if(delta.z < 0.0f)
				delta.z *= -1;

			direction.x = 2.0f * SmallestHalfWidth.x / delta.x;
			direction.y = 2.0f * SmallestHalfWidth.y / delta.y;
			direction.z = 2.0f * SmallestHalfWidth.z / delta.z ;

			int count = 0;
			for(float j = direction.x; j<=delta.x - direction.x; j+= direction.x){
				for(float k = direction.y; k<=delta.y - direction.y; k+= direction.y){
					for(float l = direction.z; l<=delta.z - direction.z; l+= direction.z){
						count++;
						ProjectStructs::SURFEL* surfel = ProjectStructs::StructHelper::CreateSurfelPointer(
							currentSurfel->vertex->pos + D3DXVECTOR3(j, k, l), 
							MathHelper::GetZeroVector(),
							MathHelper::GetZeroVector(), 
							MathHelper::GetZeroVector(),
							currentSurfel->vertex->UV + (surfelBeingUsed->vertex->UV - currentSurfel->vertex->UV) / D3DXVec3Length(&delta),
							currentSurfel->vertex->deltaUV, 
							currentSurfel->surfaceIndex);
						
						if(InitCell(surfel)){
							surfels.push_back(surfel);
						}
						else{
							delete surfel;
						}
						
						if(direction.z == 0)
							break;
					}
					if(direction.y == 0)
						break;
				}
				if(direction.x == 0)
					break;
			}

			surfelBeingUsed = nextClosestSurfel;
			count--;
		}
	}
*/
	for(unsigned int i = 0; i<surfels.size(); i++){
		D3DXVECTOR3 index = GetIndexOfPosition(surfels[i]->vertex->pos);

		if(cells.ValidIndex(index)){
			PopulateNeighbors(index);
			cells(index)->neighbors[THIS_NODE] = cells(index);
		}
	}		


	return surfels;
}

/*void PointGrid::InsertEdges(std::vector<D3DXVECTOR3> edgePoints, std::vector<ProjectStructs::SURFEL_EDGE*> edges){
	
}*/

void PointGrid::PopulateNeighbors( D3DXVECTOR3 index )
{
	D3DXVECTOR3 tmp = index;
	
	int count = 0;
	for(int i = 0; i<3; i++){
		for(int j = 0; j<3; j++){
			for(int k = 0; k<3; k++){
				if(i == 1 && j == 1 && k == 1){
					count++;
					continue;
				}

				tmp.x = index.x + (i-1);
				tmp.y = index.y + (j-1);
				tmp.z = index.z + (k-1);

				PopulateNeighbor(index, tmp, (DIRECTIONS)count++);
			}
		}
	}
}

void PointGrid::PopulateNeighbor( D3DXVECTOR3 index, D3DXVECTOR3 neighbor, DIRECTIONS indexToNeighborDir)
{
	if(!cells.ValidIndex(neighbor) || cells(neighbor) == NULL || cells(index) == NULL){
		return;
	}

	if(cells(index)->neighbors[(int)indexToNeighborDir] == NULL && cells.ValidIndex(neighbor)){
		if(cells(neighbor)->neighbors[(int)BACK_TOP_RIGHT - (int)indexToNeighborDir]  == NULL){
			cells(index)->neighbors[(int)indexToNeighborDir] = cells(neighbor);
			cells(neighbor)->neighbors[(int)BACK_TOP_RIGHT - (int)indexToNeighborDir] = cells(index);		
		}
	}
}

D3DXVECTOR3 PointGrid::GetPositionOfIndex(int x, int y, int z, bool relative){

	D3DXVECTOR3 pos = D3DXVECTOR3();

	pos.x = (x - this->halfIndex.x) * 2.0f * SmallestHalfWidth.x + SmallestHalfWidth.x;
	pos.y = (y - this->halfIndex.y) * 2.0f * SmallestHalfWidth.y + SmallestHalfWidth.y;
	pos.z = (z - this->halfIndex.z) * 2.0f * SmallestHalfWidth.z + SmallestHalfWidth.z;

	if(!relative)
		pos += Position;

	//DebugToFile::Debug("Position of index (%f, %f, %f) is (%.3f, %.3f, %.3f)", x, y, z, pos.x, pos.y, pos.z);

	return pos;
}

void PointGrid::Init()
{
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
	effect.SetFloatVector("Color", D3DXVECTOR4(RandRange::Rand(0.0f, 1.0f), RandRange::Rand(0.0f, 1.0f), RandRange::Rand(0.0f, 1.0f), 1.0f));		

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

bool PointGrid::InitCell( ProjectStructs::SURFEL* surfel)
{
	D3DXVECTOR3 index = GetIndexOfPosition(surfel->vertex->pos);
	
	if(!cells.ValidIndex(index))
		return false;
	
	if(cells(index) != NULL){
		cells(index)->surfels.push_back(surfel);
		return true;
	}

	cells(index) = ProjectStructs::StructHelper::CreatePointGridCell();	
	cells(index)->pos = GetPositionOfIndex(index.x, index.y, index.z, false);
	cells(index)->surfels.push_back(surfel);

	return true;
}

D3DXVECTOR3 PointGrid::GetIndexOfPosition( D3DXVECTOR3 surfelPos )
{
	D3DXVec3TransformCoord(&tmp, &surfelPos, &invWorld);
	D3DXVECTOR3 index = tmp + HalfDimensions;
	index.x = floor(index.x/(2.0f*SmallestHalfWidth.x));
	index.y = floor(index.y/(2.0f*SmallestHalfWidth.y));
	index.z = floor(index.z/(2.0f*SmallestHalfWidth.z));

	return index;
}

void PointGrid::Draw(){

	effect.SetMatrix("View", Helpers::Globals::AppCamera.View());

	effect.PreDraw();

	UINT stride = sizeof( ProjectStructs::SIMPLE_VERTEX);
	UINT offset = 0;
	int index = 0;

	Helpers::Globals::Device->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP);
	Helpers::Globals::Device->IASetVertexBuffers( 0, 1, &vertexBuffer, &stride, &offset );

	for(unsigned int i = 0; i < cells.GetSize(); i++){
		
		if(cells[i] != NULL){
			
			INDEX indices = cells.GetIndices(i);
			tmpWorld = World;

			D3DXVECTOR3 pos = GetPositionOfIndex(indices.x, indices.y, indices.z, false);

			tmpWorld._41 = pos.x;// Center.x;
			tmpWorld._42 = pos.y;// + Center.y;
 			tmpWorld._43 = pos.z;//+ Center.z;
		//	tmpWorld._41 = cells[i]->pos.x;
		//	tmpWorld._42 = cells[i]->pos.y;
		//	tmpWorld._43 = cells[i]->pos.z;

			effect.SetMatrix("World", tmpWorld);

			Helpers::Globals::Device->IASetIndexBuffer( indexBuffer, DXGI_FORMAT_R32_UINT, 0 );
			effect.DrawIndexed(8);

			Helpers::Globals::Device->IASetIndexBuffer( otherIndexBuffer, DXGI_FORMAT_R32_UINT, 0 );
			effect.DrawIndexed(8);
		}	
	}
}


void PointGrid::CleanUp(){
	for(unsigned int i = 0; i < cells.GetSize(); i++){
		delete cells[i];
	}
	effect.CleanUp();
	ReleaseCOM(vertexBuffer);
	ReleaseCOM(indexBuffer);
	ReleaseCOM(otherIndexBuffer);
}

