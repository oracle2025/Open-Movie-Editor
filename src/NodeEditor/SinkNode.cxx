
#include "SinkNode.H"

SinkNode::SinkNode()
{
}
void SinkNode::setInput( int input, INode* node )
{
}
uint32_t* SinkNode::getFrame( int output, double position )
{
	if ( node->inputs[0] && node->inputs[0]->node ) {
		return node->inputs[0]->node->getFrame( 0, position );
	}
	return 0;
}
