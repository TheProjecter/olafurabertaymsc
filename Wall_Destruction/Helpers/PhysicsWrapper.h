#ifndef PHYSICS_WRAPPER_H
#define PHYSICS_WRAPPER_H

#include "MeshlessObject.h"
#include "Projectiles.h"
#include "WreckingBall.h"
#include "HavokPhysicsInclude.h"
#include "Surface.h"
#include "Structs.h"
#include <exception>

class PhysicsWrapper
{
public:
	static void Init();
	static void FinishInit();
	static void Update(float dt);
	static void CleanUp();

	static void AddSurface(Surface* surface);
	static void AddSurfels(std::vector<D3DXVECTOR3> points, std::vector<ProjectStructs::SURFEL*> surfels, ProjectStructs::MATERIAL_PROPERTIES materialProperties, D3DXVECTOR3 pos);
	static void AddSurfels(std::vector<D3DXVECTOR3> points, std::vector<ProjectStructs::SURFEL*> surfels, ProjectStructs::MATERIAL_PROPERTIES materialProperties, D3DXVECTOR3 pos, bool lockWorld);
	static void AddProjectile(ProjectStructs::PROJECTILE *projectile);
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
		if(!rb)
			return;

		LockWorld();
		physicsWorld->removeEntity(rb);

		UnLockWorld();
	}

	static void RemoveRigidBodyWithoutLockingWorld(hkpRigidBody *rb, ContactListener* cl){
		if(rb == NULL)
			return;
		
		physicsWorld->lock();

		//rb->removeReference();
		rb->removeContactListener((hkpContactListener*)cl);
		physicsWorld->removeEntity(rb);
		//rb
		//delete rb;
		rb = NULL;
		delete cl;

		physicsWorld->unlock();
		/*
		delete rb;
		rb = NULL;*/
	}

	static void LW(){
		physicsWorld->lock();
	}

	static void ULW(){
		physicsWorld->unlock();
	}

	static void LockWorld(){
		if(setupComplete && !physicsWorld->m_isLocked){
			physicsWorld->lock();
			physicsWorld->markForWrite();
		}
	}

	static void UnLockWorld(){
		if(setupComplete && physicsWorld->m_isLocked){
			physicsWorld->unlock();
			physicsWorld->unmarkForWrite();
		}
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

	static hkpRigidBody* SetupSphericalRigidBody(float radius, float mass, D3DXVECTOR3 position, D3DXVECTOR3 velocity, bool isStatic, ProjectStructs::PROJECTILE *projectile);

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

	static float RigidBodyCount;

};

#endif