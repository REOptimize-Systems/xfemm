#------------------------------------------------------------------------------#
# This makefile was generated by 'cbp2make' tool rev.147                       #
#------------------------------------------------------------------------------#


WRKDIR = $(shell pwd)

CC = gcc
CXX = g++
AR = ar
LD = g++
WINDRES = windres

INC = -I../libfemm
CFLAGS = -Wall -fexceptions
RESINC = 
LIBDIR = -L../libfemm
LIB = ../libfemm/libfemm.a
LDFLAGS = 

INC_DEBUG = $(INC)
CFLAGS_DEBUG = $(CFLAGS) -g
RESINC_DEBUG = $(RESINC)
RCFLAGS_DEBUG = $(RCFLAGS)
LIBDIR_DEBUG = $(LIBDIR) -L../libfemm
LIB_DEBUG = $(LIB)
LDFLAGS_DEBUG = $(LDFLAGS)
OBJDIR_DEBUG = $(WRKDIR)/obj/Debug
DEP_DEBUG = 
BINDIR_DEBUG = $(WRKDIR)/bin/Debug
OUT_DEBUG = $(BINDIR_DEBUG)/fmesher

INC_RELEASE = $(INC)
CFLAGS_RELEASE = $(CFLAGS) -O2
RESINC_RELEASE = $(RESINC)
RCFLAGS_RELEASE = $(RCFLAGS)
LIBDIR_RELEASE = $(LIBDIR)
LIB_RELEASE = $(LIB)
LDFLAGS_RELEASE = $(LDFLAGS) -s
OBJDIR_RELEASE = $(WRKDIR)/obj/Release
DEP_RELEASE =
BINDIR_RELEASE = $(WRKDIR)/bin/Release 
OUT_RELEASE = $(BINDIR_RELEASE)/fmesher

INC_LIBRARY = $(INC)
CFLAGS_LIBRARY = $(CFLAGS) -O2
RESINC_LIBRARY = $(RESINC)
RCFLAGS_LIBRARY = $(RCFLAGS)
LIBDIR_LIBRARY = $(LIBDIR)
LIB_LIBRARY = $(LIB)
LDFLAGS_LIBRARY = $(LDFLAGS)
OBJDIR_LIBRARY = .objs
DEP_LIBRARY = 
OUT_LIBRARY = $(WRKDIR)/libfmesher.a

OBJ_DEBUG = $(OBJDIR_DEBUG)/fmesher.o $(OBJDIR_DEBUG)/intpoint.o $(OBJDIR_DEBUG)/main.o $(OBJDIR_DEBUG)/nosebl.o $(OBJDIR_DEBUG)/triangle.o $(OBJDIR_DEBUG)/writepoly.o

OBJ_RELEASE = $(OBJDIR_RELEASE)/fmesher.o $(OBJDIR_RELEASE)/intpoint.o $(OBJDIR_RELEASE)/main.o $(OBJDIR_RELEASE)/nosebl.o $(OBJDIR_RELEASE)/triangle.o $(OBJDIR_RELEASE)/writepoly.o

OBJ_LIBRARY = $(OBJDIR_LIBRARY)/fmesher.o $(OBJDIR_LIBRARY)/intpoint.o $(OBJDIR_LIBRARY)/nosebl.o $(OBJDIR_LIBRARY)/triangle.o $(OBJDIR_LIBRARY)/writepoly.o

all: release library

clean: clean_debug clean_release clean_library

before_debug: 
	test -d $(BINDIR_DEBUG) || mkdir -p $(BINDIR_DEBUG)
	test -d $(OBJDIR_DEBUG) || mkdir -p $(OBJDIR_DEBUG)

after_debug: 

debug: before_debug out_debug after_debug

out_debug: before_debug $(OBJ_DEBUG) $(DEP_DEBUG)
	$(LD) $(LIBDIR_DEBUG) -o $(OUT_DEBUG) $(OBJ_DEBUG)  $(LDFLAGS_DEBUG) $(LIB_DEBUG)

$(OBJDIR_DEBUG)/fmesher.o: fmesher.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c fmesher.cpp -o $(OBJDIR_DEBUG)/fmesher.o

$(OBJDIR_DEBUG)/intpoint.o: intpoint.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c intpoint.cpp -o $(OBJDIR_DEBUG)/intpoint.o

$(OBJDIR_DEBUG)/main.o: main.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c main.cpp -o $(OBJDIR_DEBUG)/main.o

$(OBJDIR_DEBUG)/nosebl.o: nosebl.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c nosebl.cpp -o $(OBJDIR_DEBUG)/nosebl.o

$(OBJDIR_DEBUG)/triangle.o: triangle.c
	$(CC) $(CFLAGS_DEBUG) $(INC_DEBUG) -c triangle.c -o $(OBJDIR_DEBUG)/triangle.o

$(OBJDIR_DEBUG)/writepoly.o: writepoly.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c writepoly.cpp -o $(OBJDIR_DEBUG)/writepoly.o

clean_debug: 
	rm -f $(OBJ_DEBUG) $(OUT_DEBUG)
	rm -rf $(BINDIR_DEBUG)
	rm -rf $(OBJDIR_DEBUG)

before_release: 
	test -d $(BINDIR_RELEASE) || mkdir -p $(BINDIR_RELEASE)
	test -d $(OBJDIR_RELEASE) || mkdir -p $(OBJDIR_RELEASE)

after_release: 

release: before_release out_release after_release

out_release: before_release $(OBJ_RELEASE) $(DEP_RELEASE)
	$(LD) $(LIBDIR_RELEASE) -o $(OUT_RELEASE) $(OBJ_RELEASE)  $(LDFLAGS_RELEASE) $(LIB_RELEASE)

$(OBJDIR_RELEASE)/fmesher.o: fmesher.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c fmesher.cpp -o $(OBJDIR_RELEASE)/fmesher.o

$(OBJDIR_RELEASE)/intpoint.o: intpoint.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c intpoint.cpp -o $(OBJDIR_RELEASE)/intpoint.o

$(OBJDIR_RELEASE)/main.o: main.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c main.cpp -o $(OBJDIR_RELEASE)/main.o

$(OBJDIR_RELEASE)/nosebl.o: nosebl.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c nosebl.cpp -o $(OBJDIR_RELEASE)/nosebl.o

$(OBJDIR_RELEASE)/triangle.o: triangle.c
	$(CC) $(CFLAGS_RELEASE) $(INC_RELEASE) -c triangle.c -o $(OBJDIR_RELEASE)/triangle.o

$(OBJDIR_RELEASE)/writepoly.o: writepoly.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c writepoly.cpp -o $(OBJDIR_RELEASE)/writepoly.o

clean_release: 
	rm -f $(OBJ_RELEASE) $(OUT_RELEASE)
	rm -rf $(BINDIR_RELEASE)
	rm -rf $(OBJDIR_RELEASE)

before_library: 
	test -d $(OBJDIR_LIBRARY) || mkdir -p $(OBJDIR_LIBRARY)

after_library: 

library: before_library out_library after_library

out_library: before_library $(OBJ_LIBRARY) $(DEP_LIBRARY)
	$(AR) rcs $(OUT_LIBRARY) $(OBJ_LIBRARY)

$(OBJDIR_LIBRARY)/fmesher.o: fmesher.cpp
	$(CXX) $(CFLAGS_LIBRARY) $(INC_LIBRARY) -c fmesher.cpp -o $(OBJDIR_LIBRARY)/fmesher.o

$(OBJDIR_LIBRARY)/intpoint.o: intpoint.cpp
	$(CXX) $(CFLAGS_LIBRARY) $(INC_LIBRARY) -c intpoint.cpp -o $(OBJDIR_LIBRARY)/intpoint.o

$(OBJDIR_LIBRARY)/nosebl.o: nosebl.cpp
	$(CXX) $(CFLAGS_LIBRARY) $(INC_LIBRARY) -c nosebl.cpp -o $(OBJDIR_LIBRARY)/nosebl.o

$(OBJDIR_LIBRARY)/triangle.o: triangle.c
	$(CC) $(CFLAGS_LIBRARY) $(INC_LIBRARY) -c triangle.c -o $(OBJDIR_LIBRARY)/triangle.o

$(OBJDIR_LIBRARY)/writepoly.o: writepoly.cpp
	$(CXX) $(CFLAGS_LIBRARY) $(INC_LIBRARY) -c writepoly.cpp -o $(OBJDIR_LIBRARY)/writepoly.o

clean_library: 
	rm -f $(OBJ_LIBRARY) $(OUT_LIBRARY)
	rm -rf $(OBJDIR_LIBRARY)

.PHONY: before_debug after_debug clean_debug before_release after_release clean_release before_library after_library clean_library

