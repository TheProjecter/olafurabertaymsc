#ifndef VERTEX_BUFFER_GRID_H
#define VERTEX_BUFFER_GRID_H

#include "Structs.h"
#include "ThreeInOneArray.h"
#include <map>

class VertexBufferGrid
{
public:
	VertexBufferGrid(void);
	VertexBufferGrid(int dimensions, D3DXVECTOR3 Min, D3DXVECTOR3 Max, D3DXVECTOR3 Pos);
	~VertexBufferGrid(void);

	void InsertPoints(std::vector<D3DXVECTOR3> points, std::vector<ProjectStructs::SURFEL*> surfels);
	void InsertEdges(std::vector<D3DXVECTOR3> edgePoints, std::vector<ProjectStructs::SURFEL_EDGE*> edges);

	void PopulateNode( D3DXVECTOR3 surfelPos, ProjectStructs::SURFEL *surfel, ProjectStructs::SURFEL_EDGE *edge);

	D3DXVECTOR3 GetIndexOfPosition( D3DXVECTOR3 surfelPos );
	void InitCell( D3DXVECTOR3 &index);
	D3DXVECTOR3 GetPositionOfIndex(int x, int y, int z, bool relative);

	void Init();
	void Draw();
	void Update();
	void CleanUp();


private:
	// this 3d array is only used as a position checker
	ThreeInOneArray<bool> cells;

	std::map<int, ProjectStructs::Vertex_Grid_Cell*> populatedCells;

	D3DXVECTOR3 Min;
	D3DXVECTOR3 Max;
	D3DXVECTOR3 Center;
	D3DXVECTOR3 Position;
	D3DXVECTOR3 SmallestHalfWidth;
	D3DVECTOR halfIndex;
	D3DXMATRIX invWorld, World, tmpWorld;
	D3DXVECTOR3 tmp;

	int dimensions;

};

#endif