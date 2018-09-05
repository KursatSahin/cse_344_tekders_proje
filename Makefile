# Personal informations for creating submitting format
AUTHOR_NAME 	= ali_kursat
AUTHOR_SURNAME	= sahin
COURSE_ID 	= CSE344
STD_ID		= 091044038
HW_ID		= tek_ders
ARCH_FILE_EXT	= .tar.gz

all: client server

CC = gcc
CFLAGS = -std=c11 -Wall -Wextra
HEADERS = Common.h UDPClient.h UDPServer.h
SOURCES = Common.c
CLIENT_C = UDPClient.c
SERVER_C = UDPServer.c
OTHER = Makefile tek_ders_proje_rapor.pdf udp-server-monitor-1.0-SNAPSHOT.jar udpservermonitor

LIBS = -lpthread

EXECUTABLE_CLIENT = clientApp
EXECUTABLE_SERVER = serverApp

OBJS =  client.o server.o common.o

ARCHIVE_PATH = $(STD_ID)_$(HW_ID)_$(COURSE_ID)$(ARCH_FILE_EXT)

client:
	$(CC) $(CLIENT_C) $(SOURCES) -o $(EXECUTABLE_CLIENT) $(LIBS) $(CFLAGS)

server:
	$(CC) $(SERVER_C) $(SOURCES) -o $(EXECUTABLE_SERVER) $(LIBS) $(CFLAGS)

clean:
	-rm -f *.o core *.core $(EXECUTABLE_SERVER) $(EXECUTABLE_CLIENT)

zip:
	tar -cvzf $(ARCHIVE_PATH) $(CLIENT_C) $(SERVER_C) $(SOURCES) $(HEADERS) $(OTHER) 