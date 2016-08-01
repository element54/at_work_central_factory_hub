#*****************************************************************************
#              Makefile Build System for Fawkes: Utility Library
#                            -------------------
#   Created on Sun Sep 03 14:14:14 2006
#   Copyright (C) 2006-2008 by Tim Niemueller, AllemaniACs RoboCup Team
#
#*****************************************************************************
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.
#
#*****************************************************************************

BASEDIR = ../../..
include $(BASEDIR)/etc/buildsys/config.mk
include $(BUILDSYSDIR)/boost.mk
include $(BUILDCONFDIR)/netcomm/netcomm.mk

REQ_BOOST_LIBS = thread system asio
HAVE_BOOST_LIBS = $(call boost-have-libs,$(REQ_BOOST_LIBS))


LIBS_libproperty_client = stdc++ llsfrbcore llsfrbnetcomm crypto ssl cpprest
OBJS_libproperty_client = $(patsubst %.cpp,%.o,$(patsubst qa/%,,$(subst $(SRCDIR)/,,$(realpath $(wildcard $(SRCDIR)/*.cpp $(SRCDIR)/*/*.cpp)))))
HDRS_libproperty_client = $(subst $(SRCDIR)/,,$(wildcard $(SRCDIR)/*.h $(SRCDIR)/*/*.h))

OBJS_all = $(OBJS_libproperty_client)

LIBS_all  = $(LIBDIR)/libproperty_client.$(SOEXT)

CFLAGS += $(CFLAGS_CPP11) $(call boost-libs-cflags,$(REQ_BOOST_LIBS))
LDFLAGS += $(call boost-libs-ldflags,$(REQ_BOOST_LIBS))

ifeq ($(OBJSSUBMAKE),1)
all: $(ERROR_TARGETS)
endif

include $(BUILDSYSDIR)/base.mk
