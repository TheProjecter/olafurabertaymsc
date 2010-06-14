/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2009 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#if defined(HK_DEBUG) && defined(HK_PLATFORM_WIN32)
#undef HK_FORCE_INLINE
#define HK_FORCE_INLINE inline
#pragma auto_inline(off)
#endif

//
TRIANGULATOR_HDR
HK_FORCE_INLINE											TRIANGULATOR_TYP::hkgpTriangulatorType()
{
	hkVector4	dmin,dmax;
	dmin.set(-1,-1,0,0);
	dmax.set(+1,+1,0,0);
	m_edgeData = HK_NULL;
	setDomain(dmin,dmax,0,true);
}
//
TRIANGULATOR_HDR
HK_FORCE_INLINE											TRIANGULATOR_TYP::~hkgpTriangulatorType()
{
	reset();
}

#if HK_POINTER_SIZE == 4

TRIANGULATOR_HDR
HK_FORCE_INLINE 									TRIANGULATOR_TYP::Edge::Edge( const Edge& other )
: tBase( other.m_triangle, other.m_index )
{
}

TRIANGULATOR_HDR
HK_FORCE_INLINE void								TRIANGULATOR_TYP::Edge::operator=( const Edge& other )
{
 	HK_COMPILE_TIME_ASSERT( sizeof (Edge) == 8);
 	HK_ASSERT(0x49c435c3, (hkUlong(this) % 4) == 0);
 	HK_ASSERT(0x49c435c3, (hkUlong(&other) % 4) == 0);
 	hkUint32* dst = reinterpret_cast<hkUint32*> (this);
 	const hkUint32* src = reinterpret_cast<const hkUint32*> (&other);
 	dst[0] = src[0];
 	dst[1] = src[1];
}

#endif 
//
TRIANGULATOR_HDR
HK_FORCE_INLINE hkBool32								TRIANGULATOR_TYP::Edge::canFlip() const
{
	if(!tBase::isNaked()&&!isConstrained())
	{
		const Edge	lk=tBase::link();
		const Edge	ep=tBase::prev();
		const Edge	lp=lk.prev();
		return(	(orientation(ep.start(),tBase::start(),lp.start())>0)&&
				(orientation(lp.start(),lk.start(),ep.start())>0));
	}
	return(false);
}
//
TRIANGULATOR_HDR
HK_FORCE_INLINE typename TRIANGULATOR_TYP::Edge			TRIANGULATOR_TYP::Edge::flip() const
{
	HK_ASSERT2(0x122374,canFlip(),"Invalid flip");
	const hkBool32	c0=tBase::prev().getAndClearConstraint();
	const hkBool32	c1=tBase::link().prev().getAndClearConstraint();
	Edge		edge=tBase::flip();
	if(c0) edge.link().next().setConstrained();
	if(c1) edge.next().setConstrained();
	return(edge);
}

//
TRIANGULATOR_HDR
HK_FORCE_INLINE int										TRIANGULATOR_TYP::Edge::rank() const
{
	Edge	edge=*this;
	int		r=0;
	if(tBase::start()->isBoundary())
	{
		while(!edge.isNaked()) edge.turnCcwInPlace();
		do { ++r;edge.turnCwInPlace(); } while(edge.isValid());
	}
	else
	{
		const Edge base=edge;
		do { ++r;edge.turnCwInPlace(); } while(edge!=base);
	}
	return(r);
}
//
TRIANGULATOR_HDR
HK_FORCE_INLINE 										TRIANGULATOR_TYP::EdgeDataBox::EdgeDataBox(const Edge& edge)
: m_data(HK_NULL)
{
	m_vertices[0]	=	edge.start();
	m_vertices[1]	=	edge.end();	
}
//
TRIANGULATOR_HDR
HK_FORCE_INLINE hkUlong									TRIANGULATOR_TYP::EdgeDataBox::getHash() const
{
	return(hkGeometryProcessing::makeHash(m_vertices[0]->pack(),m_vertices[1]->pack()));
}
//
TRIANGULATOR_HDR
HK_FORCE_INLINE hkBool32								TRIANGULATOR_TYP::EdgeDataBox::operator==(const EdgeDataBox& other) const
{
	return(m_vertices[0]==other.m_vertices[0] && m_vertices[1]==other.m_vertices[1]);
}
//
TRIANGULATOR_HDR
HK_FORCE_INLINE hkBool32								TRIANGULATOR_TYP::SortByArea_Predicate::operator()(const Triangle* a,const Triangle* b) const
{
	if(cfgIsCCW)
		return orientation(a->vertex(0),a->vertex(1),a->vertex(2)) > orientation(b->vertex(0),b->vertex(1),b->vertex(2));
	else
		return orientation(a->vertex(0),a->vertex(1),a->vertex(2)) < orientation(b->vertex(0),b->vertex(1),b->vertex(2));
}
//
TRIANGULATOR_HDR
HK_FORCE_INLINE typename TRIANGULATOR_TYP::tSets		TRIANGULATOR_TYP::FloodPolicy::getSetsFromEdgeData(const EdgeData* edgeData) const
{
	return (tSets)(1<<edgeData->m_tag);
}
//
TRIANGULATOR_HDR
HK_FORCE_INLINE hkInt32									TRIANGULATOR_TYP::orientation(hkInt32 ax,hkInt32 ay,hkInt32 bx,hkInt32 by,hkInt32 cx,hkInt32 cy)
{
	bx-=ax;by-=ay;
	cx-=ax;cy-=ay;
	return(ORIENTATION * (by*cx-bx*cy));
}

//
TRIANGULATOR_HDR
HK_FORCE_INLINE hkInt32									TRIANGULATOR_TYP::orientation(const Vertex* a,hkInt32 bx,hkInt32 by,hkInt32 cx,hkInt32 cy)
{
	return(orientation(a->m_x,a->m_y,bx,by,cx,cy));
}

//
TRIANGULATOR_HDR
HK_FORCE_INLINE hkInt32									TRIANGULATOR_TYP::orientation(const Vertex* a,const Vertex* b,hkInt32 x,hkInt32 y)
{
	return(orientation(a->m_x,a->m_y,b->m_x,b->m_y,x,y));
}
//
TRIANGULATOR_HDR
HK_FORCE_INLINE hkInt32									TRIANGULATOR_TYP::orientation(const Vertex* a,const Vertex* b,const Vertex* c)
{
	return(orientation(a->m_x,a->m_y,b->m_x,b->m_y,c->m_x,c->m_y));
}
//
TRIANGULATOR_HDR
HK_FORCE_INLINE hkInt32									TRIANGULATOR_TYP::orientation(const Edge& e,hkInt32 x,hkInt32 y)
{
	return(orientation(e.start(),e.end(),x,y));
}
//
TRIANGULATOR_HDR
HK_FORCE_INLINE hkInt32									TRIANGULATOR_TYP::orientation(const Edge& e,const Vertex* c)
{
	return(orientation(e,c->m_x,c->m_y));
}
//
TRIANGULATOR_HDR
HK_FORCE_INLINE hkBool32								TRIANGULATOR_TYP::checkOrientation(hkInt32 x0,hkInt32 y0,hkInt32 x1,hkInt32 y1)
{
	if(cfgIsCW)
	{
		hkAlgorithm::swap(x0,x1);
		hkAlgorithm::swap(y0,y1);
	}
	if(x0==x1)
	{
		if((x0==0) && (y0<y1))					return(false);
		else if((x0==cfgBoundary) && (y0>y1))	return(false);
	}
	if(y0==y1)
	{
		if((y0==0) && (x0>x1))					return(false);
		else if((y0==cfgBoundary) && (x0<x1))	return(false);
	}
	return(true);
}
//
TRIANGULATOR_HDR
HK_FORCE_INLINE hkBool32								TRIANGULATOR_TYP::incircle(hkInt32 ax,hkInt32 ay,hkInt32 bx,hkInt32 by,hkInt32 cx,hkInt32 cy,hkInt32 dx,hkInt32 dy)
{
	ax-=dx;ay-=dy;
	bx-=dx;by-=dy;
	cx-=dx;cy-=dy;
	const tDet2x2	d0=(tDet2x2)(ax*by-bx*ay);
	const tDet2x2	d1=(tDet2x2)(bx*cy-cx*by);
	const tDet2x2	d2=(tDet2x2)(cx*ay-ax*cy);
	const tDet2x2	l0=(tDet2x2)(ax*ax+ay*ay);
	const tDet2x2	l1=(tDet2x2)(bx*bx+by*by);
	const tDet2x2	l2=(tDet2x2)(cx*cx+cy*cy);
	return ((l0*d1+l1*d2+l2*d0)*(tDet2x2)ORIENTATION) < 0;
}
//
TRIANGULATOR_HDR
HK_FORCE_INLINE hkInt32									TRIANGULATOR_TYP::clamp(hkInt32 x,hkInt32 min,hkInt32 max)
{
	return(x<min?min:x>max?max:x);
}
//
TRIANGULATOR_HDR
HK_FORCE_INLINE hkInt32									TRIANGULATOR_TYP::round(hkReal x)
{
	return((int)(x<0?x-0.5f:x+0.5f));
}
//
TRIANGULATOR_HDR
HK_FORCE_INLINE int										TRIANGULATOR_TYP::rootIndex(int x,int y)
{
	x>>=cfgRootShift;
	y>>=cfgRootShift;
	return((y<<cfgRoots)+x);
}
//
TRIANGULATOR_HDR
HK_FORCE_INLINE	hkBool32								TRIANGULATOR_TYP::sameBoundary(const Vertex* v0,const Vertex* v1)
{
	if(v0->m_x==v1->m_x)
		if((v0->m_x==0)||(v0->m_x==cfgBoundary)) return(true);
	if(v0->m_y==v1->m_y)
		if((v0->m_y==0)||(v0->m_y==cfgBoundary)) return(true);
	return(false);
}
//
TRIANGULATOR_HDR
HK_FORCE_INLINE hkBool32								TRIANGULATOR_TYP::isConform(const Vertex* a,const Vertex* b,const Vertex* c,const Vertex* p)
{
	return(!incircle(a->m_x,a->m_y,b->m_x,b->m_y,c->m_x,c->m_y,p->m_x,p->m_y));
}
//
TRIANGULATOR_HDR
inline void												TRIANGULATOR_TYP::removeOrphanVertices()
{
	hkPointerMap<Vertex*,bool>	orphans;
	orphans.reserve(m_mesh.m_vertices.getSize());
	/* Append all vertices							*/ 
	for(typename Mesh::VertexIterator v=m_mesh.getVertexIterator();v;++v)
	{
		orphans.insert(v(),true);
	}
	/* Remove those referenced by constrained edges	*/ 
	for(typename Mesh::EdgeIterator e=m_mesh.getEdgeIterator();e;++e)
	{
		Edge	edge(e());
		if(edge.isConstrained())
		{
			orphans.remove(edge.start());
			orphans.remove(edge.end());
		}
	}
	/* Remove vertices								*/ 
	for(typename hkPointerMap<Vertex*,bool>::Iterator i=orphans.getIterator();orphans.isValid(i);i=orphans.getNext(i))
	{
		Vertex*	v=orphans.getKey(i);
		if(v->isCorner())
			v->clrConstrained();
		else
			removeVertex(*v);		
	}
}
//
TRIANGULATOR_HDR
inline int												TRIANGULATOR_TYP::getOpenBoundaries(hkArray<Edge>* edges) const
{
	/* Backup regions				*/ 
	hkArray<hkUint16>	regionsBackup;
	regionsBackup.reserveExactly(m_mesh.m_triangles.getSize());
	for(Triangle* t=m_mesh.m_triangles.getFirst();t;t=t->next())
	{
		hkUint16 region = t->m_region;
		regionsBackup.pushBack(region);
	}
	
	/* Partition					*/ 
	partition();
	
	/* Check for open boundaries	*/ 
	int		numFounds=0;
	for(Triangle* t=m_mesh.m_triangles.getFirst();t;t=t->next())
	{
		for(int i=0;i<3;++i)
		{
			Edge	e(t,i);
			if(e.isBound() && e.isConstrained())
			{
				if(e.triangle()->m_region==e.link().triangle()->m_region)
				{
					++numFounds;
					if(edges) edges->pushBack(e);
				}
			}
		}
	}

	/* Restore regions				*/ 
	const hkUint16*		regionsPointer=&regionsBackup[0];
	for(Triangle* t=m_mesh.m_triangles.getFirst();t;t=t->next())
	{
		t->m_region = *regionsPointer++;
	}

	return numFounds;
}
//
TRIANGULATOR_HDR
inline hkBool32											TRIANGULATOR_TYP::getEdgeDataOrientation() const
{
	int	counts[]={0,0};
	for(Triangle* t=m_mesh.m_triangles.getFirst();t;t=t->next())
	{
		if(t->m_region&1)
		{
			for(int i=0;i<3;++i)
			{
				Edge	e(t,i);
				if(e.isBound())
				{
					const EdgeData* dIn = getEdgeData(e);
					const EdgeData* dOut = getEdgeData(e.link());
					if(dIn) ++counts[0]; else if(dOut) ++counts[1];
				}
				else
				{
					if(getEdgeData(e)) ++counts[0]; else ++counts[1];
				}
			}
		}
	}
	return counts[0]>counts[1];
}
//
TRIANGULATOR_HDR
HK_FORCE_INLINE hkBool32								TRIANGULATOR_TYP::isConform(const Edge& edge)
{
	return(edge.isNaked()||edge.isConstrained()||isConform(edge.apex(),edge.start(),edge.end(),edge.link().apex()));
}
//
TRIANGULATOR_HDR
const typename TRIANGULATOR_TYP::EdgeData*				TRIANGULATOR_TYP::getEdgeData(const Edge& edge) const
{
	if(edge.isValid() && m_edgeData)
	{
		EdgeDataBox box(edge);
		const EdgeDataBox*	item=m_edgeData->find(box);
		return item? item->m_data : HK_NULL;
	}
	return HK_NULL;
}
//
TRIANGULATOR_HDR
void													TRIANGULATOR_TYP::setEdgeData(const Edge& edge, const EdgeData& data)
{
	HK_ASSERT2(0x52faa065, edge.isConstrained(), "Edge data are only accessible for constrained edges." );
	if(!m_edgeData)
	{
		createEdgeDataHashTable();
	}

	EdgeDataBox box(edge);
	EdgeDataBox*	edgeData=m_edgeData->find(box);

	if(!edgeData)
	{
		edgeData=m_edgeData->insert(box);
		edgeData->m_data=m_edgeDataAllocator.allocate();
	}
	edgeData->m_data->appendOrReplace(data);
}
//
TRIANGULATOR_HDR
void													TRIANGULATOR_TYP::clearEdgeData(const Edge& edge)
{
	if(m_edgeData)
	{
		EdgeDataBox box(edge);
		EdgeDataBox*	edgeData=m_edgeData->find(box);
		if(edgeData)
		{
			m_edgeDataAllocator.release(edgeData->m_data);
			m_edgeData->remove(box);
		}
		if(m_edgeData->m_numItems==0)
		{
			destroyEdgeDataHashTable();
		}
	}
}
//
TRIANGULATOR_HDR
HK_FORCE_INLINE hkInt32									TRIANGULATOR_TYP::distanceSqrd(const Vertex* a,const Vertex* b)
{
	hkInt32 dx=a->m_x-b->m_x;
	hkInt32 dy=a->m_y-b->m_y;
	return(dx*dx+dy*dy);
}

//
TRIANGULATOR_HDR
inline hkBool32											TRIANGULATOR_TYP::canSplitEdge(int ix,int iy,const Edge& edge)
{
	if(orientation(edge.next(),ix,iy)<=0)									return(false);
	else if(orientation(edge.prev(),ix,iy)<=0)								return(false);
	else if(edge.isBound() && (orientation(edge.link().next(),ix,iy)<=0))	return(false);
	else if(edge.isBound() && (orientation(edge.link().prev(),ix,iy)<=0))	return(false);
	return(true);
}

//
TRIANGULATOR_HDR
HK_FORCE_INLINE void									TRIANGULATOR_TYP::copyDataset(const Triangle* from,Triangle* to)
{
	to->m_region	= from->m_region;
	to->m_sets		= from->m_sets;
}

//
TRIANGULATOR_HDR
inline void												TRIANGULATOR_TYP::backupBothEdgeData(Edge edge,EdgeDataValue values[2])
{
	values[0].m_hasData	=	false;
	values[1].m_hasData	=	false;
	for(int i=0;i<2;++i)
	{
		const EdgeData*	data=getEdgeData(edge);
		if(data)
		{			
			values[i].m_data	=	*data;
			values[i].m_hasData	=	true;
			clearEdgeData(edge);
		}
		if(edge.isBound()) edge=edge.link(); else break;
	}
}

//
TRIANGULATOR_HDR
inline void												TRIANGULATOR_TYP::restoreBothEdgeData(Edge edge, const EdgeDataValue values[2])
{
	for(int i=0;i<2;++i)
	{
		if(values[i].m_hasData) setEdgeData(edge,values[i].m_data);
		if(edge.isBound()) edge=edge.link(); else break;
	}
}

//
TRIANGULATOR_HDR
inline void												TRIANGULATOR_TYP::createEdgeDataHashTable()
{
	ALLOCATOR			allocatorType;
	hkMemoryAllocator&	allocator(allocatorType.get(0));
	m_edgeData = new(allocator.blockAlloc(sizeof(EdgeDataHashTable))) EdgeDataHashTable(cfgEdgeDataHashSize);
}

//
TRIANGULATOR_HDR
inline void												TRIANGULATOR_TYP::destroyEdgeDataHashTable()
{	
	if(m_edgeData)
	{
		ALLOCATOR			allocatorType;
		hkMemoryAllocator&	allocator(allocatorType.get(0));
		m_edgeData->~EdgeDataHashTable();
		allocator.blockFree(m_edgeData,sizeof(EdgeDataHashTable));
		m_edgeData = HK_NULL;
	}
}

//
TRIANGULATOR_HDR
HK_FORCE_INLINE void									TRIANGULATOR_TYP::setRoot(Triangle* t)
{
	const int	cx=(t->vertex(0)->m_x+t->vertex(1)->m_x+t->vertex(2)->m_x)/3;
	const int	cy=(t->vertex(0)->m_y+t->vertex(1)->m_y+t->vertex(2)->m_y)/3;
	m_roots[rootIndex(cx,cy)]=t;
	t->m_root=1;
}
//
TRIANGULATOR_HDR
HK_FORCE_INLINE typename TRIANGULATOR_TYP::Vertex*		TRIANGULATOR_TYP::newVertex(hkInt32 x,hkInt32 y,hkBool32 constraint)
{
	Vertex*		v=m_mesh.m_vertices.allocate();
	v->m_x=x;
	v->m_y=y;
	v->m_constraint=constraint;
	v->m_data=0;
	v->m_data=~v->m_data;
	return(v);
}
//
TRIANGULATOR_HDR
HK_FORCE_INLINE void									TRIANGULATOR_TYP::releaseVertex(Vertex* v)
{
	m_mesh.m_vertices.release(v);
}
//
TRIANGULATOR_HDR
HK_FORCE_INLINE typename TRIANGULATOR_TYP::Triangle*	TRIANGULATOR_TYP::newTriangle(Vertex* a,Vertex* b,Vertex* c)
{
	Triangle*						t=m_mesh.m_triangles.allocate();
	t->setVertices(a,b,c);
	t->m_constraints=0;
	t->m_region=0;
	t->m_root=0;
	t->m_sets=0;
	setRoot(t);
	return(t);
}
//
TRIANGULATOR_HDR
HK_FORCE_INLINE void									TRIANGULATOR_TYP::releaseTriangle(Triangle* t)
{
	if(t->m_root)
	{
		for(int i=0;i<cfgTotalRoots;++i)
		{
			if(m_roots[i]==t) m_roots[i]=0;
		}
	}
	m_mesh.m_triangles.release(t);
}
//
TRIANGULATOR_HDR
inline void												TRIANGULATOR_TYP::reset(bool createDomainTriangulation)
{
	/* Reset storages	*/ 
	m_mesh.reset();
	/* Clear roots		*/ 
	for(int i=0;i<cfgTotalRoots;++i)
	{
		m_roots[i]=0;
	}
	/* Clear edge data	*/ 
	destroyEdgeDataHashTable();
	m_edgeDataAllocator.clear();
	/* Create base		*/ 
	if(createDomainTriangulation)
	{
		Vertex*		c00(newVertex(0,0,false));
		Vertex*		c01(newVertex(0,cfgBoundary,false));
		Vertex*		c10(newVertex(cfgBoundary,0,false));
		Vertex*		c11(newVertex(cfgBoundary,cfgBoundary,false));
		if(cfgIsCCW)
		{
			Triangle*	t0(newTriangle(c00,c01,c11));
			Triangle*	t1(newTriangle(c00,c11,c10));
			t0->link(0)=t0->link(1)=t0->link(2)=0;
			t1->link(0)=t1->link(1)=t1->link(2)=0;
			Edge(t0,2).bind(Edge(t1,0));
		}
		else
		{
			Triangle*	t0(newTriangle(c00,c10,c11));
			Triangle*	t1(newTriangle(c00,c11,c01));
			t0->link(0)=t0->link(1)=t0->link(2)=0;
			t1->link(0)=t1->link(1)=t1->link(2)=0;
			Edge(t0,2).bind(Edge(t1,0));
		}
	}	
}
//
TRIANGULATOR_HDR
inline void												TRIANGULATOR_TYP::clone(hkgpTriangulatorType* other) const
{
	other->reset(false);
	other->m_localToWorld	=	m_localToWorld;
	other->m_worldToLocal	=	m_worldToLocal;
	other->m_margin			=	m_margin;
	hkPointerMap<Vertex*,Vertex*>					vmap;
	hkPointerMap<Triangle*,Triangle*>				tmap;
	hkPointerMap<hkgpTopology::Uid,hkgpTopology::Uid>	emap;
	for(Vertex* v=m_mesh.m_vertices.getFirst();v;v=v->next())
	{
		Vertex*		nv=other->m_mesh->m_vertices.allocate(*v);
		vmap.insert(v,nv);
	}
	for(Triangle* t=m_mesh.m_triangles.getFirst();t;t=t->next())
	{
		Triangle*	nt=other->m_mesh->m_triangles.allocate(*t);
		tmap.insert(t,nt);
		for(int i=0;i<3;++i)
		{
			nt->vertex(i)=vmap.getWithDefault(nt->vertex(i),0);
		}
		for(int i=0;i<3;++i)
		{
			Edge	te(t,i);
			Edge	ne(nt,i);
			if(te.isNaked())
			{
				nt->link(i)=0;
			}
			else
			{
				Edge	link(te.link());
				Edge	le(emap.getWithDefault(link.uid,0));
				if(le.isValid())
				{
					ne.bind(le);
					emap.remove(link.uid());
				}
				else
				{
					emap.insert(te.uid(),ne.uid());
				}
			}
		}
	}
	for(int i=0;i<cfgTotalRoots;++i)
	{
		if(m_roots[i])
		{
			other->m_roots[i]=tmap.getWithDefault(m_roots[i],0);
		}
		else
		{
			other->m_roots[i]=0;
		}
	}
}

//
TRIANGULATOR_HDR
inline void												TRIANGULATOR_TYP::setDomain(const hkVector4& dmin,const hkVector4& dmax,hkReal angle,bool margin)
{
	HK_ASSERT2(0x454797,(dmin(0)<dmax(0))&&(dmin(1)<dmax(1)),"Invalid domain");
	m_margin	=	margin?1:0;
	const hkReal	ext(cfgBoundary-m_margin*2.0f);
	hkVector4		scl;
	const hkReal	s=hkMath::sin(angle);
	const hkReal	c=hkMath::cos(angle);
	scl.set((dmax(0)-dmin(0))/ext,(dmax(1)-dmin(1))/ext,0,0);
	m_localToWorld=hkMatrix4::getIdentity();
	m_localToWorld(0,0)=	c*scl(0);	m_localToWorld(0,1)=	s*scl(1);	m_localToWorld(0,3)=	dmin(0);
	m_localToWorld(1,0)=	-s*scl(0);	m_localToWorld(1,1)=	c*scl(1);	m_localToWorld(1,3)=	dmin(1);
	m_worldToLocal.setInverse(m_localToWorld,0);
	reset();
}
//
TRIANGULATOR_HDR
inline void												TRIANGULATOR_TYP::setDomainFromPoints(const hkStridedVertices& points, const hkMatrix4& worldToXYPlane, bool margin, bool keepRatio)
{
	/* Calculate domain bounds			*/ 
	hkVector4	dmin,dmax;
	dmin.set(+HK_REAL_MAX,+HK_REAL_MAX,+HK_REAL_MAX,0);
	dmax.set(-HK_REAL_MAX,-HK_REAL_MAX,-HK_REAL_MAX,0);
	for(int i=0,ni=points.getSize();i<ni;++i)
	{
		hkVector4	tp;
		hkVector4	vt;
		points.getVertex(i,vt);
		worldToXYPlane.transformPosition(vt,tp);
		dmin.setMin4(dmin,tp);
		dmax.setMax4(dmax,tp);
	}
	/* Fixup zero bounds				*/ 
	const hkReal	minExtend=HK_REAL_EPSILON*100;
	if((dmax(0)-dmin(0))<minExtend) { dmin(0)-=minExtend;dmax(0)+=minExtend; }
	if((dmax(1)-dmin(1))<minExtend) { dmin(1)-=minExtend;dmax(1)+=minExtend; }
	/* Keep ratio						*/ 
	hkVector4		extents;extents.setSub4(dmax,dmin);
	if(keepRatio)
	{
		const int		maxExt=extents(0)>extents(1)?0:1;
		const int		minExt=1-maxExt;
		const hkReal	half=(extents(maxExt)-extents(minExt))*0.5f;
		dmin(minExt)	-=	half;
		dmax(minExt)	+=	half;
		extents(minExt)	=	extents(maxExt);
	}
	/* Evaluate transforms				*/ 
	m_margin	=	margin?1:0;
	const hkReal	ext(cfgBoundary-m_margin*2.0f);
	hkMatrix4		trs(hkMatrix4::getIdentity());
	trs(0,0)	=	ext/extents(0);
	trs(1,1)	=	ext/extents(1);
	trs(0,3)	=	-dmin(0)*trs(0,0)+m_margin;
	trs(1,3)	=	-dmin(1)*trs(1,1)+m_margin;
	m_worldToLocal.setMul(trs,worldToXYPlane);
	m_localToWorld.setInverse(m_worldToLocal,0);
	reset();
}
//
TRIANGULATOR_HDR
inline void												TRIANGULATOR_TYP::setDomainFromPoints(const hkStridedVertices& points,bool margin,bool keepRatio)
{
	setDomainFromPoints(points,hkMatrix4::getIdentity(),margin,keepRatio);
}

//
TRIANGULATOR_HDR
inline void												TRIANGULATOR_TYP::setDomainFromPlanarPoints(const hkStridedVertices& points,hkVector4Parameter planeNormal,bool margin,bool keepRatio)
{
	hkVector4		xyPlane;xyPlane.set(0,0,1,0);
	hkQuaternion	rot;
	rot.setShortestRotation(planeNormal,xyPlane);
	hkTransform		trs=hkTransform::getIdentity();
	trs.setRotation(rot);
	hkMatrix4		mat;
	mat.set(trs);

	setDomainFromPoints(points,mat,margin,keepRatio);
}

//
TRIANGULATOR_HDR
inline hkVector4										TRIANGULATOR_TYP::getEffectiveAccuracy() const
{
	hkVector4	d,td,acc;
	acc.setZero4();
	d.set(1,0,0,0);	m_localToWorld.transformDirection(d,td); acc(0)=td.length3();
	d.set(0,1,0,0);	m_localToWorld.transformDirection(d,td); acc(1)=td.length3();
	return acc;
}

//
TRIANGULATOR_HDR
HK_FORCE_INLINE hkVector4								TRIANGULATOR_TYP::transform(const Vertex* v) const
{
	hkVector4	x;
	hkVector4 vReal; vReal.set( (hkReal)v->m_x, (hkReal)v->m_y, 0);
	m_localToWorld.transformPosition(vReal, x);
	return(x);
}
//
TRIANGULATOR_HDR
HK_FORCE_INLINE hkVector4								TRIANGULATOR_TYP::transformUnquantized(const hkVector4& v) const
{
	hkVector4	x;
	m_worldToLocal.transformPosition(v,x);
	return x;
}	
//
TRIANGULATOR_HDR
HK_FORCE_INLINE typename TRIANGULATOR_TYP::Vertex		TRIANGULATOR_TYP::quantize(const hkVector4& v) const
{
	const int	bmi=m_margin;
	const int	bmx=cfgBoundary-m_margin;
	Vertex		k;
	k.m_x	=	clamp(round(v(0)),bmi,bmx);
	k.m_y	=	clamp(round(v(1)),bmi,bmx);
	return k;
}
//
TRIANGULATOR_HDR
HK_FORCE_INLINE typename TRIANGULATOR_TYP::Vertex		TRIANGULATOR_TYP::transform(const hkVector4& v) const
{
	return quantize(transformUnquantized(v));
}	
//
TRIANGULATOR_HDR
inline hkBool32											TRIANGULATOR_TYP::isConform() const
{
	for(Triangle* t=m_mesh.m_triangles.getFirst();t;t=t->next())
	{
		for(int i=0;i<3;++i)
		{
			if(!isConform(Edge(t,i)))
			{
				return(false);
			}
		}
	}
	return(true);
}
//
TRIANGULATOR_HDR
HK_FORCE_INLINE typename TRIANGULATOR_TYP::Edge			TRIANGULATOR_TYP::conformEdge(const Edge& edge)
{
	const Vertex*	v0=edge.start();
	const Vertex*	v1=edge.end();
	m_stack.pushBack(edge.next());
	m_stack.pushBack(edge.prev());
	if(!edge.isNaked())
	{
		m_stack.pushBack(edge.nextCcw());
		m_stack.pushBack(edge.nextCw());
	}
	flushConform();
	return(locateExistingEdge(edge,v0,v1));
}	
//
TRIANGULATOR_HDR
HK_FORCE_INLINE typename TRIANGULATOR_TYP::Triangle*	TRIANGULATOR_TYP::getRoot(int x,int y) const
{
	Triangle* root=m_roots[rootIndex(x,y)];
	if(!root) root=m_mesh.m_triangles.getFirst();
	return(root);
}
//
TRIANGULATOR_HDR
HK_FORCE_INLINE typename TRIANGULATOR_TYP::Location		TRIANGULATOR_TYP::locateVertex(Edge root,int x,int y) const
{
	HK_ASSERT2(0x9845757,(x>=0)&&(x<=cfgBoundary)&&(y>=0)&&(y<=cfgBoundary),"Vertex out of domain");
	for(Triangle* base=m_mesh.m_triangles.getFirst();base;base=base->next())
	{
		int	last=0;
		while((last=orientation(root,x,y))<0)
		{
			root.turnCcwInPlace();
		}
		for(int n=m_mesh.m_triangles.getSize();n>0;--n)
		{
			const Edge	es[]={root.next(),root.prev()};
			const int	dir[]={orientation(es[0],x,y),orientation(es[1],x,y)};
			const int	moi=dir[0]<dir[1]?0:1;
			const int	mov=dir[moi];
			const Edge	med=es[moi];
			if(mov<0)
			{
				last	=	-mov;
				root	=	med.link();
			}
			else
			{
				const int type=(last==0?1:0)+(dir[0]==0?2:0)+(dir[1]==0?4:0);
				switch(type)
				{
				case	0+0+0:	return(Location(Location::IN_TRIANGLE,root));
				case	0+0+1:	return(Location(Location::ON_EDGE,root));
				case	0+2+0:	return(Location(Location::ON_EDGE,es[0]));
				case	4+0+0:	return(Location(Location::ON_EDGE,es[1]));
				case	0+2+1:	return(Location(Location::ON_VERTEX,es[0]));
				case	4+2+0:	return(Location(Location::ON_VERTEX,es[1]));
				case	4+0+1:	return(Location(Location::ON_VERTEX,root));
				default:		return(Location(Location::NOT_FOUND,Edge::null()));
				}
			}
		}
		root=Edge(base,0);
	}
	HK_ERROR(0xB8C66B5F,"Cycle detected during point location");
	return(Location(Location::CYCLE,Edge::null()));
}
//
TRIANGULATOR_HDR
inline typename TRIANGULATOR_TYP::Location				TRIANGULATOR_TYP::locateVertex(int x,int y) const
{
	return(locateVertex(Edge(getRoot(x,y),0),x,y));
}
//
TRIANGULATOR_HDR
inline typename TRIANGULATOR_TYP::Edge					TRIANGULATOR_TYP::locateExistingEdge(int x0,int y0,int x1,int y1) const
{
	Location	l0=locateVertex(x0,y0);
	Location	l1=locateVertex(x1,y1);
	if(	(l0.m_type==Location::ON_VERTEX)&&
		(l1.m_type==Location::ON_VERTEX))
	{
		return(locateExistingEdge(l0.m_edge,l0.m_edge.start(),l1.m_edge.start()));		
	}
	return(Edge::null());
}
//
TRIANGULATOR_HDR
inline typename TRIANGULATOR_TYP::Edge					TRIANGULATOR_TYP::locateExistingVertex(Edge root,const Vertex* v) const
{
	if(root.start()!=v)
	{
		root=locateVertex(root,v->m_x,v->m_y).m_edge;
		HK_ASSERT2(0x78244,root.start()==v,"Vertex does not exist");
	}
	return(root);
}
//
TRIANGULATOR_HDR
inline typename TRIANGULATOR_TYP::Edge					TRIANGULATOR_TYP::locateExistingEdge(Edge root,const Vertex* v0,const Vertex* v1) const
{
	root=locateExistingVertex(root,v0);
	if(root.end()!=v1)
	{
		if(root.start()->isBoundary())
		{
			while(!root.isNaked()) root.turnCcwInPlace();			
		}
		while(root.end()!=v1) root.turnCwInPlace();
		HK_ASSERT2(0x78754,(root.start()==v0)&&(root.end()==v1),"Edge does not exist");
	}
	return(root);
}
//
TRIANGULATOR_HDR
HK_FORCE_INLINE typename TRIANGULATOR_TYP::Edge			TRIANGULATOR_TYP::collapse(const Edge& edge)
{
	Vertex*	org=edge.start();
	Vertex*	end=edge.end();
	Edge	link=edge.link();
	if(link.isValid())
	{
		const hkBool32	c0=edge.next().getAndClearConstraint();
		const hkBool32	c1=link.prev().getAndClearConstraint();
		Edge		fan=edge;
		if(org->isBoundary())
		{
			while(!fan.isNaked()) fan.turnCcwInPlace();
		}
		Edge		base=fan;
		do 
		{
			fan.start()=end;
			fan.turnCwInPlace();
		} while(fan.isValid()&&(fan!=base));
		Edge	ea=edge.prev().link();
		Edge	eb=link.next().link();
		ea.bind(edge.next().link());
		eb.bind(link.prev().link());
		releaseTriangle(edge.triangle());
		releaseTriangle(link.triangle());
		releaseVertex(org);
		if(c0) ea.setConstrained();
		if(c1) eb.setConstrained();		
	}
	else
	{

	}
	return(Edge::null());
}
//
TRIANGULATOR_HDR
template <typename T>
inline hkBool32							TRIANGULATOR_TYP::fetchFanBoundary(Edge edge,T& boundaries)
{
	if(edge.start()->isBoundary())
	{
		Edge	last=edge;
		while(edge.isBound()) edge=edge.link().next();
		do
		{
			if(edge.isConstrained()) return(false);
			Boundary&	b	=	boundaries.expandOne();
			b.m_link		=	edge.next().link();
			b.m_vertex		=	edge.end();
			b.m_triangle	=	edge.triangle();
			b.m_constrained	=	edge.next().isConstrained() != 0;
			last			=	edge;
			edge			=	edge.prev().link();
		} while(edge.isValid());
		if(last.prev().isConstrained()) return(false);
		Boundary&	b	=	boundaries.expandOne();
		b.m_link		=	Edge::null();
		b.m_vertex		=	last.apex();
		b.m_triangle	=	HK_NULL;
		b.m_constrained	=	false;
	}
	else
	{
		Edge	first=edge;
		do
		{
			if(edge.isConstrained()) return(false);
			Boundary&	b	=	boundaries.expandOne();
			b.m_link		=	edge.next().link();
			b.m_vertex		=	edge.end();
			b.m_triangle	=	edge.triangle();
			b.m_constrained	=	edge.next().isConstrained() != 0;
			edge			=	edge.prev().link();
		} while(edge!=first);
	}
	return(true);
}
//
TRIANGULATOR_HDR
inline void								TRIANGULATOR_TYP::triangulateBoundary(Boundary* boundary)
{
	for(;;)
	{
		Boundary*	ba=boundary;
		Boundary*	bb=ba->m_next;
		Boundary*	bc=bb->m_next;
		if(bc==ba) break;
		if(orientation(ba->m_vertex,bb->m_vertex,bc->m_vertex)>0)
		{
			Boundary*	bd=bc->m_next;
			while(bd!=ba)
			{
				if(	orientation(ba->m_vertex,bb->m_vertex,bd->m_vertex)>=0	&&
					orientation(bb->m_vertex,bc->m_vertex,bd->m_vertex)>=0	&&
					orientation(bc->m_vertex,ba->m_vertex,bd->m_vertex)>=0)
				{ bd=HK_NULL;break; }
				else
				{ bd=bd->m_next; }
			}
			if(bd!=HK_NULL)
			{
				Triangle*	t=newTriangle(ba->m_vertex,bb->m_vertex,bc->m_vertex);
				Edge(t,0).bind(ba->m_link);if(ba->m_constrained) Edge(t,0).setConstrained();
				Edge(t,1).bind(bb->m_link);if(bb->m_constrained) Edge(t,1).setConstrained();
				t->link(2)			=	0;
				ba->m_link			=	Edge(t,2);
				ba->m_next			=	bc;
				ba->m_constrained	=	false;
				setRoot(t);
			}
		}
		boundary=boundary->m_next;
	}
	Edge	last(m_mesh.m_triangles.getFirst(),2);
	last.bind(boundary->m_link);
	if(boundary->m_constrained) last.setConstrained();
}
//
TRIANGULATOR_HDR
inline typename TRIANGULATOR_TYP::Insertion	TRIANGULATOR_TYP::insertVertex(int x,int y,bool conform,const Location& location,bool edgeSplit)
{
	Edge		edge(location.m_edge);
	switch(location.m_type)
	{
	case	Location::IN_TRIANGLE:
		{
			Vertex*		k=newVertex(x,y,true);
			const hkBool32	c1=edge.next().getAndClearConstraint();
			const hkBool32	c2=edge.prev().getAndClearConstraint();
			Triangle*	t0=newTriangle(edge.end(),edge.apex(),k);
			Triangle*	t1=newTriangle(edge.apex(),edge.start(),k);
			edge.apex()=k;
			copyDataset(edge.triangle(), t0);
			copyDataset(edge.triangle(), t1);
			Edge(t0,0).bind(edge.prevCcw());
			Edge(t1,0).bind(edge.prevCw());
			Edge(t0,1).bind(Edge(t1,2));
			Edge(t0,2).bind(edge.next());
			Edge(t1,1).bind(edge.prev());
			if(c1) Edge(t0,0).setConstrained();
			if(c2) Edge(t1,0).setConstrained();
			Edge			e=Edge(t0,2);
			const Vertex*	v=e.start();
			if(conform)
			{
				m_stack.pushBack(Edge(t0,0));
				m_stack.pushBack(Edge(t1,0));
				m_stack.pushBack(edge);
				flushConform();
				e=locateExistingVertex(e,v);
			}
			HK_ASSERT2(0x545451,e.start()->m_x==x && e.start()->m_y==y,"Invalid edge");
			return(Insertion(Insertion::SUCCESS,e));
		}
		break;
	case	Location::ON_EDGE:
		if(edgeSplit || !edge.isConstrained())
		{
			Vertex*			k=newVertex(x,y,true);
			const hkBool32	ce=edge.getAndClearConstraint();
			EdgeDataValue	edv[2];
			if(ce) backupBothEdgeData(edge,edv);
			if(edge.isBound())
			{
				Edge		link=edge.link();
				const hkBool32	c0=edge.prev().getAndClearConstraint();
				const hkBool32	c1=link.prev().getAndClearConstraint();
				Triangle*	t0=newTriangle(edge.apex(),edge.start(),k);
				Triangle*	t1=newTriangle(link.apex(),link.start(),k);
				copyDataset(edge.triangle(),t0);
				copyDataset(link.triangle(),t1);
				edge.start()=link.start()=k;
				Edge(t0,0).bind(edge.prevCw());
				Edge(t1,0).bind(link.prevCw());
				Edge(t0,1).bind(link);
				Edge(t1,1).bind(edge);
				Edge(t0,2).bind(edge.prev());
				Edge(t1,2).bind(link.prev());
				if(c0) Edge(t0,0).setConstrained();
				if(c1) Edge(t1,0).setConstrained();
				if(edgeSplit && ce)
				{
					Edge(t0,1).setConstrained();
					Edge(t1,1).setConstrained();
					restoreBothEdgeData(Edge(t0,1),edv);
					restoreBothEdgeData(Edge(t1,1).link(),edv);
				}
				if(conform)
				{							
					m_stack.pushBack(edge.next());
					m_stack.pushBack(link.next());
					m_stack.pushBack(Edge(t0,0));
					m_stack.pushBack(Edge(t1,0));
				}
			}
			else
			{
				const hkBool32	c0=edge.prev().getAndClearConstraint();
				Triangle*	t0=newTriangle(edge.apex(),edge.start(),k);
				copyDataset(edge.triangle(),t0);
				edge.start()=k;
				Edge(t0,0).bind(edge.prevCw());
				Edge(t0,1).bind(Edge::null());
				Edge(t0,2).bind(edge.prev());
				if(c0) Edge(t0,0).setConstrained();
				if(edgeSplit && ce)
				{
					Edge(t0,1).setConstrained();
					Edge(t0,2).link().next().setConstrained();
					restoreBothEdgeData(Edge(t0,1),edv);
					restoreBothEdgeData(Edge(t0,2).link().next(),edv);
				}
				if(conform)
				{
					m_stack.pushBack(edge.next());
					m_stack.pushBack(Edge(t0,0));
				}
			}
			if(conform)
			{
				const Vertex* v=edge.start();
				flushConform();
				edge=locateExistingVertex(edge,v);
			}
			HK_ASSERT2(0x545452,edge.start()->m_x==x && edge.start()->m_y==y,"Invalid edge");
			return(Insertion(Insertion::SUCCESS,edge));
		}
		else
		{
			return(Insertion(Insertion::ON_CONSTRAINED_EDGE,edge));
		}
		break;
	case	Location::ON_VERTEX:
		if(!edge.start()->m_constraint)
		{
			edge.start()->m_constraint=true;
			HK_ASSERT2(0x545453,edge.start()->m_x==x && edge.start()->m_y==y,"Invalid edge");
			return(Insertion(Insertion::SUCCESS,edge));
		}
		else
		{
			HK_ASSERT2(0x545454,edge.start()->m_x==x && edge.start()->m_y==y,"Invalid edge");
			return(Insertion(Insertion::ALREADY_PRESENT,edge));
		}
		break;
	default:
		break;
	}
	return(Insertion(Insertion::FAILED,Edge::null()));
}
//
TRIANGULATOR_HDR
inline typename TRIANGULATOR_TYP::Insertion	TRIANGULATOR_TYP::insertVertex(int x,int y,bool conform)
{
	HK_ASSERT2(0x9345757,(x>=m_margin)&&(x<=(cfgBoundary-m_margin))&&(y>=m_margin)&&(y<=(cfgBoundary-m_margin)),"Vertex out of domain");
	const Location	location(locateVertex(x,y));
	return(insertVertex(x,y,conform,location,false));
}
//
TRIANGULATOR_HDR
inline typename TRIANGULATOR_TYP::Insertion	TRIANGULATOR_TYP::insertVertex(const Vertex& v,bool conform)
{
	return(insertVertex(v.m_x,v.m_y,conform));
}
//
TRIANGULATOR_HDR
template <typename EDGECROSSING>
inline typename TRIANGULATOR_TYP::Insertion	TRIANGULATOR_TYP::insertVertex(int x,int y,bool conform,const EDGECROSSING& icrossing)
{
	Insertion	ins=insertVertex(x,y,conform);
	if(ins.m_type==Insertion::ON_CONSTRAINED_EDGE)
	{
		icrossing.notifyEdgeCrossing(ins.m_edge);
		Location	location;
		location.m_type	=	Location::ON_EDGE;
		location.m_edge	=	ins.m_edge;
		ins	=	insertVertex(x,y,conform,location,true);
	}
	return(ins);
}
//
TRIANGULATOR_HDR
template <typename EDGECROSSING>
inline typename TRIANGULATOR_TYP::Insertion	TRIANGULATOR_TYP::insertVertex(const Vertex& v,bool conform,const EDGECROSSING& icrossing)
{
	return(insertVertex(v.m_x,v.m_y,conform,icrossing));
}
//
TRIANGULATOR_HDR
inline typename TRIANGULATOR_TYP::Removal			TRIANGULATOR_TYP::removeVertex(int x,int y)
{
	Location	location=locateVertex(x,y);
	if(location.m_type==Location::ON_VERTEX)
	{
		Vertex*						vertex=location.m_edge.start();
		if(vertex->isCorner())
		{
			vertex->m_constraint	=	false;
			return(Removal(Removal::SUCCESS));
		}
		else
		{
			hkInplaceArray<Boundary,64>	boundaries;
			if(fetchFanBoundary(location.m_edge,boundaries))
			{			
				const int	numBoundaries=boundaries.getSize();
				for(int i=numBoundaries-1,j=0;j<numBoundaries;i=j++)
				{
					Boundary& b(boundaries[j]);
					boundaries[i].m_next=&b;
					if(b.m_triangle)
					{
						Edge(b.m_triangle,0).unbind();
						Edge(b.m_triangle,1).unbind();
						Edge(b.m_triangle,2).unbind();
						releaseTriangle(b.m_triangle);
					}
				}
				releaseVertex(vertex);
				triangulateBoundary(&boundaries[0]);
				return(Removal(Removal::SUCCESS));
			}
			else
			{
				return(Removal(Removal::BELONG_TO_CONSTRAINT));
			}
		}
	}
	return(Removal(Removal::NOT_FOUND));
}
//
TRIANGULATOR_HDR
inline typename TRIANGULATOR_TYP::Removal				TRIANGULATOR_TYP::removeVertex(const Vertex& v)
{
	return removeVertex(v.m_x,v.m_y);
}
//	
TRIANGULATOR_HDR
inline typename TRIANGULATOR_TYP::Insertion				TRIANGULATOR_TYP::insertEdge(int x0,int y0,int x1,int y1,bool conform,const EdgeData* data)
{
	/* Zero length						*/
	if((x0==x1)&&(y0==y1))				return(Insertion(Insertion::EDGE_IS_VERTEX,Edge::null()));
	/* Locate origin					*/ 
	const Location	l0=locateVertex(x0,y0);
	if(l0.m_type!=Location::ON_VERTEX)	return(Insertion(Insertion::CANNOT_LOCATE_ORIGIN,Edge::null()));
	Edge			edge=l0.m_edge;
	const Vertex*	v0=edge.start();
	/* Focus							*/ 
	while(orientation(edge,x1,y1)<0)							edge.turnCcwInPlace();		
	while(edge.isValid()&&orientation(edge.prev(),x1,y1)<=0)	edge.turnCwInPlace();
	if(!edge.isValid())					return(Insertion(Insertion::INVALID_ORIENTATION,Edge::null()));
	Edge			focal=edge.next();
	/* Already inserted					*/ 
	if(edge.end()->isEqual(x1,y1))
	{
		if(edge.isConstrained()) return(Insertion(Insertion::ALREADY_PRESENT,edge));
		edge.setConstrained();
		if(data) setEdgeData(edge,*data);
		if(conform)
		{
			edge=conformEdge(edge);
		}
		setRoot(edge.triangle());
		return(Insertion(Insertion::SUCCESS,edge));
	}
	/* Check orientation wrt. bounds	*/ 
	if(!checkOrientation(x0,y0,x1,y1))	return(Insertion(Insertion::INVALID_ORIENTATION,Edge::null()));	
	/* Walk to destination				*/ 
	while(orientation(focal,x1,y1)<0)
	{
		if(focal.isConstrained())
		{
			if(orientation(focal.start(),x0,y0,x1,y1)==0)		return(Insertion(Insertion::CROSSING_CONSTRAINED_VERTEX,focal));
			else if(orientation(focal.end(),x0,y0,x1,y1)==0)	return(Insertion(Insertion::CROSSING_CONSTRAINED_VERTEX,focal.next()));
			else												return(Insertion(Insertion::CROSSING_CONSTRAINED_EDGE,focal));
		}
		focal=focal.link();
		const int	o=orientation(v0,focal.apex(),x1,y1);
		if(o<0)
		{
			focal=focal.next();
		}
		else if(o>0)
		{
			focal=focal.prev();
		}
		else
		{
			if(!focal.apex()->isEqual(x1,y1))	return(Insertion(Insertion::CROSSING_CONSTRAINED_VERTEX,focal.prev()));
			focal=focal.prev();
		}
	}
	const Vertex*	v1=focal.start();
	if(v0==v1)							return(Insertion(Insertion::EDGE_IS_VERTEX,Edge::null()));
	if(orientation(focal,x1,y1)!=0)		return(Insertion(Insertion::CANNOT_LOCATE_DESTINATION,Edge::null()));
	if(!v1->isEqual(x1,y1))				return(Insertion(Insertion::CROSSING_CONSTRAINED_VERTEX,focal));
	if(sameBoundary(v0,v1))				return(Insertion(Insertion::INVALID_ORIENTATION,focal));
	if(orientation(edge,v1)==0)			return(Insertion(Insertion::CROSSING_CONSTRAINED_VERTEX,edge.next()));
	if(orientation(edge,v1)<0)			return(Insertion(Insertion::CROSSING_CONSTRAINED_VERTEX,focal));
	/* Insert							*/ 
	int		skippedA=0;
	while(edge.end()!=v1)
	{
		Edge	root=edge;
		edge=edge.next();
		while(orientation(v0,v1,edge.end())>0) edge.turnCcwInPlace();
		if(orientation(root.start(),edge.start(),edge.end())>0)
		{
			const Vertex*	q=edge.end();
			int				skippedB=0;
			edge=root.next();
			while(edge.end()!=q)
			{					
				if(edge.canFlip())
				{
					edge=edge.flip();
					if(skippedB)
					{
						edge=edge.nextCcw().link();
						--skippedB;
					}
					else
					{
						edge=edge.nextCw();
					}
				}
				else
				{
					++skippedB;
					edge.turnCcwInPlace();						
				}
			}
			edge=edge.prevCcw();
			if(skippedA)
			{
				while(orientation(v0,v1,edge.apex())>0) edge.turnCwInPlace();
				edge=edge.prev();
				--skippedA;
			}
		}
		else
		{
			++skippedA;
		}
	}
	
	HK_ASSERT2(0x6cc64e18, (edge.start() == v0) && (edge.end()==v1), "Edge insertion internal error");
	edge.setConstrained();
	if(data) setEdgeData(edge,*data);
	if(conform)
	{
		edge=conformEdge(edge);
	}
	setRoot(edge.triangle());
	return(Insertion(Insertion::SUCCESS,edge));
}
//
TRIANGULATOR_HDR
inline typename TRIANGULATOR_TYP::Insertion				TRIANGULATOR_TYP::insertEdge(const Vertex& v0,const Vertex& v1,bool conform,const EdgeData* data)
{
	return(insertEdge(v0.m_x,v0.m_y,v1.m_x,v1.m_y,conform,data));
}
//	
TRIANGULATOR_HDR
inline typename TRIANGULATOR_TYP::Insertion				TRIANGULATOR_TYP::insertEdge(int x0,int y0,int x1,int y1,bool conform,const EdgeData& data)
{
	return(insertEdge(x0,y0,x1,y1,conform,&data));
}
//
TRIANGULATOR_HDR
inline typename TRIANGULATOR_TYP::Insertion				TRIANGULATOR_TYP::insertEdge(const Vertex& v0,const Vertex& v1,bool conform,const EdgeData& data)
{
	return(insertEdge(v0,v1,conform,&data));
}
//	
TRIANGULATOR_HDR
template <typename IEDGECROSSING>
inline typename TRIANGULATOR_TYP::Insertion				TRIANGULATOR_TYP::insertCrossingEdge(int x0,int y0,int x1,int y1,bool conform, const IEDGECROSSING& icrossing, const EdgeData* data)
{
	#define CANNOT_RESOLVE_CROSSING HK_ERROR(0x9CABCB60,"Cannot insert crossing edge");
	for(;;)
	{
		Insertion	ins=insertEdge(x0,y0,x1,y1,conform,data);
		Edge		edge=ins.m_edge;
		switch(ins.m_type)
		{
			// Existing edge
		case	Insertion::ALREADY_PRESENT:
			if(data)
			{
				setEdgeData(ins.m_edge,*data);
			}
			break;
			// Crossing edge
		case	Insertion::CROSSING_CONSTRAINED_EDGE:
			{
				Vertex*	a		=	edge.start();
				Vertex*	b		=	edge.end();
				icrossing.notifyEdgeCrossing(edge);
				const hkInt32	x2		=	a->m_x;
				const hkInt32	y2		=	a->m_y;
				const hkInt32	x3		=	b->m_x;
				const hkInt32	y3		=	b->m_y;
				const hkInt32	num		=	x3*(y0-y2)+x0*(y2-y3)+x2*(y3-y0);
				const hkInt32	den		=	-(x2-x3)*(y0-y1)+(x0-x1)*(y2-y3);
				const hkReal	sol		=	num/(hkReal)den;
				const hkReal	ifx		=	x0+(x1-x0)*sol;
				const hkReal	ify		=	y0+(y1-y0)*sol;
				hkInt32			ix		=	(hkInt32)ifx;
				hkInt32			iy		=	(hkInt32)ify;
				/* Search a valid insertion point using concentric circles	*/ 
				hkBool32		split	=	canSplitEdge(ix,iy,edge);
				
				/* Try finding an insertion point around ix,iy			*/ 
				if(!split)
				{
					for(int r=1;!split && r<cfgSearchRadius;++r)
					{
						hkInt32 x(r),y(0);
						hkInt32 dx(1-2*r),dy(1),err(-1);
						while (x >= y)
						{
							hkInt32 kx,ky;
//#define CHECK_CAN_SPLIT() if(true==(split=canSplitEdge(kx,ky,edge))) { ix=kx;iy=ky;break; }
#define CHECK_CAN_SPLIT() split=canSplitEdge(kx,ky,edge); if(split) { ix=kx;iy=ky;break; }
							kx=ix+x;ky=iy+y; CHECK_CAN_SPLIT()
							kx=ix-x;ky=iy+y; CHECK_CAN_SPLIT()
							kx=ix-x;ky=iy-y; CHECK_CAN_SPLIT()
							kx=ix+x;ky=iy-y; CHECK_CAN_SPLIT()
							kx=ix+y;ky=iy+x; CHECK_CAN_SPLIT()
							kx=ix-y;ky=iy+x; CHECK_CAN_SPLIT()
							kx=ix-y;ky=iy-x; CHECK_CAN_SPLIT()
							kx=ix+y;ky=iy-x; CHECK_CAN_SPLIT()
#undef CHECK_CAN_SPLIT
							y+=1;err+=dy;dy+=2;
							if(2*err+dx > 0) { x-=1;err+=dx;dx+=2; }
						}
					}
				}
				
				/* Try finding an insertion point along the edge		*/ 
				if(!split)
				{
					hkInt32		dx=x2-x3;dx=dx<0?-dx:dx;
					hkInt32		dy=y2-y3;dy=dy<0?-dy:dy;
					hkVector4	endPoints[2];
					hkVector4	isecPoint;
					hkReal		nearest=HK_REAL_MAX;
					endPoints[0].set((hkReal)x2,(hkReal)y2,0,0);
					endPoints[1].set((hkReal)x3,(hkReal)y3,0,0);
					isecPoint.set(ifx,ify,0,0);
					for(int i=1,ni=4+hkMath::max2(dx,dy)*8;i<ni;++i)
					{
						const hkReal fraction(i/(hkReal)ni);
						for(int s=0;s<2;++s)
						{
							hkVector4	isec;
							isec.setInterpolate4(isecPoint,endPoints[s],fraction);
							hkReal		sqd=isec.distanceToSquared3(isecPoint);
							if(!split || sqd<nearest)
							{
								hkInt32		tx,ty;
								tx		=	(hkInt32)(isec(0)+0.5f);
								ty		=	(hkInt32)(isec(1)+0.5f);
								if((tx==x2 && ty==y2) || (tx==x3 && ty==y3)) continue;
								split=canSplitEdge(tx,ty,edge);
								if( split )
								{
									nearest	=	sqd;
									ix		=	tx;
									iy		=	ty;
								}
							}
						}
						if(split) break;
					}
				}				
				/* Insert at ix,iy if found an insertion point				*/ 
				if(split)
				{
					const hkReal	dx((hkReal)(x3-x2));
					const hkReal	dy((hkReal)(y3-y2));
					hkReal			u(((ix-x2)*dx+(iy-y2)*dy)/(dx*dx+dy*dy));
					if(u<=0)	u=HK_REAL_EPSILON;
					if(u>=1)	u=1-HK_REAL_EPSILON;
					Location		location;
					location.m_edge	=	edge;
					location.m_type	=	Location::ON_EDGE;
					ins				=	insertVertex(ix,iy,false,location,true);
					if(ins.m_type!=Insertion::SUCCESS) CANNOT_RESOLVE_CROSSING
					Edge			insertedEdge;
					icrossing.notifyNewVertex(a,b,u,sol,ins.m_edge.start());
					ins				=	insertCrossingEdge(x0,y0,ix,iy,conform,icrossing,data);
					if(ins.m_type!=Insertion::ALREADY_PRESENT && ins.m_type!=Insertion::SUCCESS)
					{
						CANNOT_RESOLVE_CROSSING
					}
					x0=ix;y0=iy;
				}
				/* Else, snap to the nearest edge's vertex					*/ 
				else
				{
					const hkInt32	o0=orientation(x0,y0,x2,y2,x1,y1);
					const hkInt32	o1=orientation(x1,y1,x3,y3,x0,y0);
					if(o0 < o1)
					{
						if(!checkOrientation(x0,y0,x2,y2)) CANNOT_RESOLVE_CROSSING
						ins=insertCrossingEdge(x0,y0,x2,y2,conform,icrossing,data);
						x0=x2;y0=y2;
					}
					else
					{
						if(!checkOrientation(x0,y0,x3,y3)) CANNOT_RESOLVE_CROSSING
						ins=insertCrossingEdge(x0,y0,x3,y3,conform,icrossing,data);
						x0=x3;y0=y3;
					}
					if(ins.m_type!=Insertion::ALREADY_PRESENT && ins.m_type!=Insertion::SUCCESS)
					{
						CANNOT_RESOLVE_CROSSING
					}
				}
			}
			continue;
			// Crossing vertex
		case	Insertion::CROSSING_CONSTRAINED_VERTEX:
			{
				/* Start over with edge->start() as x0,y0	*/ 
				icrossing.notifyVertexCrossing(edge.start());
				const hkInt32	nx=ins.m_edge.start()->m_x;
				const hkInt32	ny=ins.m_edge.start()->m_y;
				HK_ASSERT2(0x4424,orientation(x0,y0,nx,ny,x1,y1)==0,"Invalid insertion result");
				ins=insertCrossingEdge(x0,y0,nx,ny,conform,icrossing,data);
				if(ins.m_type!=Insertion::ALREADY_PRESENT && ins.m_type!=Insertion::SUCCESS)
				{
					CANNOT_RESOLVE_CROSSING
				}
				x0	=	nx;
				y0	=	ny;
			}
			continue;
		default:
			break;
		}
		return(ins);
	}	
	#undef CANNOT_RESOLVE_CROSSING
}
//
TRIANGULATOR_HDR
template <typename IEDGECROSSING>
inline typename TRIANGULATOR_TYP::Insertion				TRIANGULATOR_TYP::insertCrossingEdge(const Vertex& v0,const Vertex& v1,bool conform, const IEDGECROSSING& icrossing, const EdgeData* data)
{
	return(insertCrossingEdge(v0.m_x,v0.m_y,v1.m_x,v1.m_y,conform,icrossing,data));
}
//	
TRIANGULATOR_HDR
template <typename IEDGECROSSING>
inline typename TRIANGULATOR_TYP::Insertion				TRIANGULATOR_TYP::insertCrossingEdge(int x0,int y0,int x1,int y1,bool conform, const IEDGECROSSING& icrossing, const EdgeData& data)
{
	return(insertCrossingEdge(x0,y0,x1,y1,conform,icrossing,&data));
}
//
TRIANGULATOR_HDR
template <typename IEDGECROSSING>
inline typename TRIANGULATOR_TYP::Insertion				TRIANGULATOR_TYP::insertCrossingEdge(const Vertex& v0,const Vertex& v1,bool conform, const IEDGECROSSING& icrossing, const EdgeData& data)
{
	return(insertCrossingEdge(v0.m_x,v0.m_y,v1.m_x,v1.m_y,conform,icrossing,&data));
}
//
TRIANGULATOR_HDR
inline int												TRIANGULATOR_TYP::partition()
{
	static const hkUint32	nullregion=(1<<12)-1;
	/* Reset regions	*/ 
	for(Triangle* t=m_mesh.m_triangles.getFirst();t;t=t->next())
	{
		t->m_region=nullregion;
		HK_ASSERT2(0x4476723,nullregion==t->m_region,"Invalid null region id");
	}
	/* Flood regions	*/ 
	hkInplaceArray<Edge,cfgStacksize,ALLOCATOR>	stack2;
	hkInplaceArray<Edge,cfgStacksize,ALLOCATOR>*	current=&m_stack;
	hkInplaceArray<Edge,cfgStacksize,ALLOCATOR>*	next=&stack2;
	int									region=-1;
	next->pushBack(locateVertex(0,0).m_edge);
	do
	{
		hkAlgorithm::swap(next,current);
		next->clear();
		++region;
		do 
		{
			Edge		e=current->back();
			Triangle*	t=e.triangle();
			current->popBack();
			if(t->m_region==nullregion)
			{
				t->m_region=region;
				for(int i=0;i<3;++i)
				{
					Edge		ne=Edge(t,i).link();
					Triangle*	nt=ne.triangle();
					if(ne.isValid()&&(nt->m_region==nullregion))
					{
						if(ne.isConstrained())
							next->pushBack(ne);
						else
							current->pushBack(ne);
					}
				}
			}						
		} while (current->getSize()>0);
	} while(next->getSize()>0);
	return(region+1);
}
//
TRIANGULATOR_HDR
inline void												TRIANGULATOR_TYP::removeNonBoundaryConstrainedEdgeAndData()
{
	for(typename Mesh::EdgeIterator i=m_mesh.getEdgeIterator();i;++i)
	{
		Edge	edge(i());
		if(	edge.isConstrained() &&
			edge.isBound() &&
			edge.triangle()->m_region==edge.link().triangle()->m_region)
		{
			clearEdgeData(edge);
			clearEdgeData(edge.link());
			edge.clrConstrained();
		}
	}
}
//
TRIANGULATOR_HDR
inline void												TRIANGULATOR_TYP::clearTrianglesSets(tSets value)
{
	for(Triangle* t=m_mesh.m_triangles.getFirst();t;t=t->next()) t->m_sets=value;
}
//
TRIANGULATOR_HDR
template <typename FLOODPOLICY>
inline void												TRIANGULATOR_TYP::floodEdgeSets(const FLOODPOLICY& policy)
{
	/* Partition	*/ 
	partition();
	/* Initialize	*/ 
	hkInplaceArray<EdgeSet,cfgStacksize>	stack;
	for(Triangle* t=m_mesh.m_triangles.getFirst();t;t=t->next())
	{
		for(int i=0;i<3;++i)
		{
			Edge	e(t,i);
			if(e.isConstrained())
			{
				const EdgeData*	d(getEdgeData(e));
				if(d)
				{
					const tSets	sets=policy.getSetsFromEdgeData(d);
					if(sets!=0)
					{
						if(e.isNaked() || e.triangle()->m_region>e.link().triangle()->m_region)
						{
							for(int j=0,nj=sizeof(tSets)*8, bit = 1<<0; j<nj; ++j, bit<<=1)
							{
								if(sets&bit) stack.pushBack(EdgeSet(e,(tSets)bit));
							}
						}
					}					
				}
			}
		}
	}
	/* Flood		*/ 
	while(stack.getSize())
	{
		EdgeSet	es=stack.back();stack.popBack();
		if(!(es.m_edge.triangle()->m_sets&es.m_set))
		{
			es.m_edge.triangle()->m_sets |= es.m_set;
			for(int i=0;i<2;++i)
			{
				es.m_edge=es.m_edge.next();
				if(es.m_edge.isBound())
				{
					hkBool32	allowCrossing=!es.m_edge.isConstrained();
					if(!allowCrossing)
					{
						const EdgeData*	ed=getEdgeData(es.m_edge);
						if(!ed || !(policy.getSetsFromEdgeData(ed)&es.m_set))
						{
							if(es.m_edge.triangle()->m_region<es.m_edge.link().triangle()->m_region)
							{
								allowCrossing=true;
							}
						}
					}
					if(allowCrossing)
					{
						stack.pushBack(EdgeSet(es.m_edge.link(),es.m_set));
					}
				}
			}
		}

	}
}
//
TRIANGULATOR_HDR
inline void												TRIANGULATOR_TYP::conformEdges()
{
	bool	found;
	do
	{
		found=false;
		for(Triangle* t=m_mesh.m_triangles.getFirst();t;t=t->next())
		{
			for(int i=0;i<3;++i)
			{
				Edge	e(t,i);
				if(!isConform(e) && e.canFlip())
				{
					e=e.flip();
					found=true;
				}
			}
		}
	} while(found);
}
//
TRIANGULATOR_HDR
inline void												TRIANGULATOR_TYP::minimizeEdgeLengths()
{
	bool	found;
	do
	{
		found=false;
		for(Triangle* t=m_mesh.m_triangles.getFirst();t;t=t->next())
		{
			for(int i=0;i<3;++i)
			{
				Edge	e(t,i);
				if(e.canFlip())
				{
					if(	distanceSqrd(e.start(),e.end())>
						distanceSqrd(e.apex(),e.link().apex()))
					{
						e=e.flip();
						found=true;
					}
				}
			}
		}
	} while(found);
}

//
TRIANGULATOR_HDR
inline int												TRIANGULATOR_TYP::decompose(hkArray<hkArray<Edge,ALLOCATOR>,ALLOCATOR>& pieces) const
{
	hkPointerMap<Triangle*,hkUint8>	toSkip;
	hkArray<Triangle*>				triangles;
	toSkip.reserve(1+m_mesh.m_triangles.getSize()*4);
	triangles.reserve(m_mesh.m_triangles.getSize());
	for(Triangle* t=m_mesh.m_triangles.getFirst();t;t=t->next())
	{
		triangles.pushBackUnchecked(t);
	}
	hkAlgorithm::quickSort(&triangles[0],triangles.getSize(),SortByArea_Predicate());
	for(int triangleIndex=0;triangleIndex<triangles.getSize();++triangleIndex)
	{
		Triangle*	t=triangles[triangleIndex];
		if(toSkip.getWithDefault(t,0)==0)
		{
			const hkUint32	region=t->m_region;
			hkArray<Edge,ALLOCATOR>&	edges=pieces.expandOne();
			edges.pushBack(Edge(t,0));
			edges.pushBack(Edge(t,1));
			edges.pushBack(Edge(t,2));
			toSkip.insert(t,1);
			for(;;)
			{
				int		bestEdge=-1;
				hkInt32	bestLength=0;
				for(int i=0,ni=edges.getSize();i<ni;++i)
				{
					Edge		l=edges[i].link();
					Triangle*	lt=l.triangle();
					if(l.isValid() && lt->m_region==region && toSkip.getWithDefault(lt,0)==0)
					{
						const hkInt32	dst=distanceSqrd(l.start(),l.end());
						if(bestEdge==-1 || dst>bestLength)
						{
							Edge	p=edges[(i+ni-1)%ni];
							Edge	n=edges[(i+1)%ni];
							if(	(orientation(p,l.apex())>=0) &&
								(orientation(n,l.apex())>=0))
							{
								bestEdge	=	i;
								bestLength	=	dst;
							}
						}
					}
				}
				if(bestEdge>=0)
				{
					Edge	l=edges[bestEdge].link();
					edges[bestEdge]=l.prev();
					edges.insertAt(bestEdge,l.next());
					toSkip.insert(l.triangle(),1);
				} else break;
			}
		}
	}
	return(pieces.getSize());
}

//
TRIANGULATOR_HDR
inline int												TRIANGULATOR_TYP::decomposeFast(hkArray<hkArray<Edge,ALLOCATOR>,ALLOCATOR>& pieces) const
{
	hkPointerMap<Triangle*,hkUint8,ALLOCATOR>	toSkip;
	hkArray<Triangle*,ALLOCATOR>				triangles;
	toSkip.reserve(1+m_mesh.m_triangles.getSize()*4);
	for(Triangle* t=m_mesh.m_triangles.getFirst();t;t=t->next())
	{
		if(toSkip.getWithDefault(t,0)==0)
		{
			const hkUint32	region=t->m_region;
			hkArray<Edge,ALLOCATOR>&	edges=pieces.expandOne();
			bool			changed=true;
			{
				Edge* edgePtr = edges.expandBy(3);
				new (edgePtr+0) Edge(t,0);
				new (edgePtr+1) Edge(t,1);
				new (edgePtr+2) Edge(t,2);
			}
			toSkip.insert(t,1);
			do 
			{
				changed=false;
				for(int i=0,ni=edges.getSize();i<ni;++i)
				{
					Edge		l=edges[i].link();
					Triangle*	lt=l.triangle();
					if(l.isValid() && lt->m_region==region && toSkip.getWithDefault(lt,0)==0)
					{						
						Edge	p=edges[(i+ni-1)%ni];
						Edge	n=edges[(i+1)%ni];
						if(	(orientation(p,l.apex())>=0) &&
							(orientation(n,l.apex())>=0))
						{
							++ni;
							changed=true;
							edges[i]=l.prev();
							edges.insertAt(i--,l.next());
							toSkip.insert(lt,1);
						}
					}
				}
			} while (changed);
		}
	}
	return(pieces.getSize());
}

//
TRIANGULATOR_HDR
inline void												TRIANGULATOR_TYP::flushConform()
{		
	do
	{
		Edge edge=m_stack.back();
		m_stack.popBack();
		if(!isConform(edge))
		{
			if(edge.canFlip())
			{
				edge=edge.flip();
				m_stack.pushBack(edge.next());
				m_stack.pushBack(edge.nextCw());
		}
		}
	} while(m_stack.getSize()>0);
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
