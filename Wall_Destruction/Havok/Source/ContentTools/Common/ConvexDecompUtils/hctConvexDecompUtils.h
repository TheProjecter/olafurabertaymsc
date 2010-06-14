/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2009 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef INC_COMMON_CONVEX_DECOMP_UTILS_H
#define INC_COMMON_CONVEX_DECOMP_UTILS_H

#include <Common/Base/hkBase.h>

#include <windows.h>
#include <commctrl.h> 
#include <commdlg.h>
#include <Common/Base/Thread/Semaphore/hkSemaphore.h>
#include <Common/Base/Thread/CriticalSection/hkCriticalSection.h>
#include <Common/Base/Thread/Thread/hkThread.h>

struct hkGeometry;
class hkSceneExportMemThreadCallback;

//
// Handle progress bar updates from the convex decomposition code
//
class hctConvexDecompProgressHandler
{
public:
	static DWORD WINAPI __dummyUpdateFn( LPVOID arg ) { return 0; }

	hctConvexDecompProgressHandler()
	{
		m_jobQueueLock = new hkCriticalSection(0);
		m_interfaceLock = new hkCriticalSection(0);
		m_newJobEvent = new hkSemaphore();
		m_endThreadEvent = new hkSemaphore();
	}

	~hctConvexDecompProgressHandler()
	{
		delete m_endThreadEvent;
		delete m_newJobEvent;
		delete m_interfaceLock;
		delete m_jobQueueLock;
	}

	void setJob( const char* message, const int percentage )
	{
		m_jobQueueLock->enter();
		strcpy( m_message, message );
		m_percentage = percentage;
		m_jobQueueLock->leave();
		m_newJobEvent->release();
	}

	// A sub class should implement these functions to update the appearance of the progress bar
	virtual void processJobs() = 0;
	virtual void setupBar() = 0;
	virtual void cleanupBar( const bool result ) = 0;

	hkCriticalSection*		m_jobQueueLock;
	hkCriticalSection*		m_interfaceLock;
	hkSemaphore*			m_newJobEvent;
	hkSemaphore*			m_endThreadEvent;
	bool					m_isCancelled;
	bool					m_isCancelled_mainThread;
	char					m_message[160];
	int						m_percentage;
	mutable int				m_nJobs;
	mutable int				m_nFinished;
	mutable bool			m_wasCancelPressed;
	bool					m_isCombine;
};

//
// Convex decomposition utility
//
class hctConvexDecompUtils
{
public:
	virtual ~hctConvexDecompUtils() {}	

	// Callback to set the memory router created in a new thread back in the modeler dll
	virtual void setThreadCallback( hkSceneExportMemThreadCallback* callback ) = 0;

	enum DllError
	{
		DLL_INTERNAL_ERROR = 0,  // unable to query the DLL at all
		DLL_NO_ERROR = 1,        // DLL did load ok
		DLL_INIT_ERROR = 2,      // DLL is not initialized or failed to init internal structures
		DLL_KEYCODE_EXPIRED = 4, // the DLL keycode has expired, the DLL functions will not work
		DLL_KEYCODE_ILLEGAL = 8, // the DLL keycode is malformed
		DLL_KEYCODE_WARNING = 16 // the DLL keycode is ok but will expire in 10 days
	};

	struct GuardGenConfig
	{
		enum eMethod
		{
			NONE = 0,			/// Do not generate guards.
			SPHERE = 1,			/// Generate sphere to prevent merging
			EDGE = 2,			/// Generate edge to prevent merging
		};

		GuardGenConfig() : m_method(GuardGenConfig::NONE), m_offset(0), m_edgeSamplingSpacing(1), m_maxGlobalGuardsOctreeDepth(0) {}

		eMethod m_method;					// method of guard generation
		hkReal m_offset;					// offset to the surface
		hkReal m_edgeSamplingSpacing;		// spacing between edge samples
		int m_maxGlobalGuardsOctreeDepth;	// max depth allowed for global guards generation.
	};

	struct Input
	{
		hkReal m_accuracy;
		hkBool m_useExisting;
		hkReal m_scale;
		hkBool m_multithreaded;
		hkBool m_parts;
		hkBool m_reduce;
		hkBool m_useMaterialBoundaries;
		int m_maxVertices;
		int m_reduceOverlapsPasses;
		hkArray<hkVector4> m_guards;
		GuardGenConfig m_guardsGenerator;
		hctConvexDecompProgressHandler* m_progressJobHandler;

		Input()
		{
			m_accuracy = 0;
			m_useExisting= false;
			m_scale = 1;
			m_maxVertices = (int)((~(unsigned)0)>>1);
			m_reduceOverlapsPasses = -1;
			m_multithreaded = false;
			m_parts = true;
			m_reduce = true;
			m_useMaterialBoundaries = false;
			m_progressJobHandler = NULL;
		}

		void setupDecomposeInput( const BOOL detachParts, const BOOL reduceOverlapPasses, const double accuracy, const INT octreeDepth )
		{
			m_multithreaded = TRUE;
			m_guards.reserve(0);
			m_parts = ( detachParts != FALSE );
			m_useExisting = false;
			m_reduce = true;
			m_useMaterialBoundaries = false;

			if ( 0 == reduceOverlapPasses )
			{
				m_reduceOverlapsPasses = 0;
			}
			else
			{
				m_reduceOverlapsPasses = reduceOverlapPasses;
			}

			if ( 0 != accuracy )
			{
				m_accuracy = 0.0001f;
				m_guardsGenerator.m_method = hctConvexDecompUtils::GuardGenConfig::EDGE;
				m_guardsGenerator.m_offset = (hkReal)accuracy;
				m_guardsGenerator.m_edgeSamplingSpacing = 0.1f;
			}
			else
			{
				m_accuracy = 1.0f;
				m_guardsGenerator.m_method = hctConvexDecompUtils::GuardGenConfig::NONE;
			}

			m_guardsGenerator.m_maxGlobalGuardsOctreeDepth = octreeDepth;		
		}

		void setupCombineInput( const BOOL genGuards, const double accuracy, const INT octreeDepth )
		{
			m_multithreaded = TRUE;
			m_guards.reserve(0);
			m_parts = false;
			m_useExisting = true;
			m_reduce = false;
			m_useMaterialBoundaries = false;

			if ( genGuards )
			{
				m_accuracy = 0.0001f;
				m_guardsGenerator.m_method = hctConvexDecompUtils::GuardGenConfig::EDGE;
				m_guardsGenerator.m_offset = (hkReal)accuracy;
				m_guardsGenerator.m_edgeSamplingSpacing = 0.1f;
			}
			else
			{
				m_accuracy = (hkReal)( accuracy / 100.0f );
				m_guardsGenerator.m_method = hctConvexDecompUtils::GuardGenConfig::NONE;
			}

			m_guardsGenerator.m_maxGlobalGuardsOctreeDepth = octreeDepth;
		}
	};

	virtual bool decompose( const hctConvexDecompUtils::Input& input, const hkGeometry& geometry, hkArray<hkArray<hkVector4> >& shapesOut ) = 0;
	virtual bool combine( const hctConvexDecompUtils::Input& input, const hkGeometry& geometry, hkArray< hkArray< hkVector4 > >& shapesOut, hkArray< hkArray< hkVector4 > >* shapesToCombine ) = 0;

	virtual void createConvexGeometry( hkArray< hkVector4 >& shape, hkGeometry& shapeGeometry ) = 0;
	virtual int getConvexHullPlanes( const hkGeometry& geometry ) = 0;
	virtual void mergeGeometries(const hkGeometry& input,hkGeometry& output) = 0;
	virtual hkReal normalizeGeometry( hkGeometry& geom, hkReal size, hkTransform* normalizedToWorld, hkVector4* normalizeCenter ) = 0;
	virtual void normalizeShapes( hkArray< hkArray< hkVector4 > >& shapes, hkReal scale, hkVector4& normalizeCenter ) = 0;
};

typedef class hctBaseDll* (HK_CALL *hctGetBaseDllInterfaceFunc)(); // getBaseDllInterface()
typedef hctConvexDecompUtils* (HK_CALL *hctGetConvexDecompUtilInterfaceFunc)();  // getConvexDecompUtilInterface()
typedef hctConvexDecompUtils::DllError (HK_CALL *hctGetConvexDecompDllErrorFunc)(); // getConvexDecompDllError()

#endif // INC_COMMON_CONVEX_DECOMP_UTILS_H


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
