#ifndef PHYXEL_GRID_H
#define PHYXEL_GRID_H

#include "ThreeInOneArray.h"
#include "ImpactList.h"
#include <D3DX10.h>
#include <D3D10.h>
#include "Surface.h"
#include "Structs.h"

using namespace ProjectStructs;

class PhyxelGrid
{
public:
	PhyxelGrid(void);
	PhyxelGrid(D3DXVECTOR3 Min, D3DXVECTOR3 Max, D3DXVECTOR3 Pos, ProjectStructs::MATERIAL_PROPERTIES materialProperties);
	~PhyxelGrid(void);

	void InsertPoints(std::vector<D3DXVECTOR3> points, std::vector<ProjectStructs::SURFEL*> surfels);
	void InsertPoint(std::vector<D3DXVECTOR3> surfacePoints, ProjectStructs::SURFEL* surfel);
	void InsertCrack(ProjectStructs::CRACK_NODE* crack);

	void PopulateNode( D3DXVECTOR3 surfelPos, ProjectStructs::SURFEL *surfel);

	void PopulateNodeAndCheckNormal( D3DXVECTOR3 &index, D3DXVECTOR3 surfelPos, ProjectStructs::SURFEL * surfel);
	void InitCellAndPushSurfels( D3DXVECTOR3 index, ProjectStructs::SURFEL * surfel);
	D3DXVECTOR3 GetIndexOfPosition( D3DXVECTOR3 surfelPos );
	void InitCell( D3DXVECTOR3 &index);
	void PopulateNeighbors( D3DXVECTOR3 index );
	D3DXVECTOR3 GetPositionOfIndex(int x, int y, int z, bool relative);
	D3DXMATRIX GetInvWorld(){return invWorld;}

	ThreeInOneArray<ProjectStructs::Phyxel_Grid_Cell*> GetCells(){return cells;}

	void PopulateNeighbor( D3DXVECTOR3 index, D3DXVECTOR3 neighbor, ProjectStructs::DIRECTIONS indexToNeighborDir);
	void Init();
	void Draw();
	void DrawSingleCell(Phyxel_Grid_Cell* cell);
	void Update();
	void CleanUp();

	void CleanCell( unsigned int i );

	static void CleanIntersectingCells(ProjectStructs::SURFEL* surfel);
private:

	static int phyxelIndex;
	D3DXVECTOR3 SmallestHalfWidth;
	ID3D10Buffer *vertexBuffer, *indexBuffer, *otherIndexBuffer;

	Helpers::CustomEffect effect;
	int allocatedCells, deallocatedCells, allocatedPhyxels, deallocatedPhyxels;
	
	D3DXVECTOR3 Min;
	D3DXVECTOR3 Max;
	ThreeInOneArray<ProjectStructs::Phyxel_Grid_Cell*> cells;
	D3DXVECTOR3 Center;
	D3DXVECTOR3 Position;
	D3DVECTOR halfIndex;
	D3DXMATRIX invWorld, World, tmpWorld;
	D3DXVECTOR3 tmp;

	ProjectStructs::MATERIAL_PROPERTIES materialProperties;
};

#endif