###########################################  
#Makefile for simple programs  
###########################################  
INC=-I/mnt/hgfs/share/test/work/resource/curl/include \
	-Icurl/include \
	-IProtocolClient/
	
LDFLAGS=-L/mnt/hgfs/share/test/work/resource/curl/lib
	
LIBS=-lcurl
	
CC=arm-linux-g++
CC_FLAG=-Wall  
PRG=test  
OBJ=main.o  \
	curl/src/mycurl.o \
	ProtocolClient/BlockSocket.o \
	ProtocolClient/utf8_gb2312.o \
	ProtocolClient/utf8_unicode.o \
	ProtocolClient/pkg.o

  
$(PRG):$(OBJ)
	$(CC) $(INC) $(LDFLAGS) $(LIBS) -o $@ $(OBJ)  
      
.SUFFIXES: .c .o .cpp .hpp 
.cpp.o:
	$(CC) $(CC_FLAG) $(INC) -c $*.cpp -o $*.o

#.hpp.o:
#	$(CC) $(CC_FLAG) $(INC) -c $*.hpp -o $*.o  	
  
.PRONY:clean  
clean:
	@echo "Removing linked and compiled files......"
	rm -f $(OBJ) $(PRG)  