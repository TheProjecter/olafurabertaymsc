#include "Camera.h"
#include "Globals.h"
#include "MouseHandler.h"
#include "KeyboardHandler.h"

using namespace Helpers;

Camera::Camera(D3DXVECTOR3 pos, D3DXVECTOR3 lookAt){
	position = pos;

	initialForward = lookAt - pos;
	D3DXVec3Normalize(&initialForward, &initialForward);
	forward = initialForward;

	deltaMovement = 10.0f;
	deltaRotation = 1.0f;
	yaw = 0.0f;
	pitch = 0.0f;

	D3DXMATRIX rot;
	D3DXMatrixRotationY(&rot, (float)D3DX_PI/2.0f);

	D3DXVec3TransformCoord(&initialRight, &forward, &rot);
	D3DXVec3Normalize(&initialRight, &initialRight);
	
	right = initialRight;

	up = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	D3DXMatrixLookAtLH(&view, &position, &lookAt, &up);
	Reset();
}	

void Camera::Reset(){
	D3DXMatrixPerspectiveFovLH(&projection, Globals::HALF_PI * ((float)Globals::ClientHeight/ (float)Globals::ClientWidth), (float)Globals::ClientWidth/ (float)Globals::ClientHeight, 0.001f, 1000.0f);
}

void Camera::Update(float dt){
	bool changed = false;

	if(!Helpers::Globals::MOVE_WRECKINGBALL){
		if(Helpers::KeyboardHandler::IsKeyDown(DIK_W)){
			changed = true;

			position += forward * dt * deltaMovement;
		}
		else if(Helpers::KeyboardHandler::IsKeyDown(DIK_S)){
			changed = true;
			
			position -= forward * dt * deltaMovement;
		}

		if(Helpers::KeyboardHandler::IsKeyDown(DIK_A)){
			changed = true;
			
			position -= right * dt * deltaMovement;
		}
		else if(Helpers::KeyboardHandler::IsKeyDown(DIK_D)){
			changed = true;

			position += right * dt * deltaMovement;
		}
	}

	// look around, the MouseState object comes from the MouseHandler
	if(MouseHandler::MouseState.lX != 0 || MouseHandler::MouseState.lY != 0){
		changed = true;	

		yaw += dt*deltaRotation*((float)MouseHandler::MouseState.lX) * ((float)Helpers::Globals::ClientHeight / (float)Helpers::Globals::ClientWidth);
		pitch += dt*deltaRotation*((float)MouseHandler::MouseState.lY) * ((float)Helpers::Globals::ClientHeight / (float)Helpers::Globals::ClientWidth );

		if(yaw < 0.0f)
			yaw += Globals::TWO_PI;
		else if(yaw > Globals::TWO_PI)
			yaw -= Globals::TWO_PI;
		
		if(pitch < -Globals::HALF_PI+0.1f)
			pitch = -Globals::HALF_PI+0.1f;
		else if(pitch > Globals::HALF_PI-0.1f)
			pitch = Globals::HALF_PI-0.1f;
	}
	
	if(changed){
		D3DXMATRIX rotation;
		D3DXMatrixRotationYawPitchRoll(&rotation, yaw, pitch, 0.0f);

		D3DXVec3TransformCoord(&forward, &initialForward, &rotation);
		D3DXVec3TransformCoord(&right, &initialRight, &rotation);
		
		D3DXMatrixLookAtLH(&view, &position, &(position + forward), &up);
		D3DXMatrixInverse(&invView, NULL, &(view));
	}
}
