
CPPFLAGS= \
	-Wall \
	-Werror \
	-g

MODULES= \
	FieldControlSystem \
	IterativeRobot \
	RobotDrive \
	DigitalOutput \
	DigitalInput \
	Joystick \
	LiveWindow \
	RedBot \
	IOBuffer \
	Packet \
	Component
OBJS=$(MODULES:%=%.o)
LIB=libwpirb.a

CPPUTEST_HOME = $(HOME)/Development/tdd/cpputest-3.6
CPPFLAGS += -I$(CPPUTEST_HOME)/include
#CXXFLAGS += -include $(CPPUTEST_HOME)/include/CppUTest/MemoryLeakDetectorNewMacros.h
LD_LIBRARIES += -L$(CPPUTEST_HOME)/lib -lCppUTest -lCppUTestExt
TEST_MODULES= \
	TestIterativeRobot \
	TestRedBot \
	TestPackets \
	TestComponents \
	TestIOBuffer
TEST_OBJS=$(TEST_MODULES:%=%.o)
TEST_RUNNER=runTests

RESIDUE= \
	$(LIB) \
	$(OBJS) \
	$(TEST_OBJS) \
	$(TEST_RUNNER)


.PHONY : all
all : tags test

.PHONY : test
test : $(TEST_RUNNER)
	./$(TEST_RUNNER)

$(TEST_RUNNER) : $(LIB) $(TEST_OBJS)
	$(CXX) \
	    $(CPPFLAGS) \
	    $(CXXFLAGS) \
	    -o $@ \
	    AllTests.cpp $(TEST_OBJS) libwpirb.a \
	    $(LD_LIBRARIES)

$(LIB) : $(OBJS)
	ar r $@ $^

tags : $(MODULES:%=%.cpp) $(MODULES:%=%.h)
	ctags $(MODULES:%=%.cpp) $(MODULES:%=%.h)

.PHONY : clean
clean :
	rm -rf $(RESIDUE)
