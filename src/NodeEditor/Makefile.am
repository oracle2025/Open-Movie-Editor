# Copyright (C) 2008 Richard Spindler <richard.spindler AT gmail.com>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

noinst_LIBRARIES = libNodeEditor.a
EXTRA_DIST = NodeFilterDialog.fl
AM_CPPFLAGS = -ggdb -I$(top_srcdir)/src -I$(top_srcdir)/src/sl -I$(top_srcdir)/src/tinyxml @CPPFLAGS@
libNodeEditor_a_SOURCES = \
BezierCurveNode.cxx     Frei0rNode.cxx                          NodeFilterDialog.cxx               NodeFilterImageFactoryPlugin.cxx   \
BezierCurveNode.H       Frei0rNode.H                            NodeFilterDialog.fl                NodeFilterImageFactoryPlugin.H     \
Frei0rBoolButton.cxx    ImageNode.cxx                           NodeFilterDialog.H                 NodeFilterPreviewFactoryPlugin.cxx \
Frei0rBoolButton.H      ImageNode.H                             NodeFilterFactory.cxx              NodeFilterPreviewFactoryPlugin.H   \
Frei0rColorButton.cxx   INodeFilterFactoryPlugin.H              NodeFilterFactory.H                SinkNode.cxx                       \
Frei0rColorButton.H     INode.H                                 NodeFilterFrei0rFactory.cxx        SinkNode.H                         \
Frei0rDoubleSlider.cxx  Makefile.am                             NodeFilterFrei0rFactory.H          SrcNode.cxx                        \
Frei0rDoubleSlider.H    NodeFilterBezierCurveFactoryPlugin.cxx  NodeFilterFrei0rFactoryPlugin.cxx  SrcNode.H                          \
Frei0rGraphEditor.cxx   NodeFilterBezierCurveFactoryPlugin.H    NodeFilterFrei0rFactoryPlugin.H    PreviewNode.cxx                    \
Frei0rGraphEditor.H     NodeFilter.cxx                          NodeFilter.H                       PreviewNode.H                      \
CurveEditorBezier.cxx                                                                                                                 \
CurveEditorBezier.H

