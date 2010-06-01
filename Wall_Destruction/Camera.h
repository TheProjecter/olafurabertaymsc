#ifndef CAMERA_H
#define CAMERA_H

#include <D3DX10.h>

class Camera
{
public:
	Camera(void){};

	Camera(D3DXVECTOR3 pos, D3DXVECTOR3 lookat);	
	void Reset();
	void Update(float dt);

	~Camera(void){};

	D3DXMATRIX View(){return view;}
	D3DXMATRIX Projection(){return projection;}

	
private:
	D3DXMATRIX view, projection;
	D3DXVECTOR3 position, lookAt, right, forward, initialForward, initialRight, up;
	float deltaMovement, deltaRotation, yaw, pitch;
};

#endif

