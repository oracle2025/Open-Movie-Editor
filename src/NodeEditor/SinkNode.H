
#ifndef _SINK_NODE_H_
#define _SINK_NODE_H_

#include "INode.H"

class SinkNode : public INode
{
	public:
		SinkNode();
		~SinkNode() {}
		void setInput( int input, INode* node );
		int getInputCount() { return 1; }
		int getOutputCount() { return 0; }
		uint32_t* getFrame( int output, double position );
		const char* identifier() { return "effect:builtin:SinkNode"; }
};



#endif /* _SINK_NODE_H_ */
