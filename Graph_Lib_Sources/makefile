CC = g++ -std=c++11
INCLUDE_FLAG = -I /opt/boost_1_67_0
LIB_FLAG = -L /opt/boost_1_67_0/stage/lib -lboost_regex -lboost_graph -pthread -lcrypto

OBJDIR = obj
BINDIR = bin

# Client part
CLIENT_SRCDIR = Client/src_cpp
CLIENT_SOURCE = $(wildcard $(CLIENT_SRCDIR)/*.cpp)
CLIENT_INCLUDE = $(wildcard $(CLIENT_SRCDIR)/*.h)
CLIENT_OBJECTS = $(CLIENT_SOURCE:$(CLIENT_SRCDIR)/%.cpp=$(OBJDIR)/%.o)

# Server part
SERVER_SRCDIR = Server/src
SERVER_SOURCE = $(wildcard $(SERVER_SRCDIR)/*.cpp)
SERVER_INCLUDE = $(wildcard $(SERVER_SRCDIR)/*.h)
SERVER_OBJECTS = $(SERVER_SOURCE:$(SERVER_SRCDIR)/%.cpp=$(OBJDIR)/%.o)

# Library part
LIBS_SRCDIR = CommonLib
LIBS_SOURCE = $(wildcard $(LIBS_SRCDIR)/src/*.cpp)
LIBS_INCLUDE = $(wildcard $(LIBS_SRCDIR)/include/*.h)
LIBS_OBJECTS = $(LIBS_SOURCE:$(LIBS_SRCDIR)/src/%.cpp=$(OBJDIR)/%.o)
METIS = /opt/metis-5.1.0/libmetis.a

all: $(BINDIR)/client $(BINDIR)/server 
client: $(BINDIR)/client
server: $(BINDIR)/server

$(BINDIR)/client: $(CLIENT_OBJECTS) | $(BINDIR)
	$(CC) $(CLIENT_OBJECTS) -o $@ -lcrypto
	@echo "[Frontend]: compilation complete!"
$(CLIENT_OBJECTS): $(OBJDIR)/%.o : $(CLIENT_SRCDIR)/%.cpp $(CLIENT_INCLUDE) | $(OBJDIR)
	$(CC) -c $< -o $@ -lcrypto

$(BINDIR)/server: $(SERVER_OBJECTS) $(LIBS_OBJECTS) | $(BINDIR)
	$(CC) $(INCLUDE_FLAG) $(SERVER_OBJECTS) $(filter-out $(OBJDIR)/client_main.o, $(CLIENT_OBJECTS)) $(LIBS_OBJECTS) $(METIS) -o $@ $(LIB_FLAG)
	@echo "[Backend]: compilation complete!"
$(SERVER_OBJECTS): $(OBJDIR)/%.o : $(SERVER_SRCDIR)/%.cpp $(SERVER_INCLUDE) $(LIBS_INCLUDE) | $(OBJDIR)
	$(CC) -c $(INCLUDE_FLAG) $< -o $@ $(LIB_FLAG)

$(LIBS_OBJECTS): $(OBJDIR)/%.o : $(LIBS_SRCDIR)/src/%.cpp $(LIBS_INCLUDE) | $(OBJDIR)
	$(CC) -c $(INCLUDE_FLAG) $< -o $@ $(LIB_FLAG)

graphgen: $(BINDIR)/graphgen | $(BINDIR)
$(BINDIR)/graphgen: graph_generator.cpp
	$(CC) $(INCLUDE_FLAG) $< -o $@ $(LIB_FLAG)

graphmem: $(BINDIR)/graphmem | $(BINDIR)
$(BINDIR)/graphmem: graph_mem_test.cpp
	g++ $(INCLUDE_FLAG) $< -o $@ $(LIB_FLAG)

$(OBJDIR):
	mkdir -p $(OBJDIR)
$(BINDIR):
	mkdir -p $(BINDIR)

clean:
	rm -rf $(BINDIR)
	rm -rf $(OBJDIR)
	rm -rf Client/src_python/__pycache__