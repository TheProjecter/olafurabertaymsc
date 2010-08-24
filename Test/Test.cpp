// Test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "PhyxelGrid.h"
#include "Structs.h"

void TestSimple(){
	ProjectStructs::MATERIAL_PROPERTIES properties;
	properties.density = 2400;
	properties.phyxelGridSize = 2.0;
	D3DXVECTOR3 MIN = D3DXVECTOR3(-10.0f, -10.0f, -10.0f);
	D3DXVECTOR3 MAX = D3DXVECTOR3(10.0f, 10.0f, 10.0f);
	D3DXVECTOR3 pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	PhyxelGrid grid = PhyxelGrid(MIN, MAX, pos, properties);

	for(float i = MIN.x; i<MAX.x; i++){
		for(float j = MIN.y; j<MAX.y; j++){
			for(float k = MIN.z; k<MAX.z; k++){
				D3DXVECTOR3 index = grid.GetIndexOfPosition(D3DXVECTOR3(i, j, k));
				printf("...");
			}
		}
	}

	for(int i = 0; i<grid.GetCells().GetWidth(); i++){
		for(int j = MIN.y; j<grid.GetCells().GetHeight(); j++){
			for(int k = MIN.z; k<grid.GetCells().GetDepth(); k++){
				D3DXVECTOR3 pos = grid.GetPositionOfIndex(i, j, k, true);
				
				D3DXVECTOR3 pos2 = grid.GetPositionOfIndex(i, j, k, false);
				printf("...");
			}
		}
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	TestSimple();

	return 0;
}

