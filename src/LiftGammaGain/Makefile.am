# Copyright (C) 2008 Richard Spindler <richard.spindler AT gmail.com>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

noinst_LIBRARIES = libLiftGammaGain.a
EXTRA_DIST = LiftGammaGainWidget.fl
AM_CPPFLAGS = -ggdb -I$(top_srcdir)/src -I$(top_srcdir)/src/sl -I$(top_srcdir)/src/tinyxml @CPPFLAGS@
libLiftGammaGain_a_SOURCES = \
LiftGammaGainDialog.cxx  LiftGammaGainFactory.cxx  LiftGammaGainFilter.cxx  LiftGammaGainWidget.cxx  LiftGammaGainWidget.H \
LiftGammaGainDialog.H    LiftGammaGainFactory.H    LiftGammaGainFilter.H    Fl_Hsv_Color_Chooser.cxx Fl_Hsv_Color_Chooser.H

