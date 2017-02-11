### Toolchains and env
HOST=

COMPILER = $(HOST)g++
LINKER	= $(HOST)g++

AR = $(HOST)ar cr

CFLAGS = -O3 -Wall -fPIC -shared
LDFLAGS = 
APPNAME = test

INCLUDE = 

### Source 
	
SRC_CPP = main.cpp
SRC_CPP += BlockSocket.cpp
SRC_CPP += utf8_gb2312.cpp
SRC_CPP += utf8_unicode.cpp

SRC = pkg.c



OBJ = $(patsubst %.c,%.o,$(SRC))
OBJ += $(patsubst %.cpp,%.o,$(SRC_CPP))

### Rules
app : $(OBJ)
	@echo $(OBJ)
	$(LINKER) -o $(APPNAME) $^ -lpthread

%.c.o : %.c
	$(COMPILER) $(CFLAGS) $(INCLUDE) -c $< -o $@
	
%.cpp.o : %.cpp
	$(COMPILER) $(CFLAGS) $(INCLUDE) -c $< -o $@

clean :
	rm -rf $(OBJ)
	rm -rf $(APPNAME)

