# Copyright (C) 2005 Richard Spindler <richard.spindler AT gmail.com>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

noinst_LIBRARIES = libErrorDialog.a
EXTRA_DIST = ErrorDialog.fl
AM_CPPFLAGS = -I$(top_srcdir)/src -I$(top_srcdir)/src/sl @CPPFLAGS@
libErrorDialog_a_SOURCES = \
	ErrorDialog.cxx          \
	FltkErrorHandler.cxx     \
	IErrorHandler.cxx        \
	ErrorDialog.H            \
	FltkErrorHandler.H       \
	IErrorHandler.H

