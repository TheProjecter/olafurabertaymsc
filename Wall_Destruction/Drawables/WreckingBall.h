#ifndef WRECKING_BALL_H
#define WRECKING_BALL_H

#include <D3DX10.h>
#include <D3D10.h>
#include "Sphere.h"
#include "Chain.h"
#include "CustomEffect.h"
#include "HavokPhysicsInclude.h"

namespace Drawables{
	class WreckingBall : public Drawable
	{
	public:
		WreckingBall(void){radius = 10.0f;};
		WreckingBall(float radius){this->radius = radius;};
		~WreckingBall(void){};
		
		void Init();
		void Draw();
		void DrawDepth();
		void Update(float dt);
		void CleanUp(bool releaseRigid);
		void ResetBuffers();

		float GetRadius(){return radius;}
		void SetRigidBody( hkpRigidBody* rigidBody ){this->sphere.SetRigidBody(rigidBody);};
		hkpRigidBody* GetRigidBody( ){return this->sphere.GetRigidBody();};
		D3DXVECTOR3 GetPosition(){return sphere.GetPositionVector();}
		void SetPosition(D3DXVECTOR3 pos){this->sphere.SetPosition(pos.x, pos.y, pos.z);}
		Chain GetChain(){return chain;}
		D3DXVECTOR3 GetLastVelocity(){return lastVelocity;}
		void SetLastVelocity(D3DXVECTOR3 vel){lastVelocity = vel;}
	private:
		float radius, chainMovementSpeed;
		D3DXVECTOR3 pos;
		D3DXMATRIX scaleMatrix, rotationMatrix, translationMatrix, worldMatrix, invWorldView;;
		Sphere sphere;
		int subsets;
		Chain chain;
		D3DXVECTOR3 lastVelocity;

		Helpers::CustomEffect wreckingBallEffect, depthEffect;
	};
}

#endif