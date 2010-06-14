/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2009 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */


hkpCharacterProxyJob::hkpCharacterProxyJob( JobSubType subType, hkUint16 size )
: hkJob( HK_JOB_TYPE_CHARACTER_PROXY, subType, size, HK_JOB_SPU_TYPE_DISABLED ),
m_semaphore(HK_NULL), m_jobDoneFlag(HK_NULL)
{
	m_sharedJobHeaderOnPpu = HK_NULL;
}

inline void hkpCharacterProxyJob::atomicIncrementAndReleaseSemaphore() const
{
	if (m_jobDoneFlag)
	{
		hkCriticalSection::atomicExchangeAdd(m_jobDoneFlag, 1);
	}

	if (m_semaphore)
	{
		hkSemaphoreBusyWait::release(m_semaphore);
	}
}

// =====================================================================================================================
// CHARACTER PROXY 
// =====================================================================================================================

hkpCharacterProxyIntegrateJob::hkpCharacterProxyIntegrateJob(hkpCharacterProxyJobHeader* jobHeader,
															 hkSemaphoreBusyWait* semaphore,
															 const hkpCharacterProxyIntegrateCommand* commandArray,
															 hkpCharacterProxy** characterArrayPtr,
															 int numCommands,
															 const hkStepInfo* stepInfo,
															 const hkVector4& worldGravity,
															 const int maxInteractions,
															 const int numCommandsPerTask) 
:	hkpCharacterProxyJob(CHARACTER_PROXY_JOB_INTEGRATE, sizeof(hkpCharacterProxyIntegrateJob)),
	m_stepInfo(stepInfo),
	m_worldGravity(worldGravity),
	m_maxInteractions(maxInteractions),
	m_numCommandsPerTask(numCommandsPerTask)
{
	HK_ASSERT2( 0xaf136151, jobHeader && ((hkUlong)jobHeader & 0xf) == 0,
						"jobHeader has to be set and be 16 byte aligned." );

	HK_ASSERT2( 0xaf1647f5, numCommands > 0,
						"numCommands has to be > 0." );

	HK_ASSERT2( 0xaf1647e0, commandArray && ((hkUlong)commandArray & 0xf) == 0,
						"commandArray has to be set and 16 byte aligned." );

	HK_ASSERT2( 0xaf1647b4, numCommandsPerTask > 0 && numCommandsPerTask <= MAXIMUM_NUMBER_OF_COMMANDS_PER_TASK,
						"numCommandsPerTask has to be > 0 and <= " << MAXIMUM_NUMBER_OF_COMMANDS_PER_TASK << "." );	

#if defined HK_DEBUG
#define MAX_MAX hkpCharacterProxyIntegrateJob::MAX_MAXIMUM_INTERACTIONS_PER_CHARACTER

	HK_ASSERT2( 0xae1e4cb6, maxInteractions > 0 && maxInteractions <= MAX_MAX, 
						"maxInteractions has to be > 0 and <= " << MAX_MAX << "." );

#undef MAX_MAX
#endif

#if defined(HK_DEBUG)
	// check memory allocated on commands
	{
		for (int i = 0; i < numCommands; i++)
		{
			hkpCharacterProxyIntegrateCommand* command = const_cast<hkpCharacterProxyIntegrateCommand*>( &commandArray[i] );

			HK_ASSERT2( 0xaf2647fe, command->m_objectInteraction && ((hkUlong)command->m_objectInteraction & 0xf) == 0,
						"hkpCharacterProxyCommand::m_objectInteraction has to be set and 16 byte aligned." );
		}
	}

	// This is a very simple and crude attempt to try to catch a common mistake where the user might
	// forget to actually advance the pointer to the results. Doing so could cause incorrect query results.
	// This check though will NOT catch trickier situations like e.g. partially overlapping results.
	if ( numCommands > 1 )
	{
		HK_ASSERT2(0xaf253413, commandArray[0].m_objectInteraction != commandArray[1].m_objectInteraction, 
						"You are not allowed to re-use the same results buffer for two different query commands.");
	}
#endif

	m_sharedJobHeaderOnPpu	= jobHeader;
	m_semaphore				= semaphore;
	m_commandArray			= commandArray;
	m_characterArrayPtr		= characterArrayPtr;
	m_numCommands			= numCommands;

	// precalculate the total number of jobs that will be spawned from the original job (incl. the original)
	m_sharedJobHeaderOnPpu->m_openJobs = ((numCommands-1)/numCommandsPerTask) + 1;
}

hkJobQueue::JobPopFuncResult hkpCharacterProxyIntegrateJob::popJobTask( hkpCharacterProxyIntegrateJob& out )
{
	//
	// split off a fully filled child job if there are more tasks left than one job can handle
	//
	if ( m_numCommands > m_numCommandsPerTask )
	{
		out.m_numCommands	 = m_numCommandsPerTask;
		m_numCommands		-= m_numCommandsPerTask;
		m_commandArray		 = hkAddByteOffsetConst(m_commandArray, m_numCommandsPerTask * sizeof(hkpCharacterProxyIntegrateCommand));

		return hkJobQueue::DO_NOT_POP_QUEUE_ENTRY;
	}

	return hkJobQueue::POP_QUEUE_ENTRY;
}

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
