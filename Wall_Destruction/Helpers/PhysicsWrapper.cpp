#include "PhysicsWrapper.h"
#include "HavokPhysicsInclude.h"
#include "ContactListener.h"
#include <D3DX10.h>
#include <D3D10.h>
#include "Structs.h"
#include "MathHelper.h"
#include <vector>

hkpWorld* PhysicsWrapper::physicsWorld;
hkVisualDebugger* PhysicsWrapper::vdb;
hkpPhysicsContext* PhysicsWrapper::context;
hkJobQueue* PhysicsWrapper::jobQueue;
hkJobThreadPool* PhysicsWrapper::threadPool;
hkStopwatch PhysicsWrapper::stopWatch;
hkReal PhysicsWrapper::lastTime;
hkReal PhysicsWrapper::timestep;
D3DXMATRIX PhysicsWrapper::tempTranslationMatrix, PhysicsWrapper::tempRotationMatrix;
D3DXVECTOR3 PhysicsWrapper::tempVector;
bool PhysicsWrapper::setupComplete;
float PhysicsWrapper::RigidBodyCount = 0;

// fix for release
hkTestEntry* hkUnitTestDatabase;
hkBool HK_CALL hkTestReport(hkBool32 cond, const char* desc, const char* file, int line)
{
	printf(desc);
	return false;
}

void HK_CALL PhysicsWrapper::errorReport(const char* msg, void* userArgGivenToInit)
{
	printf(msg);
}

void PhysicsWrapper::Init()
{
	hkMemoryRouter* memoryRouter = hkMemoryInitUtil::initDefault();
	hkBaseSystem::init( memoryRouter, errorReport );
	setupComplete = false;
		//
		// Initialize the multi-threading classes, hkJobQueue, and hkJobThreadPool
		//

		// They can be used for all Havok multithreading tasks. In this exmaple we only show how to use
		// them for physics, but you can reference other multithreading demos in the demo framework
		// to see how to multithread other products. The model of usage is the same as for physics.
		// The hkThreadpool has a specified number of threads that can run Havok jobs.  These can work
		// alongside the main thread to perform any Havok multi-threadable computations.
		// The model for running Havok tasks in Spus and in auxilary threads is identical.  It is encapsulated in the
		// class hkJobThreadPool.  On PLAYSTATION(R)3 we initialize the SPU version of this class, which is simply a SPURS taskset.
		// On other multi-threaded platforms we initialize the CPU version of this class, hkCpuJobThreadPool, which creates a pool of threads
		// that run in exactly the same way.  On the PLAYSTATION(R)3 we could also create a hkCpuJobThreadPool.  However, it is only
		// necessary (and advisable) to use one Havok PPU thread for maximum efficiency. In this case we simply use this main thread
		// for this purpose, and so do not create a hkCpuJobThreadPool.
//		hkJobThreadPool* threadPool;

		// We can cap the number of threads used - here we use the maximum for whatever multithreaded platform we are running on. This variable is
		// set in the following code sections.
		int totalNumThreadsUsed;

		// Get the number of physical threads available on the system
		hkHardwareInfo hwInfo;
		hkGetHardwareInfo(hwInfo);
		totalNumThreadsUsed = hwInfo.m_numThreads;

		// We use one less than this for our thread pool, because we must also use this thread for our simulation
		hkCpuJobThreadPoolCinfo threadPoolCinfo;
		threadPoolCinfo.m_numThreads = totalNumThreadsUsed - 1;

		// This line enables timers collection, by allocating 200 Kb per thread.  If you leave this at its default (0),
		// timer collection will not be enabled.
		threadPoolCinfo.m_timerBufferPerThreadAllocation = 200000;
		threadPool = new hkCpuJobThreadPool( threadPoolCinfo );

		// We also need to create a Job queue. This job queue will be used by all Havok modules to run multithreaded work.
		// Here we only use it for physics.
		hkJobQueueCinfo info;
		info.m_jobQueueHwSetup.m_numCpuThreads = totalNumThreadsUsed;
		jobQueue = new hkJobQueue(info);

		//
		// Enable monitors for this thread.
		//

		// Monitors have been enabled for thread pool threads already (see above comment).
		hkMonitorStream::getInstance().resize(200000);

		//
		// <PHYSICS-ONLY>: Create the physics world.
		// At this point you would initialize any other Havok modules you are using.
		//
		
		{
			// The world cinfo contains global simulation parameters, including gravity, solver settings etc.
			hkpWorldCinfo worldInfo;

			// Set the simulation type of the world to multi-threaded.
			worldInfo.m_simulationType = hkpWorldCinfo::SIMULATION_TYPE_MULTITHREADED;

			// Flag objects that fall "out of the world" to be automatically removed - just necessary for this physics scene
			worldInfo.m_broadPhaseBorderBehaviour = hkpWorldCinfo::BROADPHASE_BORDER_REMOVE_ENTITY;

			physicsWorld = new hkpWorld(worldInfo);

			// Disable deactivation, so that you can view timers in the VDB. This should not be done in your game.
			physicsWorld->m_wantDeactivation = false;


			// When the simulation type is SIMULATION_TYPE_MULTITHREADED, in the debug build, the sdk performs checks
			// to make sure only one thread is modifying the world at once to prevent multithreaded bugs. Each thread
			// must call markForRead / markForWrite before it modifies the world to enable these checks.
			physicsWorld->markForWrite();
			physicsWorld->setGravity(hkVector4(0.0f, -9.8f, 0.0f));


			// Register all collision agents, even though only box - box will be used in this particular example.
			// It's important to register collision agents before adding any entities to the world.
			hkpAgentRegisterUtil::registerAllAgents( physicsWorld->getCollisionDispatcher() );

			// We need to register all modules we will be running multi-threaded with the job queue
			physicsWorld->registerWithJobQueue( jobQueue );
		}
}

void PhysicsWrapper::FinishInit(){
	//
	// Initialize the VDB
	//
	hkArray<hkProcessContext*> contexts;


	// <PHYSICS-ONLY>: Register physics specific visual debugger processes
	// By default the VDB will show debug points and lines, however some products such as physics and cloth have additional viewers
	// that can show geometries etc and can be enabled and disabled by the VDB app.

	{
		// The visual debugger so we can connect remotely to the simulation
		// The context must exist beyond the use of the VDB instance, and you can make
		// whatever contexts you like for your own viewer types.
		context = new hkpPhysicsContext();
		hkpPhysicsContext::registerAllPhysicsProcesses(); // all the physics viewers
		context->addWorld(physicsWorld); // add the physics world so the viewers can see it
		contexts.pushBack(context);

		// Now we have finished modifying the world, release our write marker.
		physicsWorld->unmarkForWrite();
	}

	vdb = new hkVisualDebugger(contexts);
	vdb->serve();		

	stopWatch.start();
	lastTime = stopWatch.getElapsedSeconds();

	timestep = 1.f / 60.f;
	setupComplete = true;
}

void PhysicsWrapper::Update( float dt )
{
	// Pause until the actual time has passed
/*	if(stopWatch.getElapsedSeconds() < lastTime + timestep)
		return;
*/
	physicsWorld->stepMultithreaded( jobQueue, threadPool, timestep );

	// Step the visual debugger. We first synchronize the timer data
	context->syncTimers( threadPool );
	vdb->step();

	// Clear accumulated timer data in this thread and all slave threads
	hkMonitorStream::getInstance().reset();
	threadPool->clearTimerData();

	//lastTime += timestep;
}

void PhysicsWrapper::CleanUp()
{
	//
	// Clean up physics and graphics
	//
	if(stopWatch.isRunning())
		stopWatch.stop();

	// <PHYSICS-ONLY>: cleanup physics
	{
		physicsWorld->markForWrite();
		physicsWorld->removeAll();
		physicsWorld->removeReference();
	}
	vdb->removeReference();

	// Contexts are not reference counted at the base class level by the VDB as
	// they are just interfaces really. So only delete the context after you have
	// finished using the VDB.
	context->removeReference();

	delete jobQueue;

	//
	// Clean up the thread pool
	//

	threadPool->removeReference();

	hkBaseSystem::quit();
	hkMemoryInitUtil::quit();
}

void PhysicsWrapper::AddSurfels(std::vector<D3DXVECTOR3> points, std::vector<ProjectStructs::SURFEL*> surfels, ProjectStructs::MATERIAL_PROPERTIES materialProperties, D3DXVECTOR3 pos){
 AddSurfels(points, surfels, materialProperties, pos, true);
}

void PhysicsWrapper::AddSurfels(std::vector<D3DXVECTOR3> points, std::vector<ProjectStructs::SURFEL*> surfels, ProjectStructs::MATERIAL_PROPERTIES materialProperties, D3DXVECTOR3 pos, bool lockWorld){

	if(lockWorld)
		LockWorld();

	hkpRigidBodyCinfo info;
	hkpMassProperties massProperties;

	hkGeometry geo;

	D3DXVECTOR3* vertexPositions = NULL;

	vertexPositions = new D3DXVECTOR3[6];

	for(int i = 0; i<surfels.size(); i+=4){
		int index = 0;
		
	/*	if(!MathHelper::IsFiniteNumber(points[i].x) || !MathHelper::IsFiniteNumber(points[i].y) || !MathHelper::IsFiniteNumber(points[i].y) ||
			!MathHelper::IsFiniteNumber(points[i+1].x) || !MathHelper::IsFiniteNumber(points[i+1].y) || !MathHelper::IsFiniteNumber(points[i+1].y) ||
			!MathHelper::IsFiniteNumber(points[i+2].x) || !MathHelper::IsFiniteNumber(points[i+2].y) || !MathHelper::IsFiniteNumber(points[i+2].y) ||
			!MathHelper::IsFiniteNumber(points[i+3].x) || !MathHelper::IsFiniteNumber(points[i+3].y) || !MathHelper::IsFiniteNumber(points[i+3].y)){
				continue;
		}
*/
		vertexPositions[index++] = D3DXVECTOR3(points[i + 0] + pos - (points[i + 0] - surfels[i]->vertex->pos) * 0.5f);
		vertexPositions[index++] = D3DXVECTOR3(points[i + 1] + pos - (points[i + 1] - surfels[i]->vertex->pos) * 0.5f);
		vertexPositions[index++] = D3DXVECTOR3(points[i + 2] + pos - (points[i + 2] - surfels[i]->vertex->pos) * 0.5f);
		
		vertexPositions[index++] = D3DXVECTOR3(points[i + 1] + pos - (points[i + 1] - surfels[i]->vertex->pos) * 0.5f);
		vertexPositions[index++] = D3DXVECTOR3(points[i + 3] + pos - (points[i + 3] - surfels[i]->vertex->pos) * 0.5f);
		vertexPositions[index++] = D3DXVECTOR3(points[i + 2] + pos - (points[i + 2] - surfels[i]->vertex->pos) * 0.5f);

		hkStridedVertices stridedVerts;
		stridedVerts.m_numVertices = index;
		stridedVerts.m_striding = sizeof(D3DXVECTOR3);
		stridedVerts.m_vertices = &(vertexPositions[0].x);

		hkGeometry geom;
		hkArray<hkVector4> planeEquations;

		hkGeometryUtility::createConvexGeometry( stridedVerts, geom, planeEquations);

		if(geom.m_vertices.getSize() == 0)
			continue;

		stridedVerts.m_numVertices = geom.m_vertices.getSize();
		stridedVerts.m_striding = sizeof(hkVector4);
		stridedVerts.m_vertices = &(geom.m_vertices[0](0));
		hkReal convexRadius = 0.1f;
		hkGeometryUtility::expandPlanes(planeEquations, convexRadius);
		hkpConvexVerticesShape* shape = new hkpConvexVerticesShape(stridedVerts, planeEquations, convexRadius);

		info.m_position = hkVector4(0.0f, 0.0f, 0.0f);

		info.m_motionType = hkpMotion::MOTION_FIXED;

		info.m_shape = shape;

		info.m_qualityType = HK_COLLIDABLE_QUALITY_FIXED;
		info.m_collisionResponse = hkpMaterial::RESPONSE_SIMPLE_CONTACT;

		hkpRigidBody* rigid = new hkpRigidBody( info );

		physicsWorld->addEntity( rigid );
		
		if(materialProperties.deformable){
			rigid->addContactListener(new ContactListener(surfels[i]));
		}

		rigid->removeReference();
		shape->removeReference();
			
		//surfels[i]->rigidBody = rigid;
		RigidBodyCount++;
	}

	delete vertexPositions;
	
	if(lockWorld)
		UnLockWorld();
}
/*
void PhysicsWrapper::AddSurface(Surface* surface)
{
	LockWorld();

	hkpRigidBodyCinfo info;
	hkpMassProperties massProperties;

	hkGeometry geo;

	// read from the surfel vertex buffer
	ProjectStructs::SOLID_VERTEX* vertices = 0;

	HR(surface->GetSurfelReadableBuffer()->Map(D3D10_MAP_READ, 0, reinterpret_cast< void** >(&vertices)));
	surface->GetSurfelReadableBuffer()->Unmap();
	
	D3DXVECTOR4* vertexPositions = NULL;

	for(int surfelIndex = 0; surfelIndex < surface->GetSurfaceSurfelCount(); surfelIndex++){
		vertexPositions = new D3DXVECTOR4[6];
		int index = 0;
		for(int i = 0; i<6;i+=3){
			D3DXVec4Transform(&vertexPositions[index++], &D3DXVECTOR4(vertices[surfelIndex*6 + i].pos , 1.0f), &surface->GetWorld());
			D3DXVec4Transform(&vertexPositions[index++], &D3DXVECTOR4(vertices[surfelIndex*6 + i+1].pos , 1.0f), &surface->GetWorld());
			D3DXVec4Transform(&vertexPositions[index++], &D3DXVECTOR4(vertices[surfelIndex*6 + i+2].pos , 1.0f), &surface->GetWorld());
		}

		hkStridedVertices stridedVerts;
		stridedVerts.m_numVertices = index;
		stridedVerts.m_striding = sizeof(D3DXVECTOR4);
		stridedVerts.m_vertices = &(vertexPositions[0].x);

		hkGeometry geom;
		hkArray<hkVector4> planeEquations;

		hkGeometryUtility::createConvexGeometry( stridedVerts, geom, planeEquations);

		if(geom.m_vertices.getSize() == 0)
			continue;

		stridedVerts.m_numVertices = geom.m_vertices.getSize();
		stridedVerts.m_striding = sizeof(hkVector4);
		stridedVerts.m_vertices = &(geom.m_vertices[0](0));
		hkReal convexRadius = 0.05f;
		hkGeometryUtility::expandPlanes(planeEquations, convexRadius);
		hkpConvexVerticesShape* shape = new hkpConvexVerticesShape(stridedVerts, planeEquations, convexRadius);

		info.m_position = hkVector4(0.0f, 0.0f, 0.0f);
		
		info.m_motionType = hkpMotion::MOTION_FIXED;
		
		info.m_shape = shape;

		info.m_qualityType = HK_COLLIDABLE_QUALITY_FIXED;
		info.m_collisionResponse = hkpMaterial::RESPONSE_SIMPLE_CONTACT;
		
		hkpRigidBody* rigid = new hkpRigidBody( info );

		physicsWorld->addEntity( rigid );
		ContactListener *cl = new ContactListener(surfel);
		rigid->addContactListener(cl);
		
		rigid->removeReference();

		info.m_shape->removeReference();
		surface->AddRigidBody(rigid);
		surface->AddContactListener(cl);

		delete vertexPositions;
	}
	UnLockWorld();
}
*/
void PhysicsWrapper::AddProjectile(ProjectStructs::PROJECTILE *projectile)
{
	LockWorld();

	hkpRigidBody* rb = SetupSphericalRigidBody(1.0f, 100.0f * 5.0f, projectile->position, projectile->velocity, false, projectile);	

	UnLockWorld();
	projectile->rigidBody = rb;
}

void PhysicsWrapper::AddWreckingBall(WreckingBall *wreckingball)
{
	LockWorld();

	hkpRigidBodyCinfo chainBodyInfo, wreckingBallBodyInfo;
	chainBodyInfo.m_shape = new hkpSphereShape(wreckingball->GetChain()[0]->GetRadius());
	hkpInertiaTensorComputer::setShapeVolumeMassProperties(chainBodyInfo.m_shape, 3000.0f* 7.5f, chainBodyInfo);
	chainBodyInfo.m_mass = 3000.0f* 7.5f;

	wreckingBallBodyInfo.m_shape = new hkpSphereShape(wreckingball->GetRadius());
	hkpInertiaTensorComputer::setShapeVolumeMassProperties(wreckingBallBodyInfo.m_shape, 5500.0f* 7.5f, wreckingBallBodyInfo);
	wreckingBallBodyInfo.m_mass = 5500.0f* 7.5f;
	wreckingBallBodyInfo.m_position.set(wreckingball->GetPosition().x, wreckingball->GetPosition().y, wreckingball->GetPosition().z);
	wreckingBallBodyInfo.m_motionType = hkpMotion::MOTION_DYNAMIC;

	//
	// Construct string of independent bilateral constraints
	//
	hkpConstraintData* data;
	hkpBallAndSocketConstraintData* bsData = new hkpBallAndSocketConstraintData();
	bsData->setInBodySpace(hkVector4::getZero(), hkVector4(0.0f, -wreckingball->GetChain()[0]->GetRadius(), 0.0f));
	data = bsData;

	hkArray<hkpEntity*> entities;

	for (int b = 0; b < wreckingball->GetChain().GetCount(); b++)
	{
		chainBodyInfo.m_position.set(wreckingball->GetChain()[b]->GetPositionVector().x, wreckingball->GetChain()[b]->GetPositionVector().y, wreckingball->GetChain()[b]->GetPositionVector().z);
		chainBodyInfo.m_motionType = b ? hkpMotion::MOTION_DYNAMIC : hkpMotion::MOTION_FIXED;

		hkpRigidBody* body = new hkpRigidBody(chainBodyInfo);
		
		body->addProperty(HK_OBJECT_IS_CHAIN, hkpPropertyValue(wreckingball->GetChain()[b]));

		wreckingball->GetChain()[b]->SetRigidBody(body);
		physicsWorld->addEntity(body);

		entities.pushBack(body);
		// we know, a reference is kept by the world
		body->removeReference();
	}

	hkpRigidBody* body = new hkpRigidBody(wreckingBallBodyInfo);
	body->addProperty(HK_OBJECT_IS_WRECKINGBALL, hkpPropertyValue(wreckingball));

	wreckingball->SetRigidBody(body);
	physicsWorld->addEntity(body);
	body->removeReference();	
	
	hkpConstraintChainInstance* chainInstance = HK_NULL;

	hkpBallSocketChainData* chainData = new hkpBallSocketChainData();
	chainInstance = new hkpConstraintChainInstance( chainData );

	chainInstance->addEntity( entities[0] );

	float radius = abs(wreckingball->GetChain()[0]->GetPositionVector().y - wreckingball->GetChain()[wreckingball->GetChain().GetCount()-1]->GetPositionVector().y) / (wreckingball->GetChain().GetCount());
	for (int e = 1; e < entities.getSize(); e++)
	{
		chainData->addConstraintInfoInBodySpace(hkVector4( 0.0f, -radius, 0.0f), hkVector4( 0.0f, radius, 0.0f) );
		chainInstance->addEntity( entities[e] );
	}

	chainData->addConstraintInfoInBodySpace(hkVector4( 0.0f, -radius, 0.0f), hkVector4( 0.0f, wreckingball->GetRadius(), 0.0f) );
	chainInstance->addEntity( wreckingball->GetRigidBody());

	chainData->m_tau = 0.7f;
	chainData->removeReference();
	
	physicsWorld->addConstraint( chainInstance );
	chainInstance->removeReference();

	chainBodyInfo.m_shape->removeReference();

	UnLockWorld();
}

hkpRigidBody* PhysicsWrapper::SetupSphericalRigidBody(float radius, float mass, D3DXVECTOR3 position, D3DXVECTOR3 velocity, bool isStatic, ProjectStructs::PROJECTILE *projectile){
	hkVector4 relPos( 0.0f,radius, 0.0f);

	hkpRigidBodyCinfo info;
	hkpMassProperties massProperties;
	hkpInertiaTensorComputer::computeSphereVolumeMassProperties(radius, mass, massProperties);

	info.m_mass = massProperties.m_mass;
	info.m_centerOfMass  = massProperties.m_centerOfMass;
	info.m_friction = 0.75f;
	info.m_inertiaTensor = massProperties.m_inertiaTensor;
	info.m_shape = new hkpSphereShape( radius );
	relPos.add4(hkVector4(position.x, position.y, position.z));	
	info.m_position = relPos;
	info.m_linearVelocity = hkVector4(velocity.x, velocity.y, velocity.z);
	if(isStatic)
	{
		info.m_motionType = hkpMotion::MOTION_FIXED;
		info.m_qualityType = HK_COLLIDABLE_QUALITY_FIXED;
	}
	else{
		info.m_motionType = hkpMotion::MOTION_SPHERE_INERTIA;
		info.m_qualityType = HK_COLLIDABLE_QUALITY_MOVING;
	}

	hkpRigidBody* rb = new hkpRigidBody( info );
	rb->addProperty(HK_OBJECT_IS_PROJECTILE, hkpPropertyValue(projectile));
	
	physicsWorld->addEntity( rb );
	
	rb->removeReference();
	info.m_shape->removeReference();

	return rb;
}
