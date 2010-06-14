/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2009 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HKGP_CONVEX_HULL_H
#define HKGP_CONVEX_HULL_H

class	hkAabb;
class	hkPseudoRandomGenerator;
struct	hkStridedVertices;

/// Default cosine of the angle above which two plane are merged.
#define HKGPCONVEXHULL_DEFAULT_MIN_COS_ANGLE			0.9999f

/// Default thickness used for splitting.
#define HKGPCONVEXHULL_DEFAULT_SPLIT_THICKNESS			0.0000001f

/// This class handles Convex hull generation. It also exposes utilities such as connectivity, mass properties and Boolean operations.
/// Convex hull generation is fast enough to be called at runtime on moderate input size ( < 10000 points )
///
/// It handles convex hull of dimensions up to 3 such as:
///
///  - 0D : Point
///  - 1D : Line
///  - 2D : Plane convex hull
///  - 3D : Volume convex hull
///
/// Notes A): Internally, the convex hull stores two sets of coordinates for each input point, 
///           the internal representation may differ slightly from the actual inputs.
///           For that reason, some methods need to be called with the coordinates type specified.
///           in this case, 'Source' refer to the input coordinates used to build the convex hull.
///
///		  B): Sources are never modified, including the 'W' component.
/// 
class hkgpConvexHull : public hkReferencedObject
{
public:

	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_GEOMETRY);
	struct	Vertex		{};
	struct	Triangle	{};		
public:

	/// Build configuration structure.
	struct BuildConfig
	{
		BuildConfig();
		hkBool		m_allowLowerDimensions;			///
		hkReal		m_minCosAngle;					///
		hkBool		m_setSourceIndices;				///
		hkBool		m_buildIndices;					///
		hkBool		m_buildMassProperties;			///
		hkBool		m_sortInputs;					///
		hkBool		m_internalInputs;				///

		/// Internally, the algorithm might modify input coordinates to ensure numerically robust results.
		/// As a result, the plane equations might not exactly confine the input points set.
		/// Setting this value to true will make sure the plane equations enclose both sources and internals points representations. (default: false).
		hkBool		m_ensurePlaneEnclosing;			
	};

	/// Simplification configuration structure.
	struct SimplifyConfig
	{
		SimplifyConfig();
		int			m_maxVertices;					///
		hkBool		m_ensureContainment;			///
		hkReal		m_minArea;						///
		hkReal		m_minEdgeLength;				///
		hkBool		m_removeUnreferencedVertices;	///
		hkBool		m_forceRebuild;					///
	};

	/// Absolute scale configuration structure.
	struct AbsoluteScaleConfig 
	{
		enum Method
		{
			SKM_VERTICES,	/// Scale by shifting vertices.
			SKM_PLANES,		/// Scale by shifting planes.
		};
		AbsoluteScaleConfig();
		Method		m_method;						///
		hkReal		m_minComDistance;				///
	};

	/// Input coordinates to use.
	enum Inputs
	{
		SOURCE_VERTICES,							///
		INTERNAL_VERTICES,							///
	};

	/// Result of 'classify' and 'splitByPlane'.
	enum Side
	{
		ZERO		,								///
		POSITIVE	,								///
		NEGATIVE	,								///
		CROSSING	,								///
		INVALID										///
	};

	/// User object interface.
	struct IUserObject
	{
		virtual					~IUserObject() {}
		virtual IUserObject*	clone() const=0;
		virtual void			append(const IUserObject* iOther)=0;
	};

	/// Boolean function of convex hulls.
	struct IBooleanFunction
	{
		virtual					~IBooleanFunction() {}
		virtual hkBool			operator()(const hkgpConvexHull*const* hulls,int numHulls) const=0;
	};

public:

	/// Constructor
						hkgpConvexHull();

	/// Destructor.
	virtual				~hkgpConvexHull();

	/// Get a unique identifier for that convex hull.
	hkUint32			uid() const;

	/// Reset the convex hull to an empty set of dimension -1.
	void				reset();

	/// Clone the convex hull, userData and properties.
	hkgpConvexHull*		clone() const;

	/// Get user data.
	void*&				userData() const;		

	/// Get the user object.
	IUserObject*&		userObject() const;

	/// Delete the user object.
	void				deleteUserObject();
	
	/// Builds a convex hull given a points set, return the resulting convex hull dimension or -1 if failed.
	int					build(const hkVector4* points, int numPoints, const BuildConfig& config=BuildConfig());

	/// Builds a convex hull given a points set, return the resulting convex hull dimension or -1 if failed.
	int					build(const hkStridedVertices& points, const BuildConfig& config=BuildConfig());

	/// Builds a planar convex hull given a points set and a projection plane, return the resulting convex hull dimension or -1 if failed.
	int					buildPlanar(const hkVector4* points, int numPoints, const hkVector4& plane, const BuildConfig& config=BuildConfig());

	/// Builds a planar convex hull given a points set and a projection plane, return the resulting convex hull dimension or -1 if failed.
	int					buildPlanar(const hkStridedVertices& points, const hkVector4& plane, const BuildConfig& config=BuildConfig());

	/// Builds a convex hull given a planes set, return the resulting convex hull dimension or -1 if failed.
	int					buildFromPlanes(const hkVector4* planes, int numPlanes, const BuildConfig& config=BuildConfig());

	/// Builds a convex hull given a planes set and a projection plane, return the resulting convex hull dimension or -1 if failed.
	int					buildFromPlanes(const hkVector4* planes, int numPlanes, const hkVector4& projectionPlane, const BuildConfig& config=BuildConfig());

	/// Simplify a convex hull.
	void				simplify(const SimplifyConfig& config);

	/// Expand the convex hull to ensure that it contains \a points.
	void				ensureContainment(const hkArray<hkVector4>& points);

	/// Decimate vertices of a convex hull.
	/// If \a ensureContainment is true, ensureContainment() is called automatically.
	bool				decimateVertices(int numVerticesToKeep, bool ensureContainment);	

	/// Expand convex hull with margin using face normal.
	bool				expandByPlanarMargin(const hkReal margin);

	/// Absolute scaling of convex hull.
	void				absoluteScale(hkReal offset,const AbsoluteScaleConfig& config=AbsoluteScaleConfig());

	/// Assigns plane, vertex and triangle indices.
	/// This data is needed for e.g. getIndex(), getPlaneIndex() and generateIndexedFaces().
	/// Planes are merged if PAxyz.PBxyz > minCosAngle.
	/// Note: this is automatically called by build if BuildConfig.m_buildIndices is set (default).
	void				buildIndices(hkReal	minCosAngle = HKGPCONVEXHULL_DEFAULT_MIN_COS_ANGLE, bool forceRebuild=false);

	/// Calculates and stores mass properties. See getVolume(), getLocalInertia().
	hkResult			buildMassProperties( );		

	/// Checks whether a point is a vertex of the convex hull.
	bool				isVertexUsed(const hkVector4& point) const;

	/// Checks whether a point is inside or on the convex hull.
	/// For maximum performance, call buildIndices() and set \a usePlanes to true.
	/// All planes will be expanded by \a planeThickness.
	bool				isPointInside( const hkVector4& point, bool usePlanes = false, hkReal planeThickness=0) const;

	/// Clips lines against planes, returning false if the line is completely outside the convex hull.
	/// All planes will be expanded by \a planeThickness.
	bool				clipLine(const hkVector4& a, const hkVector4& b, hkReal& t0, hkReal& t1, hkReal planeThickness=0) const;

	/// Retrieves the maximum distance to the planes of a given point.
	hkReal				maximumDistanceToPlanes(const hkVector4& point) const;

	/// Retrieves configuration used to build the convex hull.
	const BuildConfig&	getConfiguration() const;

	/// Gets the number of dimensions of the convex hull, returns -1 for invalid.
	int					getDimensions() const;

	/// Gets the axis-aligned bounding box of the convex hull.
	hkAabb				getBoundingBox(Inputs inputs, hkReal scale=1, hkReal radialScale=0) const;

	/// Gets the oriented bounding box of the convex hull.
	void				getOrientedBoundingBox(hkVector4& halfExtents, hkTransform& worldTransform) const;

	/// Gets the oriented rectangle of the convex hull given a projection axis.
	void				getOrientedRectangle(const hkVector4& projectionPlane, hkVector4& halfExtents, hkTransform& worldTransform) const;

	/// Gets the projection axis (only for convex hull of dimension 1).
	hkVector4			getProjectionAxis() const;

	/// Gets the projection plane (only for convex hull of dimension 2).
	hkVector4			getProjectionPlane() const;

	/// Retrieves the total number of planes, returned as the number of faces.
	int					getNumPlanes() const;

	/// Retrieves a plane by index.
	const hkVector4&	getPlane(int index) const;

	/// Retrieves the total number of vertices.
	int					getNumVertices() const;

	/// Retrieves the first vertex of the convex hull.
	Vertex*				getFirstVertex() const;

	/// Retrieves the vertex index for the specified vertex.
	int					getIndex(const Vertex* vertex) const;

	/// Retrieves the next vertex, which is part of the convex hull.
	Vertex*				getNext(const Vertex* vertex) const;

	/// Retrieves the world position of a vertex.
	hkVector4			getPosition(const Vertex* vertex) const;

	/// Retrieves the source position (used for building) of a vertex.
	const hkVector4&	getSourcePosition(const Vertex* vertex) const;

	/// Retrieves the total number of triangles.
	int					getNumTriangles() const;

	/// Retrieves the next triangle.
	Triangle*			getFirstTriangle() const;

	/// Retrieves triangle index.
	int					getIndex(const Triangle* triangle) const;

	/// Retrieves the next triangle.
	Triangle*			getNext(const Triangle* triangle) const;

	/// Retrieves the plane index of a triangle.
	int					getPlaneIndex(const Triangle* triangle) const;

	/// Retrieves the plane equation of a triangle.
	hkVector4			getPlaneEquation(const Triangle* triangle) const;

	/// Retrieves a vertex of triangle.
	Vertex*				getVertex(const Triangle* triangle, int index) const;

	/// Retrieves the neighbor triangle given an edge index.
	Triangle*			getNeighbour(Triangle* triangle, int index) const;

	/// Gets 'width*|direction|' of the convex hull.
	hkReal				getWidth(const hkVector4& direction) const;

	/// Gets the minimum distance from a plane.
	hkReal				getMinimumDistanceFromPlane(const hkVector4& plane) const;

	/// Gets the maximum distance from a plane
	hkReal				getMaximumDistanceFromPlane(const hkVector4& plane) const;

	/// Support mapping.
	void				getSupportingVertex(const hkVector4& direction,hkVector4& vertexOut) const;

	/// Gets nearest vertex from a given points.
	void				getNearestVertex(Inputs inputs, const hkVector4& point, hkVector4& vertexOut) const;

	/// Gets farthest vertex from a given point.
	void				getFarthestVertex(Inputs inputs, const hkVector4& point, hkVector4& vertexOut) const;

	/// Gets the centroid of the convex hull.
	const hkVector4&	getCentroid() const;

	/// Returns the sum of all edge lengths.
	hkReal				getEdgeLengths() const;

	/// Returns the sphericity of the convex hull.
	hkReal				getSphericity() const;

	/// Returns the volume of the convex hull.
	/// You need to call buildMassProperties() before you call this method.
	hkReal				getVolume() const;

	/// Returns the surface area of the convex hull.
	/// You need to call buildMassProperties() before you call this method.
	hkReal				getSurfaceArea() const;

	/// Retrieves inertia space and inertia diagonal.
	/// You need to call buildMassProperties() before you call this method.
	void				getLocalInertia(hkTransform& inertiaSpace, hkVector4& inertiaDiagonal) const;

	/// Retrieves world inertia matrix.
	/// You need to call buildMassProperties() before you call this method.
	const hkMatrix3&	getWorldInertia() const;

	/// Retrieves the center of mass.
	/// You need to call buildMassProperties() before you call this method.
	const hkVector4&	getCenterOfMass() const;

	/// Retrieves major (world) plane/axis from inertia.
	/// You need to call buildMassProperties() before you call this method.
	hkVector4			getMajorPlane() const;
	
	/// Retrieves width-sorted planes/axis from inertia.
	void				getMajorPlanes(hkVector4 planesOut[3]) const;

	/// Retrieves the best projection plane.
	hkVector4			getBestProjectionPlane() const;

	/// Classifies relative to a plane and retrieve distance to plane bounds.
	Side				classify(const hkVector4& plane, hkReal thickness, hkReal* bounds=0) const;

	/// Splits the convex hull by a plane.
	/// Note: Even in the case where the return value is CROSSING, posOut, negOut or both can be null is they are degenerate.
	/// Handling of those cases are application dependent.
	Side				splitByPlane(const hkVector4& plane, hkgpConvexHull** posOut, hkgpConvexHull** negOut, hkReal cuttingPlaneThickness=HKGPCONVEXHULL_DEFAULT_SPLIT_THICKNESS) const;

	/// Recursively subdivides the convex hull along its major axis.
	void				subdivide(int recursions, hkArray<hkgpConvexHull*>& output, hkReal thickness=HKGPCONVEXHULL_DEFAULT_SPLIT_THICKNESS) const;

	/// Fetches all vertex positions.
	void				fetchPositions(Inputs inputs, hkArray<hkVector4>& positionsOut) const;

	/// Fetches all planes.
	void				fetchPlanes(hkArray<hkVector4>& planes) const;

	/// Fetches bevel planes.
	void				fetchBevelPlanes( hkReal maxCosAngle, hkArray<hkVector4>& bevelPlanesOut ) const;

	/// Generates indexed faces, returns the number of faces/planes. 
	/// This data is needed by hkpConvexVerticesConnectivity.
	/// Note: if inputs == SOURCES, indices will be the one stored in source position with hkVector4::setInt24W.
	int					generateIndexedFaces(Inputs inputs, hkArray<hkUint8>& verticesPerFace, hkArray<hkUint16>& indices, bool maximizeArea) const;

	/// Generates indexed faces, returns the number of faces/planes.
	/// Note: if inputs == SOURCES, is set, indices will be the one stored in source position with hkVector4::setInt24W.
	int					generateIndexedFaces(Inputs inputs, hkArray<int>& verticesPerFace, hkArray<int>& indices, bool maximizeArea) const;

	/// Generates vertex adjacency graph as COUNT,EDGES for each vertex.
	void				generateVertexAdjacencyGraph(hkArray<int>& edgesOut, hkArray<int>& vertexOffsetsOut) const;

	/// Generates hkGeometry.
	void				generateGeometry(Inputs inputs, struct hkGeometry& geometryOut) const;

	//
	// Boolean operations
	//

	/// Boolean split of A with B.
	static void HK_CALL				booleanSplit(const hkgpConvexHull* operandA, const hkgpConvexHull* operandB, hkgpConvexHull** inside, hkArray<hkgpConvexHull*>* outside, hkReal thickness=HKGPCONVEXHULL_DEFAULT_SPLIT_THICKNESS);

	/// Boolean intersection of two convex hull (A & B).
	static bool HK_CALL 			booleanIntersection(const hkgpConvexHull* operandA, const hkgpConvexHull* operandB, hkgpConvexHull*& output, hkReal thickness=HKGPCONVEXHULL_DEFAULT_SPLIT_THICKNESS);

	/// Boolean subtraction of two convex hull (A - B).
	static bool HK_CALL 			booleanSubtraction(const hkgpConvexHull* operandA, const hkgpConvexHull* operandB, hkArray<hkgpConvexHull*>& output, hkReal thickness=HKGPCONVEXHULL_DEFAULT_SPLIT_THICKNESS);

	/// Boolean subtraction of two convex hull (A - Bs).
	static bool HK_CALL 			booleanSubtraction(const hkgpConvexHull* operandA, const hkArray<hkgpConvexHull*>& operandsB, hkArray<hkgpConvexHull*>& output, hkReal thickness=HKGPCONVEXHULL_DEFAULT_SPLIT_THICKNESS);
	
	//
	// Utilities
	//		

	/// Volume based simplification, removed vertices until volume difference is greater than \a threshold (0.0f = 0% , 1.0f = 100%).
	/// If \a ensureContainment is true, ensureContainment() is called automatically
	static hkgpConvexHull* HK_CALL	volumeSimplify(const hkgpConvexHull* hull, hkReal threshold, bool ensureContainment);

	/// compute Minkowski sum or difference {x + y * bScale | x e A, y e B}
	static hkgpConvexHull* HK_CALL	computeMinkowski(const hkStridedVertices& A, const hkStridedVertices& B, hkSimdRealParameter bScale);
	
	/// Finds the best splitting plane from convex hull a that maximizes the volume of -b and satisfies f(a U -b , +b); returns the volume of -b.
	static hkReal HK_CALL			findBestUnion(const hkgpConvexHull* a, const hkgpConvexHull* b, const IBooleanFunction& f, hkgpConvexHull*& aUnb, hkgpConvexHull*& pb);
	
	/// Creates a compound convex hull from a given set.
	static hkgpConvexHull* HK_CALL	createCompound(Inputs inputs, const hkgpConvexHull* const* hulls, int numHulls);

	/// Creates a compound convex hull from a given set.
	static hkgpConvexHull* HK_CALL	createCompound(Inputs inputs, const hkgpConvexHull* a, const hkgpConvexHull* b);	

	/// Creates random samples inside the convex hull.
	static void HK_CALL				createRandomSamples(const hkgpConvexHull* domain,hkPseudoRandomGenerator& generator,int numSamples,hkArray<hkVector4>& samples);

	/// Creates Voronoi cells from a convex domain and a points set.
	static void HK_CALL				createVoronoiCells(const hkgpConvexHull* domain,const hkArray<hkVector4>& points,hkArray<hkgpConvexHull*>& cells,hkReal margin=0);

	/// Creates Voronoi edges from a convex domain and a points set.
	static void HK_CALL				createVoronoiEdges(const hkgpConvexHull* domain,const hkArray<hkVector4>& points,hkArray<hkgpConvexHull*>& cells,hkReal margin=0);

	/// Creates Voronoi features from a convex domain and a points set.
	static void HK_CALL				createVoronoiFeatures(const hkgpConvexHull* domain,const hkArray<hkVector4>& points,hkArray<hkgpConvexHull*>& cells,hkArray<hkgpConvexHull*>& edges,hkReal margin=0);

private:
	inline			hkgpConvexHull(const hkgpConvexHull& ch)	: hkReferencedObject(ch)	{}
	void			operator=(const hkgpConvexHull&)										{}

private:

	friend class hkgpConvexHullImpl;
	class hkgpConvexHullImpl*	m_data;
	mutable void*				m_userData;
	mutable IUserObject*		m_userObject;
};

#endif // HKGP_CONVEX_HULL_H

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
