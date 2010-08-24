#ifndef CROSSHAIR_H
#define CROSSHAIR_H

#include <D3D10.h>
#include "CustomEffect.h"

class Crosshair
{
public:
	Crosshair(void);
	~Crosshair(void);
	void Init();
	void ResetVertexBuffer();
	void Draw();
	void CleanUp();

private:
	ID3D10Buffer* crosshairBuffer;
	Helpers::CustomEffect crosshairEffect;
	float size;
};

#endif