#ifndef CSGTREE_H
#define CSGTREE_H

#include "Volume.h"

enum OPERATOR_TYPE {AND, OR};

/*	CSG Tree:
				N
			   / \
			  /   \
			 N     N
		    / \   / \
           /   \ V   N
          V     V   / \
			       V   V
V = Volume
N = Operator Node
*/

struct OPERATOR_NODE{
	OPERATOR_TYPE type;
	
	// if left or right is an in-tree operator
	OPERATOR_NODE *NLeft;
	OPERATOR_NODE *NRight;

	// If left or right is a leave
	Volume *Left;
	Volume *Right;
};

class CSGTree
{
public:
	CSGTree(void);
	~CSGTree(void);

	void CreateTree(OPERATOR_NODE* node){
		this->Root = node;
	}

	Volume* GetComputedVolume();

	// and's
	OPERATOR_NODE* And(Volume *Left, Volume *Right);
	OPERATOR_NODE* And(Volume *Left, OPERATOR_NODE *Right);
	OPERATOR_NODE* And(OPERATOR_NODE *Left, Volume *Right);
	OPERATOR_NODE* And( OPERATOR_NODE *Left, OPERATOR_NODE *Right );
	// or's
	OPERATOR_NODE* Or(Volume *Left, Volume *Right);
	OPERATOR_NODE* Or(Volume *Left, OPERATOR_NODE *Right);
	OPERATOR_NODE* Or(OPERATOR_NODE *Left, Volume *Right);
	OPERATOR_NODE* Or(OPERATOR_NODE *Left, OPERATOR_NODE *Right );
	// Get the surface of the object
	Volume* Compute();

	void CleanUp();

private:

	OPERATOR_NODE *Root;
	void CleanUpNode(OPERATOR_NODE* node);
	Volume *volume;

	OPERATOR_NODE* CreateNode(Volume *Left, Volume *Right, OPERATOR_NODE *NLeft, OPERATOR_NODE *NRight, OPERATOR_TYPE type);
	// compute the surface from a operator node
	Volume* Compute(OPERATOR_NODE *N);
};

#endif