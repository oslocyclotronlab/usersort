# -*- Makefile -*-

# uncomment the following if you do not have zlib
#DEFINES        += -DMISSING_ZLIB

LIBS           = 
INCLUDES       = 
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

SRCS    = $(wildcard *.cpp)

OBJS    = $(SRCS:%.cpp=%.o)

OBJECTS        = $(sort $(OBJS))

CXXFLAGS       += $(RFLAGS)
LIBS           += $(RLIBS)

TARGETS = libsorting.a

all: $(TARGETS)

install:

libsorting.a: $(OBJS)
	@$(SHOW) $(ARMSG) $@
	$H $(AR) $@ $^
	$H $(RANLIB) $@

# for automatic dependency files
DEPEND = .depend-
DEPFILES = $(OBJECTS:%.o=$(DEPEND)%.dep)

%.o: %.cpp $(DEPEND)%.dep
	@$(SHOW) $(CXXMSG) $<
	$H $(CXX) $(CXXFLAGS) -o $@ -c $<

clean:  
	$(RM) $(OBJECTS) a.out core $(TARGETS)

very-clean: clean
	rm -f $(DEPFILES)

$(DEPEND)%.dep: %.cpp
	@$(SHOW) $(DEPMSG) $<
	$H ( $(ECHO_N) "$@ "; $(CXX) $(CXXFLAGS) -MM $< ) > $@ || rm -f $@

-include $(DEPFILES)

.SUFFIXES: # Delete the default suffixes
.PHONY: all install install-dirs install-targets clean very-clean
