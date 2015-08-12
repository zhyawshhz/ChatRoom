
#// Modification History:
#// 2014/12/25: Created by Brian Zhang
#// -gstabs+
#////////////////////////////////////////////////////////////////////////////

TARGETEXE = chatroom

CC = g++

.SUFFIXES: .cpp .h

FLAGS = -std=c++0x -g -Wall -O0 -DBOOST_ALL_DYN_LINK -DBOOST_LOG_DYN_LINK -DOMN_PLATFORM_UNIX  -fpermissive $(Optimize)

INCDIRS = -I../../src

LIBDIR = -L../../lib

OBJDIR = ../../obj/$(TARGETEXE)

CREATEOBJDIR = $(shell if [ -d $(OBJDIR) ]; then echo ""; else mkdir $(OBJDIR); fi)

LIBS = -lboost_log_setup -lboost_log -lprotobuf -lprotobuf-lite -lprotoc -lboost_date_time -lboost_system -lboost_filesystem -lboost_thread -lboost_program_options -lboost_locale -lpthread -llog


#
# objects and targets
#
OBJECTS = $(patsubst %.cpp,$(OBJDIR)/%.o,$(wildcard *.cpp))

#
# Rules for normal comile and link 
#
all:: $(TARGETEXE) 

$(TARGETEXE): $(OBJECTS)
	$(CC) -o $(TARGETEXE) -O0 $^ $(LIBS) $(LIBDIR) 

$(OBJECTS): $(OBJDIR)/%.o: %.cpp
	$(CREATEOBJDIR)
	$(CC) -c $(FLAGS) $< $(INCDIRS) -o $@

clean:
	rm -rf $(OBJDIR)/*.o $(TARGETEXE) 
