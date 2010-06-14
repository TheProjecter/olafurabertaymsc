/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2009 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef INC_COMMON_CONVEX_DECOMP_UTILS_IMPL_H
#define INC_COMMON_CONVEX_DECOMP_UTILS_IMPL_H

#include <ContentTools/Common/ConvexDecompUtils/hctConvexDecompUtils.h>
#include <Common/ConvexDecomposition/hkgpConvexDecomposition.h>

//
// Progress bar, implements interface in hkgpConvexDecomposition
//
struct hctConvexDecompProgress : public hkgpConvexDecomposition::IProgress
{
public:
	hctConvexDecompProgress(){ m_progressJobHandler = 0; }
	~hctConvexDecompProgress(){}

	// These functions are called from the convex decomposition code, and add jobs to update the progress bar.
	bool startJob ( int index, int totalJobs ) const;
	void endJob() const;
	bool didUserCancel() const;
	bool onDecomposePlanes( int currentPlane, int totalPlanes ) const;
	bool onReduce( int currentHullCount, int totalHulls ) const;
	bool onDecimate( int currentHullCount, int objectiveHullsCount ) const;

	mutable hctConvexDecompProgressHandler*		m_progressJobHandler;
	mutable bool								m_areSeparateDecompositions;
	int											m_nOrigHulls;
};

//
// Convex decomposition utility implementation, calls hkgpConvexDecomposition functionality
//
class hctConvexDecompUtilsImpl : public hctConvexDecompUtils
{
public:

	hctConvexDecompUtilsImpl();
	virtual ~hctConvexDecompUtilsImpl();

	virtual void setThreadCallback( hkSceneExportMemThreadCallback* callback ){ m_threadCallback = callback; }

	virtual bool decompose( const hctConvexDecompUtils::Input& input, const hkGeometry& geometry, hkArray<hkArray<hkVector4> >& shapesOut );
	virtual bool combine( const hctConvexDecompUtils::Input& input, const hkGeometry& geometry, hkArray< hkArray< hkVector4 > >& shapesOut, hkArray< hkArray< hkVector4 > >* shapesToCombine );

	virtual void createConvexGeometry( hkArray< hkVector4 >& shape, hkGeometry& shapeGeometry );
	virtual int getConvexHullPlanes( const hkGeometry& geometry );
	virtual void mergeGeometries(const hkGeometry& input,hkGeometry& output);
	virtual hkReal normalizeGeometry( hkGeometry& geom, hkReal size, hkTransform* normalizedToWorld, hkVector4* normalizeCenter );
	virtual void normalizeShapes( hkArray< hkArray< hkVector4 > >& shapes, hkReal scale, hkVector4& normalizeCenter );

protected:
	hkSceneExportMemThreadCallback* m_threadCallback;
};

__declspec(dllexport) class hctBaseDll* HK_CALL getBaseDllInterface();
__declspec(dllexport) hctConvexDecompUtils* HK_CALL getConvexDecompUtilInterface();
__declspec(dllexport) hctConvexDecompUtils::DllError HK_CALL getConvexDecompDllError();

#endif // INC_COMMON_CONVEX_DECOMP_UTILS_IMPL_H


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
