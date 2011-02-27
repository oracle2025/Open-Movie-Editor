
#include "SrcNode.H"

SrcNode::SrcNode( nle::NodeFilter* node_filter )
{
	m_node_filter = node_filter;
}
void SrcNode::setInput( int input, INode* node )
{
}
uint32_t* SrcNode::getFrame( int output, double position )
{
	return m_node_filter->m_frame_cache;
}
