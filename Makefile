# -*- Makefile -*-

LIBDIR = lib
LIBSORTING = $(LIBDIR)/libsorting.a
SRCS    = user_sort.cpp

########################################################################
########################################################################
########################################################################

# comment the following if you do not have zlib, and edit $(LIBDIR)/Makefile
LIBS           += -lz

INCLUDES       = -I$(LIBDIR)
RLIBS          = $(shell root-config --ldflags --libs) # for ROOT
RFLAGS         = $(shell root-config --cflags)

CC             = gcc
CFLAGS         = -O3
CXX            = g++
CXXFLAGS       = -Wall -W -O2 -g -fPIC $(DEFINES) $(INCLUDES)

AR             = ar cr
RANLIB         = ranlib
RM             = rm -f
ECHO_N         = /bin/echo -n

# some nice messages to print when compiling
ifeq ($(CC),gcc)
CCMSG          = "       [GCC]"
else
CCMSG          = " [other GCC]"
endif
ifeq ($(CXX),g++)
CXXMSG         = "       [CXX]"
CXXLMSG        = "    [CXX-LD]"
else
CXXMSG         = " [other CXX]"
CXXLMSG        = " [o. CXX-LD]"
endif
ARMSG          = "        [AR]"
RLMSG          = "    [RANLIB]"
DEPMSG         = "       [DEP]"


# if SHOW_COMMANDS is not empty, print command instead of '[GCC]'...
ifneq ($(SHOW_COMMANDS),)
  # true is always successful and prints nothing
  SHOW = true
  # '$H' is in front of all compilation commands; if it is empty, the
  # command is shown, if it is '@', it is hidden
  H =
else
  # really print something
  SHOW = echo
  # hide the actual command
  H = @
endif

DEFINES        += -D_FILE_OFFSET_BITS=64

SRCS    = user_sort.cpp

OBJS    = $(SRCS:%.cpp=%.o)

OBJECTS        = $(sort $(OBJS))

CXXFLAGS       += $(RFLAGS)
LIBS           += $(RLIBS)

TARGETS = sorting

all: $(TARGETS)

install:

sorting: $(OBJS) $(LIBSORTING)
	@$(SHOW) $(CXXLMSG) $@
	$H $(CXX) -o $@ $^ $(LIBS)


$(LIBSORTING):
	make -C $(shell dirname $@) $(shell basename $@)

# for automatic dependency files
DEPEND = .depend-
DEPFILES = $(OBJECTS:%.o=$(DEPEND)%.dep)

%.o: %.cpp $(DEPEND)%.dep
	@$(SHOW) $(CXXMSG) $<
	$H $(CXX) $(CXXFLAGS) -o $@ -c $<

clean:  
	make -C $(shell dirname $(LIBSORTING)) $@
	$(RM) $(OBJECTS) a.out core $(TARGETS)

very-clean: clean
	make -C $(shell dirname $(LIBSORTING)) $@
	rm -f $(DEPFILES)

$(DEPEND)%.dep: %.cpp
	@$(SHOW) $(DEPMSG) $<
	$H ( $(ECHO_N) "$@ "; $(CXX) $(CXXFLAGS) -MM $< ) > $@ || rm -f $@

-include $(DEPFILES)

.SUFFIXES: # Delete the default suffixes
.PHONY: all install install-dirs install-targets clean very-clean $(LIBSORTING)