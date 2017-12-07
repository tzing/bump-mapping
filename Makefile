SRCDIR:=./src
SRCS:=main.cpp shader.cpp glm_helper.cpp interaction.cpp

OUTPUT:=ball

CFLAGS=-I./include -I./glm/include
LIBS=-L./glm/lib -lglm -ljpeg


# auto -------------------------------------
CFLAGS+= $(shell pkg-config --cflags glew)
LIBS+= $(shell pkg-config --libs glew libpng)

OBJS=$(subst .cpp,.o,$(SRCS))


# os depends -------------------------------
OS=$(shell uname)

ifeq ($(OS),Darwin)
	CFLAGS+= -std=c++11
	LIBS+= -framework OpenGL -framework GLUT
else
	LIBS+= -lGL -lGLU -lglut
endif


# rules ------------------------------------
.DEFAULT_GOAL := $(OUTPUT)
.PHONY: all clean cleanall $(OUTPUT)

all: $(OUTPUT)

%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CFLAGS) $(CXXFLAGS) -c $< $(LIBS)

$(OUTPUT): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(LIBS)

clean:
	$(RM) $(OBJS)

cleanall: clean
	$(RM) $(OUTPUT)
