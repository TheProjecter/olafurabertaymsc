#ifndef CHAIN_H
#define CHAIN_H

#include <D3DX10.h>
#include <vector>
#include "Sphere.h"
#include "CustomEffect.h"

class Chain : public Drawable
{
public:
	Chain(void){};
	~Chain(void){};

	void Init(D3DXVECTOR3 start, D3DXVECTOR3 end, int count);
	void Update(float dt);
	void Draw();
	void DrawDepth();
	void CleanUp();

	int GetCount(){return chainSpheres.size();}
	D3DXVECTOR3 GetDelta(){return delta;}

	Sphere* operator [] (unsigned int index) {
		return chainSpheres[index];
	}	

private:
	std::vector<Sphere*> chainSpheres;
	Helpers::CustomEffect effect, depthEffect;

	D3DXVECTOR3 start, end, delta;
	D3DXMATRIX invWorldView;

};

#endif