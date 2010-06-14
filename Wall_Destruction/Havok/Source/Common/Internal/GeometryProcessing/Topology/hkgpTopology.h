/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2009 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HKGP_TOPOLOGY_H
#define HKGP_TOPOLOGY_H

#if defined(HK_DEBUG) && defined(HK_PLATFORM_WIN32)
#undef HK_FORCE_INLINE
#define HK_FORCE_INLINE inline
#pragma auto_inline(off)
#endif

/// If set, store edge uid unpack, giving a nice speed up on platform with slow bitwise operations, but uses twice as much memory.
#define HKGP_TOPOLOGY_UNPACKED_UID	0

/// Topological data structures
class hkgpTopology
{
public:
	//
	// Types
	//

	/// Uid
	/// Store a TRIANGLE / INDEX pair

	static HK_FORCE_INLINE hkUlong	uid2ulong(void* t,hkUint32 i)				{ return (*(const hkUlong*)&t)+(hkUlong)i; }
	template <typename T>
	static HK_FORCE_INLINE void		ulong2uid(hkUlong uid, T*& t,hkUint32& i)	{ i = (hkUint32)(uid&3);uid&=~3;t=*(T**)&uid; }

	#if HKGP_TOPOLOGY_UNPACKED_UID
	struct	Uid
	{
		HK_FORCE_INLINE Uid()						{}
		HK_FORCE_INLINE Uid(hkUlong pk)				{ ulong2uid(pk,m_triangle,m_index); }
		HK_FORCE_INLINE operator hkUlong() const	{ return uid2ulong(m_triangle,m_index); }
		void*		m_triangle;
		hkUint32	m_index;
	};
	
	static HK_FORCE_INLINE Uid	nullUid()										{ static const Uid uid(0); return uid; }
	static HK_FORCE_INLINE Uid	invalidUid()									{ static const Uid uid(~(hkUint32)0); return uid; }
	static HK_FORCE_INLINE Uid	uidFromUlong(hkUlong pk)						{ Uid uid; ulong2uid(pk,uid.m_triangle,uid.m_index); return uid; }
	static HK_FORCE_INLINE Uid	packUid(void* t,hkUint32 i)						{ Uid uid; uid.m_triangle = t; uid.m_index = i; return uid; }	
	template <typename T>
	static HK_FORCE_INLINE void	unpackUid(const Uid& uid, T*& t,hkUint32& i)	{ t = (T*)uid.m_triangle; i = uid.m_index; }
	#else
	typedef hkUlong		Uid;

	static HK_FORCE_INLINE Uid	nullUid()										{ return 0; }
	static HK_FORCE_INLINE Uid	invalidUid()									{ return ~(hkUint32)0; }
	static HK_FORCE_INLINE Uid	uidFromUlong(hkUlong pk)						{ return pk; }
	static HK_FORCE_INLINE Uid	packUid(void* t,hkUint32 i)						{ return uid2ulong(t,i); }
	template <typename T>
	static HK_FORCE_INLINE void	unpackUid(Uid uid, T*& t,hkUint32& i)			{ ulong2uid(uid,t,i); }
	#endif

	/// Edge
	/// Main data structure used walk and process topology see: 'hkgpTopology.svg'
	/// 'EDGE' must be your own edge implementation
	/// 'VERTEX' must be your vertex implementation, and must implement 'isOrdered' (see 'hkgpAbstractMesh' for reference implementation).	
	/// 'TRIANGLE' must be your triangle implemementation, and must implement 'link' and 'vertex' (see 'hkgpAbstractMesh' for reference implementation).
	/// 'VERTEX_MEMBER' is the type of vertex references (default: VERTEX*).
	template <typename EDGE,typename VERTEX,typename TRIANGLE,typename VERTEX_MEMBER=VERTEX*>
	struct Edge
	{
	public:
		typedef VERTEX			tVertex;
		typedef VERTEX_MEMBER	tMVertex;
		typedef TRIANGLE		tTriangle;
	protected:
		/* i+1 mod 3	*/ 
		static HK_FORCE_INLINE unsigned	ip1mod3(unsigned i);
		/* i+2 mod 3	*/ 
		static HK_FORCE_INLINE unsigned	ip2mod3(unsigned i);
	public:
		HK_FORCE_INLINE static const EDGE&	null()					{ static const EDGE e(0,0);return(e); }
	public:
		HK_FORCE_INLINE				Edge()														{}
		HK_FORCE_INLINE				Edge(tTriangle* t,unsigned i) : m_triangle(t),m_index(i)	{}
		HK_FORCE_INLINE				Edge(const Uid& id)											{ unpackUid(id,m_triangle,m_index); }
		
		/// return the next edge along current edge triangle (CW)
		HK_FORCE_INLINE	EDGE		next() const					{ return(EDGE(m_triangle,ip1mod3(m_index))); }
		
		/// return the previous edge along current edge triangle (CCW)
		HK_FORCE_INLINE	EDGE		prev() const					{ return(EDGE(m_triangle,ip2mod3(m_index))); }
		
		/// return the edge bound with this edge of any
		HK_FORCE_INLINE EDGE		link() const					{ return(EDGE(m_triangle->link(m_index))); }
		
		/// return this edge rotated ccw around start()
		HK_FORCE_INLINE EDGE		turnCcw() const					{ return(link().next()); }
		
		/// return this edge rotated cw around start()
		HK_FORCE_INLINE EDGE		turnCw() const					{ return(prev().link()); }
		
		/// next edge cw in the dual mesh
		HK_FORCE_INLINE EDGE		nextCw() const					{ return(link().prev()); }
		
		/// next edge ccw in the dual mesh
		HK_FORCE_INLINE EDGE		nextCcw() const					{ return(link().next()); }
		
		/// previous edge cw in the dual mesh							 
		HK_FORCE_INLINE EDGE		prevCw() const					{ return(prev().link()); }
		
		/// previous edge ccw in the dual mesh							 
		HK_FORCE_INLINE EDGE		prevCcw() const					{ return(next().link()); }
		
		/// return the master edge of this edge							 
		HK_FORCE_INLINE EDGE		master() const					{ if(isMaster()) return(EDGE(m_triangle,m_index)); else return(link()); }
		
		/// return the triangle bound to this edge						 
		HK_FORCE_INLINE TRIANGLE*	triangle() const				{ return(m_triangle); }
		
		/// return the edge index										 
		HK_FORCE_INLINE int			index() const					{ return((int)m_index); }
		
		/// return the uid of this edge
		HK_FORCE_INLINE Uid			uid() const						{ return packUid(m_triangle,m_index); }
		
		/// return the 'start' vertex									 
		HK_FORCE_INLINE tMVertex&	start()	const					{ return(m_triangle->vertex(m_index)); }
		
		/// return the 'end' vertex										 
		HK_FORCE_INLINE tMVertex&	end() const						{ return(m_triangle->vertex(ip1mod3(m_index))); }
		
		/// return the apex of this edge									 
		HK_FORCE_INLINE tMVertex&	apex() const					{ return(m_triangle->vertex(ip2mod3(m_index))); }
		
		/// return itself												 
		HK_FORCE_INLINE EDGE		self() const					{ return(EDGE(m_triangle,m_index)); }
		
		/// check if this edge is valid									 
		HK_FORCE_INLINE bool		isValid() const					{ return(triangle()!=0); }
		
		/// check if this edge is master									 
		HK_FORCE_INLINE bool		isMaster() const				{ return(tVertex::isOrdered(start(),end())||isNaked()); }
		
		/// check if this edge is not bound						 
		HK_FORCE_INLINE bool		isNaked() const					{ return(!link().isValid()); }
		
		/// check if this edge is bound							 
		HK_FORCE_INLINE bool		isBound() const				{ return(!isNaked()); }
		
		/// check if this edge can be bound to another					 
		HK_FORCE_INLINE bool		canBind(const Edge& edge) const	{ return((!edge.isValid())||((start()==edge.end())&&(end()==edge.start()))); }
		
		/// try to bind this edge										 
		HK_FORCE_INLINE bool		tryBind(const Edge& edge) const { const bool cb(canBind(edge));if(cb) bind(edge);return(cb); }
		
		/// bind this edge with another									 
		HK_FORCE_INLINE void		bind(const Edge& edge) const;
		
		/// bind next().link() with prev().link()						 
		HK_FORCE_INLINE void		bindSides() const;
		
		/// unbind this edge												 
		HK_FORCE_INLINE void		unbind() const;
		
		/// flip this edge												 
		HK_FORCE_INLINE EDGE		flip() const;
		
		/// apply the functor 'fnc' to this edge fan						 
		/// FNC must implement 'bool operator()(Edge&)' , if true is returned, the application is stopped.
		template <typename FNC>
		HK_FORCE_INLINE void		applyFan(FNC& fnc) const;
		
		HK_FORCE_INLINE bool		operator==(const EDGE& e) const	{ return(uid()==e.uid()); }
		HK_FORCE_INLINE bool		operator!=(const EDGE& e) const	{ return(uid()!=e.uid()); }
	protected:
		tTriangle*				m_triangle;		///
		hkUint32				m_index;		///
	};
};

#define TOPOLOGY_HDR	template <typename EDGE,typename VERTEX,typename TRIANGLE,typename VERTEX_MEMBER>
#define TOPOLOGY_TYP	hkgpTopology::Edge<EDGE,VERTEX,TRIANGLE,VERTEX_MEMBER>

#include <Common/Internal/GeometryProcessing/Topology/hkgpTopology.inl>

#undef TOPOLOGY_TYP
#undef TOPOLOGY_HDR

#endif // HKGP_TOPOLOGY_H


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
