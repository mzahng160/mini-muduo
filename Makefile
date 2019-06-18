DESTINATION := mini-muduo
LIBS := pthread
INCLUDE := .

RM := rm -f
#c, cc ,cpp 
PS=cc
CC=g++
CPPFLAG = -g -Wall -O3 -march=x86
CPPFLAG += $(addprefix -I, $(INCLUDE))
CPPFLAG += -MMD

SOURCE := $(wildcard *.$(PS))
OBJS := $(patsubst %.$(PS), %.o, $(SOURCE))
DEPS := $(patsubst %.o, %.d, $(OBJS))
MISSING_DEPS := $(filter-out $(wildcard $(DEPS)),$(DEPS))
MISSING_DEPS_SOURCES := $(wildcard $(patsubst %.d,%.$(PS),$(MISSING_DEPS)))

.PHONY : all deps objs clean rebuild

all : $(DESTINATION)

deps : $(DEPS)
		$(CC) -MM -MMD $(SOURCE)

objs : $(OBJS)

clean :
		@$(RM) *.o
		@$(RM) *.d
		@$(RM) $(DESTINATION)

rebuild : clean all

ifneq ($(MISSING_DEPS),)
$(MISSING_DEPS) :
		@$(RM) $(patsubst %.d,%.o,$@)
endif

$(DESTINATION) : $(OBJS)
		$(CC) -o $(DESTINATION) $(OBJS) $(addprefix -1, $(LIBS))