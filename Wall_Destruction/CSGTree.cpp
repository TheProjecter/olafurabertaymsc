#include "CSGTree.h"

CSGTree::CSGTree(void)
{
}

CSGTree::~CSGTree(void)
{

}

OPERATOR_NODE* CSGTree::CreateNode(Volume *Left, Volume *Right, OPERATOR_NODE *NLeft, OPERATOR_NODE *NRight, OPERATOR_TYPE type){
	OPERATOR_NODE *node = new OPERATOR_NODE;
	node->Left = Left;
	node->NLeft = NLeft;
	node->Right= Right;
	node->NRight = NRight;
	node->type = type;
	return node;
}

OPERATOR_NODE* CSGTree::And( Volume *Left, Volume *Right )
{
	return CreateNode(Left, Right, NULL, NULL, AND);
}

OPERATOR_NODE* CSGTree::And( Volume *Left, OPERATOR_NODE *Right )
{
	return CreateNode(Left, NULL, NULL, Right, AND);
}

OPERATOR_NODE* CSGTree::And( OPERATOR_NODE *Left, Volume *Right )
{
	return CreateNode(NULL, Right, Left, NULL, AND);
}

OPERATOR_NODE* CSGTree::And( OPERATOR_NODE *Left, OPERATOR_NODE *Right )
{
	return CreateNode(NULL, NULL, Left, Right, AND);
}

OPERATOR_NODE* CSGTree::Or( Volume *Left, Volume *Right )
{
	return CreateNode(Left, Right, NULL, NULL, OR);
}

OPERATOR_NODE* CSGTree::Or( Volume *Left, OPERATOR_NODE *Right )
{
	return CreateNode(Left, NULL, NULL, Right, OR);
}

OPERATOR_NODE* CSGTree::Or( OPERATOR_NODE *Left, Volume *Right )
{
	return CreateNode(NULL, Right, Left, NULL, OR);
}

OPERATOR_NODE* CSGTree::Or( OPERATOR_NODE *Left, OPERATOR_NODE *Right )
{
	return CreateNode(NULL, NULL, Left, Right, OR);
}

Volume* CSGTree::Compute()
{
	return Compute(Root);
}

Volume* CSGTree::Compute( OPERATOR_NODE *node )
{
	/*Volume *Volume;
	if(node->NLeft){
		if(node->NRight)
			Compute(node->NLeft);
		else if(node->Right)

	}
	else if(node->NRight){
		if(node->Left)
			Compute(node->NRight);
	}*/	
	
	return NULL;
}

void CSGTree::Draw(){
	DrawNode(this->Root);
}

void CSGTree::DrawNode(OPERATOR_NODE* node){

	if(node->NLeft)
		DrawNode(node->NLeft);
	if(node->NRight)
		DrawNode(node->NRight);
	if(node->Left)
		node->Left->Draw();
	if(node->Right)
		node->Right->Draw();
}

void CSGTree::CleanUp()
{
	CleanUpNode(Root);
}

void CSGTree::CleanUpNode(OPERATOR_NODE* node){

	if(node->NLeft)
		CleanUpNode(node->NLeft);
	if(node->NRight)
		CleanUpNode(node->NRight);
	if(node->Left)
		delete node->Left;
	if(node->Right)
		delete node->Right;

	delete node;
}