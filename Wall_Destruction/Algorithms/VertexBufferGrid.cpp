#include "VertexBufferGrid.h"

VertexBufferGrid::VertexBufferGrid(void)
{
}

VertexBufferGrid::~VertexBufferGrid(void)
{
}

VertexBufferGrid::VertexBufferGrid(int dimensions, D3DXVECTOR3 Min, D3DXVECTOR3 Max, D3DXVECTOR3 Pos){
	this->dimensions = dimensions;
	this->Min = Min;
	this->Max = Max;
	this->Center = (this->Max - this->Min) * 0.5f;

	this->SmallestHalfWidth = Center / (float)dimensions;

	this->Position = Pos;

	this->cells = ThreeInOneArray<bool>(
		(int)floor((Max - Min).x/(2.0f*SmallestHalfWidth.x) + 1), 
		(int)floor((Max - Min).y/(2.0f*SmallestHalfWidth.y) + 1 ), 
		(int)floor((Max - Min).z/(2.0f*SmallestHalfWidth.z) + 1));

	this->halfIndex = D3DXVECTOR3(this->cells.GetWidth() * 0.5f, this->cells.GetHeight() * 0.5f, this->cells.GetDepth() * 0.5f);

	D3DXMatrixTranslation(&World, Position.x, Position.y, Position.z);

	D3DXMatrixTranslation(&this->invWorld, -Min.x - Center.x, -Min.y - Center.y, -Min.z- Center.z);
}


void VertexBufferGrid::InsertPoints(std::vector<D3DXVECTOR3> surfacePoints, std::vector<ProjectStructs::SURFEL*> surfels){
	for(unsigned int i = 0; i<surfacePoints.size(); i++){
		PopulateNode( surfacePoints[i], surfels[i], NULL);
	}
}

void VertexBufferGrid::InsertEdges(std::vector<D3DXVECTOR3> edgePoints, std::vector<ProjectStructs::SURFEL_EDGE*> edges){
	for(unsigned int i = 0; i<edgePoints.size(); i++){
		PopulateNode( edgePoints[i], NULL, edges[i]);
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

	this->cells.Clear();
}

void VertexBufferGrid::PopulateNode( D3DXVECTOR3 surfelPos, ProjectStructs::SURFEL *surfel, ProjectStructs::SURFEL_EDGE *edge)
{
	D3DXVECTOR3 index = GetIndexOfPosition(surfelPos);
	int i;

	if(cells(index) == NULL){
		// insert a new thing in the map
		i = cells.GetIndex((unsigned int)index.x, (unsigned int)index.y, (unsigned int)index.z);
		if(populatedCells.find(i) == populatedCells.end()){
			populatedCells[i] = new ProjectStructs::Vertex_Grid_Cell;
			populatedCells[i]->buffer = NULL;
		}

		if(surfel != NULL)
			populatedCells[i]->surfels.push_back(surfel);
		else{
			populatedCells[i]->edges.push_back(edge);
		}
	}
	else {
		printf("hmm...");
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
