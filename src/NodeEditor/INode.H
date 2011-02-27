#ifndef _I_NODE_H_
#define _I_NODE_H_

#include "Frei0rGraphEditor.H"

class INode
{
	public:
		virtual ~INode() {}
		virtual void setInput( int input, INode* node ) = 0;
		virtual int getInputCount() = 0;
		virtual int getOutputCount() = 0;
		virtual void init_widgets() {};
		virtual void delete_widgets() {};
		virtual uint32_t* getFrame( int output, double position ) = 0;
		virtual void init() {}
		virtual const char* identifier() = 0;
		virtual void readXML( TiXmlElement* xml_node ) {}
		virtual void writeXML( TiXmlElement* xml_node ) {}
		filters* node;

};


#endif /* _I_NODE_H_ */
