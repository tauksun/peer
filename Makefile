CXX = clang++
CXXFLAGS = -Wall -g

TARGET = build/exec/peer
SRCS = main.cpp server.cpp client.cpp
OBJDIR = build/objects
OBJS = $(patsubst %.cpp,$(OBJDIR)/%.o,$(SRCS))

all: $(TARGET)

# Link the executable
$(TARGET): $(OBJS)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

# Compile .cpp files into .o files inside build/objects/
$(OBJDIR)/%.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf build
