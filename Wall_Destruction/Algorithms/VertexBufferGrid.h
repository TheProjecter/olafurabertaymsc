#ifndef VERTEX_BUFFER_GRID_H
#define VERTEX_BUFFER_GRID_H

#include "Structs.h"
#include "ThreeInOneArray.h"
#include "CustomEffect.h"
#include "PhyxelGrid.h"
#include <map>

struct VertexWithPosAndColor{
	D3DXVECTOR3 pos;
	D3DXVECTOR3 color;
};

class VertexBufferGrid : public Drawable
{
public:
	VertexBufferGrid(void);
	VertexBufferGrid(D3DXVECTOR3 Min, D3DXVECTOR3 Max, D3DXVECTOR3 Pos, ProjectStructs::MATERIAL_PROPERTIES materialProperties);
	~VertexBufferGrid(void);

	void SetPhyxelGrid(PhyxelGrid* phyxelGrid){this->phyxelGrid = phyxelGrid;}
	void InsertPoints(std::vector<ProjectStructs::SURFEL*> surfels);
	bool PopulateNode(ProjectStructs::SURFEL *surfel);
	void ResetSurfel(ProjectStructs::SURFEL *surfel);

	std::map<int, ProjectStructs::Vertex_Grid_Cell*>::const_iterator GetFrontOfPopulatedCells(){
		return populatedCells.begin();
	}

	std::map<int, ProjectStructs::Vertex_Grid_Cell*>::const_iterator GetEndOfPopulatedCells(){
		return populatedCells.end();
	}

	D3DXVECTOR3 GetIndexOfPosition( D3DXVECTOR3 surfelPos );
	void InitCell( D3DXVECTOR3 &index);
	D3DXVECTOR3 GetPositionOfIndex(int x, int y, int z, bool relative);

	void InitCells();
	void Draw(D3DXMATRIX world);
	void Update();
	void CleanUp();		
	void ResetCell(Vertex_Grid_Cell* cell);

	static float RadiusScale, LastRadiusScale;
	static bool isChanged;

private:

	void CleanUpCell( ProjectStructs::Vertex_Grid_Cell* cell);

	void DrawSurfel(Vertex_Grid_Cell* cell);
	void DrawSolid(Vertex_Grid_Cell* cell);
	void DrawWireframe(Vertex_Grid_Cell* cell);
	void DrawNeighbors(Vertex_Grid_Cell* cell);

	void InitSurfel();
	void InitWireframe();
	void InitSolid();
	void InitGeometryPass();
	void InitCommonSolidAndWireframe(Vertex_Grid_Cell* cell);
	
	void ProcessChangedSurfels(Vertex_Grid_Cell* cell);

	void DrawToReadableBuffer(Vertex_Grid_Cell* cell);

	void SetUpNeighborEffect();

	void SetUpNeighborDraw(Vertex_Grid_Cell* cell);

	void InitCell(Vertex_Grid_Cell* cell);

	void SetupSurfelVertexBuffer( Vertex_Grid_Cell* cell );
	void SetupPopulatedCellList();
	void DrawCell(ProjectStructs::Vertex_Grid_Cell* cell);
	ThreeInOneArray<bool> cells;
	
	std::map<int, ProjectStructs::Vertex_Grid_Cell*> populatedCells;
	std::map<ProjectStructs::Vertex_Grid_Cell*, int> inversePopulatedCells;
	ProjectStructs::Vertex_Grid_Cell** populatedCellList;

	std::vector<ProjectStructs::Vertex_Grid_Cell*> changedCells;
	PhyxelGrid* phyxelGrid;

	D3DXVECTOR3 Min;
	D3DXVECTOR3 Max;
	D3DXVECTOR3 Center;
	D3DXVECTOR3 Position;
	D3DXVECTOR3 SmallestHalfWidth;
	D3DVECTOR halfIndex;
	D3DXMATRIX invWorld, World, tmpWorld;
	D3DXVECTOR3 tmp;

	std::string planeTexture;

	int dimensions;

	ProjectStructs::MATERIAL_PROPERTIES materialProperties;
	static ID3D10RasterizerState *SolidRenderState;
	static Helpers::CustomEffect surfelEffect, solidEffect, wireframeEffect, geometryEffect, simpleEffect;
	static ID3D10ShaderResourceView *SurfelTexture, *SurfelWireframeTexture;
	static bool vertexBufferGridInitialized;

	static UINT offset, solidStride, wireframeStride, surfelStride, neighborStride;
	static int CellToDraw;
	int populatedCellCount;
};

#endif