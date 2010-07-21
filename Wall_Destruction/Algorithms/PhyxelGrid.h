#ifndef PHYXEL_GRID_H
#define PHYXEL_GRID_H

#include "ThreeInOneArray.h"
#include "CustomEffect.h"
#include <D3DX10.h>
#include <D3D10.h>
#include "Surface.h"
#include "Structs.h"

using namespace ProjectStructs;

class PhyxelGrid
{
public:
	PhyxelGrid(void);
	PhyxelGrid(int dimensions, D3DXVECTOR3 Min, D3DXVECTOR3 Max, D3DXVECTOR3 Pos, ProjectStructs::MATERIAL_PROPERTIES materialProperties);
	~PhyxelGrid(void);

	void InsertPoints(std::vector<D3DXVECTOR3> points, std::vector<ProjectStructs::SURFEL*> surfels);
	void InsertEdges(std::vector<D3DXVECTOR3> edgePoints, std::vector<ProjectStructs::SURFEL_EDGE*> edges);

	void PopulateNode( D3DXVECTOR3 surfelPos, ProjectStructs::SURFEL *surfel, ProjectStructs::SURFEL_EDGE *edge);

	void PopulateNodeAndCheckNormal( D3DXVECTOR3 &index, D3DXVECTOR3 surfelPos, ProjectStructs::SURFEL * surfel, ProjectStructs::SURFEL_EDGE * edge );
	void InitCellAndPushSurfels( D3DXVECTOR3 index, ProjectStructs::SURFEL * surfel, ProjectStructs::SURFEL_EDGE * edge );
	D3DXVECTOR3 GetIndexOfPosition( D3DXVECTOR3 surfelPos );
	void InitCell( D3DXVECTOR3 &index);
	void PopulateNeighbors( D3DXVECTOR3 index );
	D3DXVECTOR3 GetPositionOfIndex(int x, int y, int z, bool relative);

	ThreeInOneArray<ProjectStructs::Phyxel_Grid_Cell*> GetCells(){return cells;}

	void PopulateNeighbor( D3DXVECTOR3 index, D3DXVECTOR3 neighbor, ProjectStructs::DIRECTIONS indexToNeighborDir);
	void Init(ChangedPhyxels *changedPhyxels);
	void Draw();
	void Update();
	void CleanUp();

	void CleanCell( unsigned int i );
private:
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
	int dimensions;
};

#endif