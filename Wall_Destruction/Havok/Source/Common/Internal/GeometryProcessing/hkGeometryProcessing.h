/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2009 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_GEOMETRY_PROCESSING_H
#define HK_GEOMETRY_PROCESSING_H

#include <Common/Base/System/Stopwatch/hkStopwatch.h>
#include <Common/Base/Types/Geometry/Aabb/hkAabb.h>
struct hkGeometry;

/// Tools for geometry processing
struct hkGeometryProcessing
{
	//
	// Types
	//	
	
	/// Some prime numbers
	enum Primes
	{
		NEXT_PRIME_AFTER_8		=	11,
		NEXT_PRIME_AFTER_16		=	17,
		NEXT_PRIME_AFTER_32		=	37,
		NEXT_PRIME_AFTER_64		=	67,
		NEXT_PRIME_AFTER_128	=	131,
		NEXT_PRIME_AFTER_256	=	257,
		NEXT_PRIME_AFTER_512	=	521,
		NEXT_PRIME_AFTER_1024	=	1031,
		NEXT_PRIME_AFTER_2048	=	2053,
		NEXT_PRIME_AFTER_4096	=	4099,
		NEXT_PRIME_AFTER_8192	=	8209
	};

	/// A simple hash table providing fast value lookup
	template <typename VALUE,typename ALLOCATOR=hkContainerHeapAllocator>
	class HashTable
	{
	public:
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_MAP,HashTable);

		/// Iterator
		struct Iterator
		{			
		private:
			friend class HashTable;
			inline					Iterator() {}
			inline					Iterator(const HashTable* hashTable) : m_table(hashTable),m_bucket(-1),m_item(-1) { next(); }
			
			const HashTable*		m_table;
			int						m_bucket;
			int						m_item;
		public:
			Iterator&				operator++()						{ next(); return *this; }
			Iterator				operator++(int)						{ const Iterator i(*this); operator++(); return i; }
			const VALUE&			operator()() const					{ return m_table->m_buckets[m_bucket][m_item]; }
									operator const Iterator*() const	{ return(m_bucket<m_table->m_buckets.getSize()?this:0); }
		private:
			HK_FORCE_INLINE void	next();
		};
		
		/// Create an empty hash table with zero buckets
		HK_FORCE_INLINE							HashTable() : m_numItems(0) {}
		
		/// Create an empty hash table with 'numBuckets' buckets
		HK_FORCE_INLINE							HashTable(int numBuckets) : m_numItems(0) { setBucketsCount(numBuckets); }
		
		/// Destructor
		HK_FORCE_INLINE							~HashTable() {}
		
		/// remove all items and set the number of buckets
		HK_FORCE_INLINE	void					setBucketsCount(int numBuckets);
		
		/// remove all items and set bucket size to zero.
		HK_FORCE_INLINE void					clear() { m_buckets.clear();m_numItems=0; }

		/// remove all items.
		HK_FORCE_INLINE void					removeAll() { for(int i=0;i<m_buckets.getSize();++i) { m_buckets[i].clear(); } m_numItems=0; }

		/// reserve items in buckets
		HK_FORCE_INLINE void					reserve(int numItems) { for(int i=0;i<m_buckets.getSize();++i) { m_buckets[i].reserve(m_buckets[i].getSize()+numItems); } }
		
		/// return the number of items stored in the hash table
		HK_FORCE_INLINE int						getSize() const { return(m_numItems); }

		/// get an iterator
		HK_FORCE_INLINE Iterator				getIterator() const { return Iterator(this); }
		
		/// lookup a bucket given an hash
		HK_FORCE_INLINE	hkArray<VALUE,ALLOCATOR>&		lookupBucket(int hash) { HK_ASSERT2(0x545415,m_buckets.getSize()>0,"Hash table not initialized"); return(m_buckets[((unsigned)hash)%m_buckets.getSize()]); }

		/// lookup a bucket given an hash
		HK_FORCE_INLINE	const hkArray<VALUE,ALLOCATOR>&	lookupBucket(int hash) const { HK_ASSERT2(0x545415,m_buckets.getSize()>0,"Hash table not initialized"); return(m_buckets[((unsigned)hash)%m_buckets.getSize()]); }
		
		/// find an item given an hash and a value
		HK_FORCE_INLINE	const VALUE*			find(int hash,const VALUE& value) const;

		/// find an item given an hash and a value
		HK_FORCE_INLINE	VALUE*					find(int hash,const VALUE& value);
		
		/// insert an item given an hash and a value
		HK_FORCE_INLINE	VALUE*					insert(int hash,const VALUE& value);
		
		/// remove an item given an hash and a value
		HK_FORCE_INLINE void					remove(int hash,const VALUE& value);
		
		/// shorthand for find
		HK_FORCE_INLINE	const VALUE*			find(const VALUE& value) const { return(find((int)value.getHash(),value)); }

		/// shorthand for find
		HK_FORCE_INLINE	VALUE*					find(const VALUE& value) { return(find((int)value.getHash(),value)); }
		
		/// shorthand for insert
		HK_FORCE_INLINE	VALUE*					insert(const VALUE& value) { return(insert((int)value.getHash(),value)); }
		
		/// shorthand for remove
		HK_FORCE_INLINE void					remove(const VALUE& value) { return(remove((int)value.getHash(),value)); }

		/// append another hashtable to this, rehash items only if m_buckets.getSize() != other.m_buckets.getSize()
		inline void								append(const HashTable& other);
	public:
		hkArray<hkArray<VALUE,ALLOCATOR>,ALLOCATOR >	m_buckets;
		int												m_numItems;
	};

	/// Pool base allocator policy
	template <typename T,const int N,typename ALLOCATOR=hkContainerHeapAllocator>
	struct PoolAllocator
	{
		struct Item
		{
			hkUint8	m_data[sizeof(T)];
		};
		struct Pool
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_GEOMETRY,Pool);
			Item	m_items[N];
			Item*	m_free;
			int		m_used;
		};
		inline					PoolAllocator() {}
		inline					~PoolAllocator();
		HK_FORCE_INLINE T*		allocate()						{ return(new(allocateItem()) T()); }
		HK_FORCE_INLINE T*		allocate(const T& value)		{ return(new(allocateItem()) T(value)); }
		inline void				release(T* item);
		HK_FORCE_INLINE bool	needRelease() const				{ return(false); }
		HK_FORCE_INLINE void	clear();
	private:
		inline Item*			allocateItem();
		inline Pool*			allocateNewPool();
		inline void				releasePool(Pool* pool) const;
		PoolAllocator(const PoolAllocator&)	{}
		void operator=(const PoolAllocator&) {}
	private:
		hkArray<Pool*,ALLOCATOR>	m_pools;
	};

	/// TransposedVectors : Use to evaluate four dot product at time.
	struct TransposedVectors
	{
		hkVector4	m_x;
		hkVector4	m_y;
		hkVector4	m_z;
	};

	/// IFunction
	template <typename IN_TYPE,typename OUT_TYPE>
	struct IFunction
	{
		typedef IN_TYPE		typeIn;
		typedef OUT_TYPE	typeOut;
		
		virtual ~IFunction() {}
		virtual typeOut		evaluate(const typeIn&) const=0;
	};

	/// ConstFunction
	template <typename FUNCTION_TYPE>
	struct ConstFunction : FUNCTION_TYPE
	{
										ConstFunction(const typename FUNCTION_TYPE::typeOut& value) : m_value(value) {}
		typename FUNCTION_TYPE::typeOut	evaluate(const typename FUNCTION_TYPE::typeIn&) const { return(m_value); }
		typename FUNCTION_TYPE::typeOut	m_value;
	};

	/// IFunction_4_1 : map hkVector4 to hkReal
	typedef IFunction<hkVector4,hkReal>		IFunction_4_1;

	/// Linear quantizer
	template <int BITS,int MARGIN,int DIMS>
	struct Quantizer
	{
		enum
		{
			DOMAIN_BITS			=	BITS,
			DOMAIN_MARGIN		=	MARGIN,
			DOMAIN_HALF_RANGE	=	1<<(BITS-1),
			DOMAIN_MIN_BOUND	=	-DOMAIN_HALF_RANGE,
			DOMAIN_MAX_BOUND	=	DOMAIN_HALF_RANGE-1,
			DOMAIN_MIN_BOUND_WM	=	DOMAIN_MIN_BOUND+MARGIN,
			DOMAIN_MAX_BOUND_WM	=	DOMAIN_MAX_BOUND-MARGIN
		};
		inline							Quantizer();
		inline void						setWorldToLocalTransform(const hkMatrix4& t);
		inline void						setLocalToWorldTransform(const hkMatrix4& t);
		HK_FORCE_INLINE	void			worldToLocal(const hkVector4& position,hkInt32* output) const;
		HK_FORCE_INLINE	void			localToWorld(const hkInt32* position,hkVector4& output) const;
		static HK_FORCE_INLINE hkInt32	round(hkReal x);
		static HK_FORCE_INLINE hkInt32	clamp(hkInt32 x);

		hkMatrix4						m_worldToLocal;
		hkMatrix4						m_localToWorld;
	};

	/// Span
	template <typename T>
	struct Span
	{
		typedef T						type;

		HK_FORCE_INLINE void			setFromVertices(const hkVector4& direction, const hkVector4* vertices,int numVertices);		
		static HK_FORCE_INLINE hkBool32	intersect(const Span& a,const Span& b);

		T								m_bounds[2];
		
	};

	typedef Span<hkReal>				RealSpan;
	typedef Span<hkSimdReal>			SimdRealSpan;

	/// IsoVertex
	struct IsoVertex
	{
		inline							IsoVertex() : m_edge(-1),m_solution(0) {}
		inline							IsoVertex(int e,hkReal s) : m_edge(e),m_solution(s) {}
		int								m_edge;
		hkReal							m_solution;

		static int HK_CALL				solveTriangle(const hkReal values[3],IsoVertex verticesOut[2],hkReal orientation = 1.0f);
	};

	//
	// Methods
	//

	/// evaluate (a-o) x (b-o)
	static HK_FORCE_INLINE hkVector4	crossOffset(const hkVector4& o, const hkVector4& a, const hkVector4& b);

	/// evaluate (a1-a0) x (b1-b0)
	static HK_FORCE_INLINE hkVector4	crossEdges(const hkVector4& a0, const hkVector4& a1, const hkVector4& b0, const hkVector4& b1);

	/// evaluate plane equation (Ax+By+Cz+D=0) given three points, A,B,C,D mapped to x,y,z,w
	static HK_FORCE_INLINE hkVector4	planeEquation(const hkVector4& a, const hkVector4& b, const hkVector4& c, bool normalized);

	/// Compare two plane for equality
	static HK_FORCE_INLINE bool			comparePlanes(const hkVector4& a, const hkVector4& b, hkReal maxDistance, hkReal minCosAngle);

	/// Decompose a vector into paralel and perpendicular components, return vector/axis dot product.
	static HK_FORCE_INLINE hkReal		decomposeVector(const hkVector4& vector, const hkVector4& axis, hkVector4& para, hkVector4& perp);

	/// Project vector on plane
	static HK_FORCE_INLINE hkVector4	projectOnPlane(const hkVector4& vector,const hkVector4& axis);

	/// Project vector on axis
	static HK_FORCE_INLINE hkVector4	projectOnAxis(const hkVector4& vector,const hkVector4& axis);

	/// calculate triangle (area * 2)^2
	static HK_FORCE_INLINE hkReal		triangleArea2Squared(const hkVector4& a, const hkVector4& b);

	/// calculate triangle area * 2
	static HK_FORCE_INLINE hkReal		triangleArea2(const hkVector4& a, const hkVector4& b, const hkVector4& c);	

	/// calculate tetrahedron (signed) volume * 6
	static HK_FORCE_INLINE hkReal		tetrahedronVolume6(const hkVector4& a, const hkVector4& b, const hkVector4& c, const hkVector4& d);

	/// apply a transform to a plane.
	static HK_FORCE_INLINE hkVector4	transformPlaneToWorld(const hkTransform& transform,const hkVector4& plane);

	/// apply the inverse of a transform to a plane.
	static HK_FORCE_INLINE hkVector4	transformPlaneToLocal(const hkTransform& transform,const hkVector4& plane);

	/// intersect 3 planes
	static bool	HK_CALL					intersectPlanes(const hkVector4& p0, const hkVector4& p1, const hkVector4& p2, hkVector4& intersection);
	
	/// intersect a line with a plane
	static bool	HK_CALL					intersectLineWithPlane(const hkVector4& x0,const hkVector4& x1,const hkVector4& plane,hkReal& toi);

	/// check intersection between two triangles
	static hkBool32 HK_CALL				checkTrianglesIntersection(const hkVector4* triangleA,const hkVector4* triangleB);

	/// distance from a point to a line
	static hkReal HK_CALL				squaredDistanceFromPointToLine(const hkVector4& p, const hkVector4& p0, const hkVector4& p1,hkReal& toi);
	
	/// distance from a point to a line
	static hkReal HK_CALL				squaredDistanceFromPointToLine(const hkVector4& p, const hkVector4& p0, const hkVector4& p1,hkVector4& normalOut,hkReal* baryOut=0);	

	/// distance from a point to a triangle
	static hkReal HK_CALL				squaredDistanceFromPointToTriangle(const hkVector4& p, const hkVector4& p0, const hkVector4& p1, const hkVector4& p2,hkVector4& normalOut,hkVector4* baryOut=0);

	/// nearest distance between two 3d lines
	static hkReal HK_CALL				nearestDistanceBetweenLines(const hkVector4& a0,const hkVector4& a1,const hkVector4& b0,const hkVector4& b1,hkReal& u,hkReal& v);
	
	/// check if a point is inside a triangle
	static bool HK_CALL					checkPointInsideTriangle(const hkVector4& p,const hkVector4& a,const hkVector4& b,const hkVector4& c,hkReal epsilon=-HK_REAL_EPSILON);

	/// build a transposed array of hkVector4
	static void HK_CALL					buildTransposedArray(const hkArray<hkVector4>& vertices, hkArray<TransposedVectors>& output);

	/// get supporting vertex (index stored in W)
	static void	HK_CALL					getSupportingVertex(const hkArray<TransposedVectors>& transposedVertices,const hkVector4& direction, hkVector4& vertexOut);
		
	/// generate random points distribution inside a geometry
	static bool HK_CALL					generateRandomDistribution(const hkGeometry& geometry,int numPoints,hkArray<hkVector4>& points,int seed=180673);

	/// generate a density controled points distribution inside a geometry, if densityFunction is not set, uniform density is assumed.
	static bool HK_CALL					generateUniformDistribution(const hkGeometry& geometry,int numPoints,hkArray<hkVector4>& points,int maxIterations=256,int seed=180673,const IFunction_4_1* densityFunction=HK_NULL);

	/// make sure the aspect ratio of an aabb is one.
	static void HK_CALL					normalizeAspectRatio(hkAabb& box);
	
	/// get one the aabb vertex
	static HK_FORCE_INLINE void			getVertex(const hkAabb& box, int index, hkVector4& vertexOut);
	
	/// create an hash code based on two pointer size values, with makeHash(0,0)==0
	template <typename A,typename B>
	static HK_FORCE_INLINE hkUlong		makeHash(A a,B b);

	/// create a symmetric hash code based on two pointer size values, with makeSymmetricHash(0,0)==0 and makeSymmetricHash(a,b)==makeSymmetricHash(b,a)
	template <typename A,typename B>
	static HK_FORCE_INLINE hkUlong		makeSymmetricHash(A a,B b);

	/// Choose N from M with N <= M , count permutations.
	static HK_FORCE_INLINE int			chooseMN_count(int m,int n);
	
	/// Choose N from M with N <= M , get a permutation.
	static inline void					chooseMN_pick(int m,int n,int i,int* set);

	/// Map unfold octahedron coordinates to unit normal.
	static void	HK_CALL					octahedronToNormal(const hkVector4& uv, hkVector4& normal);

	/// Map unit normal to unfold octahedron coordinates.
	static void	HK_CALL					normalToOctahedron(const hkVector4& normal, hkVector4& uv);

	/// Wrap unfold octahedron coordinates.
	static void	HK_CALL					wrapOctahedronCoordinates(hkVector4& uv);

	/// Compute triangle quality.
	static hkReal HK_CALL				computeTriangleQuality(const hkVector4& p0,const hkVector4& p1,const hkVector4& p2);

	/// Compute the median of two vectors
	static hkVector4 HK_CALL			computeMedian(const hkVector4& a,const hkVector4& b);
	
	/// Compute the median of three vectors
	static hkVector4 HK_CALL			computeMedian(const hkVector4& a,const hkVector4& b,const hkVector4& c);

	/// Compute the median of four vectors
	static hkVector4 HK_CALL			computeMedian(const hkVector4& a,const hkVector4& b,const hkVector4& c,const hkVector4& d);
		
	//
	// Macros
	//
	
	// Performance scope
	#define HK_PERF_SCOPE_EX(_name_,_ID_,_SILENT_)	hkGeometryProcessing::PerformanceScope performance_scope_##_ID_(_name_,_SILENT_)
	#define HK_PERF_SCOPE(_name_)					hkGeometryProcessing::PerformanceScope performance_scope(_name_,false)
	struct PerformanceScope
	{
		HK_FORCE_INLINE PerformanceScope(const char* name,bool silent) : m_name(name),m_silent(silent)
			{
				if(!m_silent) HK_REPORT("=> "<<name<<"...");
				m_stopWatch.start();
			}
		HK_FORCE_INLINE ~PerformanceScope()
			{
				m_stopWatch.stop();
				const hkReal	esecs=m_stopWatch.getElapsedSeconds();
				if(!m_silent)
				{
					if(esecs<=1)
					{ HK_REPORT("=> "<<m_name<<" : "<<(esecs*1000)<<" ms"); }
					else
					{ HK_REPORT("=> "<<m_name<<" : "<<(esecs*1000)<<" ms ("<<(esecs/60)<<" minutes)"); }
				}
			}
		hkStopwatch	m_stopWatch;
		const char*	m_name;
		bool		m_silent;
	};
	// Accumulator scope
	#define HK_ACC_SCOPE_EX(_storage_,_ID_)	hkGeometryProcessing::AccumulatorScope accumulator_scope_##_ID_(_storage_)
	#define HK_ACC_SCOPE(_storage_)			hkGeometryProcessing::AccumulatorScope accumulator_scope(_storage_)
	struct AccumulatorScope
	{
		struct Item { Item() { reset(); } void reset() { m_time=0;m_count=0; } hkReal m_time;int m_count; };
		HK_FORCE_INLINE AccumulatorScope(Item& storage) : m_storage(&storage) { ++storage.m_count;m_stopWatch.start(); }
		HK_FORCE_INLINE ~AccumulatorScope()	{ m_stopWatch.stop();m_storage->m_time+=m_stopWatch.getElapsedSeconds(); }
		hkStopwatch	m_stopWatch;
		Item*		m_storage;
	};

};
#include <Common/Internal/GeometryProcessing/hkGeometryProcessing.inl>

#endif

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
