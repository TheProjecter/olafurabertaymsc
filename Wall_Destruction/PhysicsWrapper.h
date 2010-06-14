#ifndef PHYSICS_WRAPPER_H
#define PHYSICS_WRAPPER_H

#include "MeshlessObject.h"
#include "Projectiles.h"
#include "WreckingBall.h"
#include "HavokPhysicsInclude.h"

class PhysicsWrapper
{
public:
	static void Init();
	static void FinishInit();
	static void Update(float dt);
	static void CleanUp();

	static void AddMeshlessObject(MeshlessObject *mo);
	static void AddProjectile(Structs::PROJECTILE *projectile);
	static void AddWreckingBall(WreckingBall *wreckingball);

	static void Pause(){
		if(setupComplete && stopWatch.isRunning())
			stopWatch.stop();
	}
	static void Resume(){
		if(setupComplete && !stopWatch.isRunning())
			stopWatch.start();
	}

	static void RemoveRigidBody(hkpRigidBody *rb){
		physicsWorld->lock();
		physicsWorld->removeEntity(rb);
		physicsWorld->unlock();
	}

	static void SetPosition(hkpRigidBody *rb, D3DXVECTOR3 deltaPos, bool add){

		physicsWorld->lock();
		rb->lockAll();
		rb->markForWrite();
		hkVector4 v;
		
		if(add)
			v = hkVector4(rb->getPosition()(0) + deltaPos.x, rb->getPosition()(1) + deltaPos.y, rb->getPosition()(2) + deltaPos.z);
		else
			v = hkVector4(deltaPos.x, deltaPos.y, deltaPos.z);

		rb->setPosition(v);
		rb->unmarkForWrite();
		rb->unlockAll();
		physicsWorld->unlock();
	}

	// helper functions:
	static D3DXMATRIX GetWorld(const hkpRigidBody *rb){
		float* world = NULL;
		world = new float[16];
		rb->getTransform().get4x4ColumnMajor(world);

		tempTranslationMatrix(0, 0) = world[0];
		tempTranslationMatrix(0, 1) = world[1];
		tempTranslationMatrix(0, 2) = world[2];
		tempTranslationMatrix(0, 3) = world[3];
		tempTranslationMatrix(1, 0) = world[4];
		tempTranslationMatrix(1, 1) = world[5];
		tempTranslationMatrix(1, 2) = world[6];
		tempTranslationMatrix(1, 3) = world[7];
		tempTranslationMatrix(2, 0) = world[8];
		tempTranslationMatrix(2, 1) = world[9];
		tempTranslationMatrix(2, 2) = world[10];
		tempTranslationMatrix(2, 3) = world[11];
		tempTranslationMatrix(3, 0) = world[12];
		tempTranslationMatrix(3, 1) = world[13];
		tempTranslationMatrix(3, 2) = world[14];
		tempTranslationMatrix(3, 3) = world[15];

		delete world;
		return tempTranslationMatrix;	
	}

	static D3DXVECTOR3 GetVector(const hkVector4 *vector){
		hkVector4 v = *vector;
		tempVector.x = v(0);
		tempVector.y = v(1);
		tempVector.z = v(2);

		return tempVector;
	}

private:

	static hkpRigidBody* SetupSphericalRigidBody(float radius, float mass, D3DXVECTOR3 position, D3DXVECTOR3 velocity, bool isStatic);
	static void LinkChain(WreckingBall *wreckingball);
	static void LinkSphereToPlane(WreckingBall *wreckingball, hkpRigidBody* plane);

	static void HK_CALL PhysicsWrapper::errorReport(const char* msg, void* userArgGivenToInit);	

	static hkpWorld* physicsWorld;
	static hkVisualDebugger* vdb;
	static hkpPhysicsContext* context;
	static hkJobQueue* jobQueue;
	static hkJobThreadPool* threadPool;
	static hkStopwatch stopWatch;
	static hkReal lastTime;
	static hkReal timestep;
	static bool setupComplete;

	static MeshlessObject* CeilingObject;

	// temp variables
	static D3DXMATRIX tempTranslationMatrix, tempRotationMatrix;
	static D3DXVECTOR3 tempVector;

};

#endif