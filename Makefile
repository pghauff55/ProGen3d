


TARGET=procedural_buildings
OBJS=main.o MathParser.o Evaluate.o lexer.o NFunction.o Rule.o string-switch.o ExtendedCMath.o LBAssembler.o MathObject.o NFunctionalOperator.o NMathObject.o  RuleLib.o \
       Context.o   Mesh.o  PLYWriter.o  Scope.o  

# Standard compiler variables

CXX = g++
CXXFLAGS =  -g -O3 -Wreturn-type

# Rules start here

$(TARGET):$(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $^

.o:.cpp
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(TARGET) *.o
