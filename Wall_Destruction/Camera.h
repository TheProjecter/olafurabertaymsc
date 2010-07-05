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
	D3DXMATRIX ViewProjection(){return viewProjection;}
	D3DXMATRIX InvView(){return invView;}

	D3DXVECTOR3 Position(){return position;}
	D3DXVECTOR3 Forward(){return forward;}
	float FOV(){return fov;}
	float FOVDivBy2(){return fov*0.5f;}

	float NearHeight(){return nearHeight;}
	float NearWidth(){return nearWidth;}
	float ViewPortWidth(){return viewPortWidth;}
	float ViewPortHeight(){return viewPortHeight;}
	float FarPlane(){return far_plane;}
	float NearPlane(){return near_plane;}

	
private:
	D3DXMATRIX view, projection, invView, viewProjection;
	D3DXVECTOR3 position, lookAt, right, forward, initialForward, initialRight, up;
	float deltaMovement, deltaRotation, yaw, pitch;
	float fov;
	float near_plane;
	float far_plane;
	float nearHeight, nearWidth, viewPortWidth, viewPortHeight;
};

#endif

