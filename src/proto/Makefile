# File: makefile
#
# This is a makefile for use with g++
#
LIBNAME = proto

CC = g++

.SUFFIXES: .cpp .h

FLAGS = -std=c++11 -g -DOMN_PLATFORM_UNIX $(Optimize) -Wall -DAOS_USERLAND  -D__USE_FILE_OFFSET64 -DBOOST_LOG_DYN_LINK

INCDIRS = -I../../src

OBJDIR = ../../obj/$(LIBNAME)

CREATEDIR = $(shell if [ -d $(OBJDIR) ]; then echo ""; else mkdir $(OBJDIR); fi)

OUTPUTDIR = ../../lib

#
# objects and targets
#
OBJECTS = $(patsubst %.cpp,$(OBJDIR)/%.o,$(wildcard *.cpp))

TARGETLIB = lib$(LIBNAME).a

#
# Rules for normal comile and link 
#
all:: lib

lib: $(OBJECTS)
	ar rvu $(TARGETLIB) $^
	mv $(TARGETLIB) $(OUTPUTDIR)

$(OBJECTS): $(OBJDIR)/%.o : %.cpp
	$(CREATEDIR)
	$(CC) -c $(FLAGS) $< $(INCDIRS) -o $@

clean:
	rm -rf $(OBJECTS) $(OUTPUTDIR)/$(TARGETLIB) 
