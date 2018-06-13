TPATH = ../../../../../../
INCLUDE=-I./
            
LIBS+=-lrt


CC=gcc
CXX=g++
GCCVER := $(shell $(CC) -dumpversion | awk -F. '{ print $$1"."$$2}' )

#CFLAGS=-g -pg -O3 -fPIC  -Wall -pipe -fno-ident -D_GNU_SOURCE -D_REENTRANT
CFLAGS=-g -O3 -fPIC  -Wall -pipe -fno-ident -D_GNU_SOURCE -D_REENTRANT

TARGET = cal

BINARY = $(patsubst %.cpp, %.o, $(wildcard *.cpp))
#BINARY = $(patsubst %.cpp,%.o, $(patsubst %.c,%.o, $(LOCAL_SRC)))

all: $(TARGET)
	

$(TARGET):$(BINARY)
	$(CXX) $? -o $@ $(CFLAGS) $(INCLUDE) $(LIBS)
	
%.o:%.cpp
	$(CXX) -c $< -o $@ $(CFLAGS) $(INCLUDE)

clean:
	@rm -f $(TARGET) $(BINARY)
	

all32:
	make -e ARCH=32
	

	 