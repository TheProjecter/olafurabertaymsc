#ifndef POINT_GRID_H
#define POINT_GRID_H

#include "ThreeInOneArray.h"
#include <D3DX10.h>
#include "Structs.h"
#include "CustomEffect.h"

using namespace ProjectStructs;

class PointGrid
{
public:
	PointGrid(void);
	PointGrid(D3DXVECTOR3 minCoords, D3DXVECTOR3 maxCoords, D3DXVECTOR3 Pos, float dimensions, float smallestHalfWidth);
	~PointGrid(void);
	
	std::vector<ProjectStructs::SURFEL*> InsertPoints(std::vector<ProjectStructs::SURFEL*> surfels);

	void InitCellAndPushSurfels( D3DXVECTOR3 index, ProjectStructs::SURFEL * surfel);
	D3DXVECTOR3 GetIndexOfPosition( D3DXVECTOR3 surfelPos );
	bool InitCell( ProjectStructs::SURFEL* surfel);
	void PopulateNeighbors( D3DXVECTOR3 index );
	D3DXVECTOR3 GetPositionOfIndex(int x, int y, int z, bool relative);
	D3DXMATRIX GetInvWorld(){return invWorld;}

	ThreeInOneArray<ProjectStructs::Point_Grid_Cell*> GetCells(){return cells;}
	ProjectStructs::Point_Grid_Cell* GetCell(D3DXVECTOR3 index){return cells(index);}

	void PopulateNeighbor( D3DXVECTOR3 index, D3DXVECTOR3 neighbor, ProjectStructs::DIRECTIONS indexToNeighborDir);
	void Init();
	void Draw();
	void Update();
	void CleanUp();

	void CleanCell( unsigned int i );

private:
	static int phyxelIndex;
	D3DXVECTOR3 SmallestHalfWidth;

	D3DXVECTOR3 Min;
	D3DXVECTOR3 Max;
	ThreeInOneArray<ProjectStructs::Point_Grid_Cell*> cells;
	D3DXVECTOR3 HalfDimensions;
	D3DXVECTOR3 Position;
	D3DXVECTOR3 halfIndex, tmp;
	D3DXMATRIX invWorld, World, tmpWorld;
	Helpers::CustomEffect effect;
	ID3D10Buffer *vertexBuffer, *indexBuffer, *otherIndexBuffer;
};

#endif