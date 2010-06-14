/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2009 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HKBASE_STACKTRACER_H
#define HKBASE_STACKTRACER_H

/// An object which can generate stack traces.
/// Some platforms may also be able to associate addresses to 
/// function and source file information.
class hkStackTracer : public hkReferencedObject, public hkSingleton<hkStackTracer>
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE_CLASS);

			/// Stores multiple callstacks in a tree to save space.
			/// This works by merging trees towards their roots.
		class CallTree
		{
			public:

				struct Node
				{
					Node() : m_value(0), m_parent(-1), m_firstChild(-1), m_next(-1) {}
					hkUlong m_value;
					int m_parent;
					int m_firstChild;
					int m_next;
				};

				typedef int TraceId;

				CallTree() : m_allocator(HK_NULL) {}

				void init( hkMemoryAllocator* a ) { m_allocator = a; }
				void quit() { m_nodes._clearAndDeallocate(*m_allocator); m_allocator = HK_NULL; }

					/// Get a stack trace and insert it into the given tree.
				TraceId insertCallStack( hkStackTracer& tracer );

					/// Add a callstack to the tree and get its id.
				TraceId insertCallStack( const hkUlong* addrs, int numAddrs );

					/// Retrieve a callstack from an id.
				int getCallStack( TraceId id, hkUlong* addrs, int maxAddrs ) const;

					/// Read only access for traversal
				const Node& node(int i) const { return m_nodes[i]; }

			protected:

				hkArrayBase<Node> m_nodes;
				hkMemoryAllocator* m_allocator;
		};

		~hkStackTracer();

		typedef void (HK_CALL *printFunc)(const char*, void* context);

		static hkStackTracer* HK_CALL create();

			/// Print the stack trace with pfunc.
			/// pfunc is called may be called multiple times.
			/// The output format is platform specific.
		void dumpStackTrace( const hkUlong* trace, int numtrace, printFunc pfunc, void* context=HK_NULL ) const;

			/// Write at most maxtrace stack entries into 'trace'.
			/// Return the number of entries written.
		int getStackTrace( hkUlong* trace, int maxtrace );

			/// If you dynamically load DLLs then you will need to refresh the symbols 
			/// so that the stack tracer can see them
		void refreshSymbols();

	protected:

		hkStackTracer();

		void* m_impl;
};

#endif // HKBASE_STACKTRACER_H

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
