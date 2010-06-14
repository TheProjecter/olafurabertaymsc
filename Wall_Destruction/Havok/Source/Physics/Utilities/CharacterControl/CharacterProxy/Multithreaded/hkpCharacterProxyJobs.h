/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2009 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_CHARACTER_PROXY_JOBS_H
#define HK_CHARACTER_PROXY_JOBS_H

#include <Physics/Utilities/CharacterControl/CharacterProxy/hkpCharacterProxyListener.h>
#include <Common/Base/Thread/JobQueue/hkJobQueue.h>

class hkStepInfo;

	/// Each character proxy job needs an hkpCharacterProxyJobHeader.
	/// Usually this is passed in through the job's constructor.
	/// PlayStation(R)3 note: This job has only been implemented on the PPU
struct hkpCharacterProxyJobHeader
{
public:
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_UTILITIES, hkpCharacterProxyJobHeader );

	// This member can only be accessed if the job queue is locked once its governing job has been added to the job queue.	
	HK_ALIGN16( mutable int m_openJobs );
};


//
// The base class for all character proxy jobs
// Important: the 16bit m_jobType HAS to be the first member of this class and it HAS to be 16byte aligned!
// See hkJob for more details.
//
struct hkpCharacterProxyJob : public hkJob
{
public:
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_UTILITIES, hkpCharacterProxyJob );

	enum JobSubType
	{
		CHARACTER_PROXY_JOB_INTEGRATE,
		CHARACTER_PROXY_JOB_END
	};

	void atomicIncrementAndReleaseSemaphore() const;

protected:

	HK_FORCE_INLINE hkpCharacterProxyJob( JobSubType subType, hkUint16 size );

public:

		// This semaphore is released once the original job (and all its spawned children) has finished.
	hkSemaphoreBusyWait*		m_semaphore;

		// This header must be set for all jobs that potentially spawn additional jobs or that have been spawned by others
	hkpCharacterProxyJobHeader*	m_sharedJobHeaderOnPpu;

		// The variable at this location will be incremented (atomically) when the job is done.
	hkUint32*					m_jobDoneFlag;

};

// =====================================================================================================================
// CHARACTER PROXY INTEGRATE
// =====================================================================================================================

	/// This is a helper struct based on hkpCharacterObjectInteractionResult which contains 
	/// the impulse values that will later be applied to the colliding rigid body (m_collidingBody)
struct hkpCharacterProxyInteractionResults : hkpCharacterObjectInteractionResult
{
		/// The body to which we will later apply the impulse values from the base class
	hkpRigidBody* m_collidingBody;
};


	/// hkpCharacterProxyCommand stores the character interactions which should be applied after the multithread phase.
HK_CLASSALIGN16(struct) hkpCharacterProxyIntegrateCommand
{
public:

	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_CHARACTER, hkpCharacterProxyIntegrateCommand );
	
public:

	// ===================================================================
	// Output
	// ===================================================================

		/// This is a pointer to the interaction results associated with the character-rigid_body collision.
		/// Each interaction result has a pointer to the colliding dynamic rigid body as well as the respective impulse
		/// to be applied.
	hkpCharacterProxyInteractionResults*	m_objectInteraction;

};

	/// An hkpCharacterProxyJob will take an arbitrary number of hkpCharacterProxyCommand.
	/// This job is able to split itself into two jobs if it holds more commands than the maximum allowed
	/// number that can be executed in one go.
struct hkpCharacterProxyIntegrateJob : public hkpCharacterProxyJob
{
public:

	friend struct hkpCharacterProxyJobQueueUtils;

	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_COLLIDE, hkpCharacterProxyIntegrateJob );

	enum { MAX_MAXIMUM_INTERACTIONS_PER_CHARACTER = 10, MAXIMUM_NUMBER_OF_COMMANDS_PER_TASK = 128 };

public:

		/// When creating an hkpCharacterProxyJob you have to pass in an unique jobHeader as well as an array of commands.
		/// The supplied semaphore is released once all commands of this job have been completed and the job has been 
		/// removed from the job queue. The number of commands that are grouped into one task is customizable.
		/// The maximum number of dynamic rigid body interactions per character is also customizable.
	HK_FORCE_INLINE hkpCharacterProxyIntegrateJob( hkpCharacterProxyJobHeader* jobHeader, 
												hkSemaphoreBusyWait* semaphore,	
												const hkpCharacterProxyIntegrateCommand* commandArray, 
												hkpCharacterProxy** characterArrayPtr,
												int numCommands, 												
												const hkStepInfo* stepInfo, 
												const hkVector4& worldGravity, 
												const int maxInteractions, 												 
												const int numCommandsPerTask = MAXIMUM_NUMBER_OF_COMMANDS_PER_TASK );
	

		/// Only necessary on PlayStation(R)3. 
	void setRunsOnSpuOrPpu();


protected:

	HK_FORCE_INLINE hkJobQueue::JobPopFuncResult popJobTask( hkpCharacterProxyIntegrateJob& out );

public:

	const hkpCharacterProxyIntegrateCommand*	m_commandArray;			
	hkpCharacterProxy**							m_characterArrayPtr;
	int											m_numCommands;	
	const hkStepInfo*							m_stepInfo;
	const hkVector4								m_worldGravity;
	const int									m_maxInteractions;
	// maximum # of commands per task; once this limit is breached a subjob is spawned
	const int									m_numCommandsPerTask;	
};

#include <Physics/Utilities/CharacterControl/CharacterProxy/Multithreaded/hkpCharacterProxyJobs.inl>

#endif // HK_CHARACTER_PROXY_JOBS_H

/*
* Havok SDK - NO SOURCE PC DOWNLOAD, BUILD(#20091222)
* 
* Confidential Information of Havok.  (C) Copyright 1999-2009
* Telekinesys Research Limited t/a Havok. All Rights Reserved. The Havok
* Logo, and the Havok buzzsaw logo are trademarks of Havok.  Title, ownership
* rights, and intellectual property rights in the Havok software remain in
* Havok and/or its suppliers.
* 
* Use of this software for evaluation purposes is subject to and indicates
* acceptance of the End User licence Agreement for this product. A copy of
* the license is included with this software and is also available at www.havok.com/tryhavok.
* 
*/
