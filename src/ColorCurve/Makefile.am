# Copyright (C) 2008 Richard Spindler <richard.spindler AT gmail.com>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

noinst_LIBRARIES = libColorCurve.a
EXTRA_DIST = ColorGrader2.fl
AM_CPPFLAGS = -ggdb -I$(top_srcdir)/src -I$(top_srcdir)/src/sl -I$(top_srcdir)/src/tinyxml @CPPFLAGS@
libColorCurve_a_SOURCES = \
ColorCurveDialog.cxx  ColorCurveFactory.cxx  ColorCurveFilter.cxx  CurveEditor2.cxx \
ColorCurveDialog.H    ColorCurveFactory.H    ColorCurveFilter.H    CurveEditor2.H   \
ColorGrader2.cxx ColorGrader2.h
