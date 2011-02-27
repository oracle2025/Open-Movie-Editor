
#ifndef _SRC_NODE_H_
#define _SRC_NODE_H_

#include "INode.H"

class nle::NodeFilter;
class SrcNode : public INode
{
	public:
		SrcNode( nle::NodeFilter* node_filter );
		~SrcNode() {}
		void setInput( int input, INode* node );
		int getInputCount() { return 0; }
		int getOutputCount() { return 1; }
		uint32_t* getFrame( int output, double position );
		const char* identifier() { return "effect:builtin:SrcNode"; }
	private:
		nle::NodeFilter* m_node_filter;
};



#endif /* _SRC_NODE_H_ */
