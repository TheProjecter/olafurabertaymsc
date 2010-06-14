/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2009 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_CHARACTER_PROXY_JOB_UTIL_H
#define HK_CHARACTER_PROXY_JOB_UTIL_H

#include<Common/Base/Thread/Semaphore/hkSemaphoreBusyWait.h>

class hkpCharacterProxyJobUtil
{
public:

		/// Data to be supplied to the hkpCharacterProxyJobUtil
	struct JobData
	{
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_ARRAY, JobData);

			///	Multithreading data
		class hkJobQueue*					m_jobQueue;

			///	Multithreading data
		int									m_numJobs;

			/// The array of characters to be simulated with multithreading
		hkArray<class hkpCharacterProxy*>*	m_characters;

			/// This is the maximum number of character-rigid body interactions per-character that will be executed per 
			/// simulation step. The number of interactions must be limited due to the fixed size of the command buffer 
			/// that contains the per-character command information: see hkpCharacterProxyIntegrateCommand for more.
		hkUint8								m_maxInteractions;

		const class hkStepInfo*				m_stepInfo;

		class hkpWorld*						m_world;

	};

	//
	// Interface for synchronous multithreading.
	//
		
		/// Build and perform integrate jobs. After jobs have been processed, update the broad phase aabbs from the 
		/// characters and apply impulses to colliding bodies (if any) stored in the command.
		/// Stack allocations are used for temporary buffer.	
	static void HK_CALL simulateCharactersSynchronously( class hkJobThreadPool* threadPool, const struct JobData& mtData );

	//
	// Interface for asynchronous multithreading.
	//
		/// Returns the size of the buffer (in bytes) required to hold the information for asynchronous casting.
		/// \param numJobs the number of jobs intended be used.
	static int HK_CALL getBufferSize( const JobData& mtData );

		/// Build and add a collection of integrate jobs to a job queue.
		/// \param semaphore the semaphore to attach to each job.
		/// \param buffer a buffer allocated by the caller to hold information for the casts.
		/// \return the number of jobs added (does not account for jobs splitting when there are too many commands
		/// for a job).
	static void HK_CALL buildAndAddJobs( const JobData& mtData, hkSemaphoreBusyWait* semaphore, void* buffer );

		/// Update all characters using the results in the buffer.
		/// \param buffer the buffer previously passed to buildAndAddJobs.
	static void HK_CALL handleResults( const JobData& mtData , void* buffer );
	
};

#endif // HK_CHARACTER_PROXY_JOB_UTIL_H

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
