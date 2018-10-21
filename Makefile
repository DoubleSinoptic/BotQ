
SGE_EXEC = bin/SgeTests.exe

CC 		  = g++
CXX       = g++ -std=c++14
AR        = ar rcs


# Linker flags
LDFLAGS   = -pthread

INCLUDE_FLAGS=-IBotQGraphics/Code -IBotQEngine/Code -IBotQSystemTools/Code -IBotQ/Code -IThirdParty/include -IBulletPhysics/Code
LIBRARY_FLAGS=-lz 


BotQEngine_SRC = \
		$(wildcard BotQEngine/Code/**/*.cpp)        \
		$(wildcard BotQEngine/Code/**/*.c)	
		
BotQEngine_OBJ = $(BotQEngine_SRC:.cpp=.o)  $(BotQEngine_SRC:.c=.o)		


BotQGraphics_SRC = \
		$(wildcard BotQGraphics/Code/**/*.cpp)        \
		$(wildcard BotQGraphics/Code/**/*.c)	
		
BotQGraphics_OBJ = $(BotQGraphics_SRC:.cpp=.o) $(BotQGraphics_SRC:.c=.o)				
		
	
BotQ_SRC = \
		$(wildcard BotQ/Code/**/*.cpp)        \
		$(wildcard BotQ/Code/**/*.c)	
		
BotQ_OBJ = $(BotQ_SRC:.cpp=.o) $(BotQ_SRC:.c=.o)			
			
	
BotQSystemTools_SRC = \
		$(wildcard BotQSystemTools/Code/**/*.cpp)        \
		$(wildcard BotQSystemTools/Code/**/*.c)	
		
BotQSystemTools_OBJ = $(BotQSystemTools_SRC:.cpp=.o) $(BotQSystemTools_SRC:.c=.o)	 		
	
BulletPhysics_SRC = \
		$(wildcard BulletPhysics/Code/**/*.cpp)        \
		$(wildcard BulletPhysics/Code/**/*.c)	
		
BulletPhysics_OBJ = $(BulletPhysics_SRC:.cpp=.o) $(BulletPhysics_SRC:.c=.o)			
	
# Rules
all: $(SGE_EXEC)

$(SGE_EXEC): $(BotQEngine_OBJ) $(BotQGraphics_OBJ) $(BotQ_OBJ) $(BotQSystemTools_OBJ) $(BulletPhysics_OBJ)
	$(CXX) $(LDFLAGS) -fPIC $^ -o $@  $(LIBRARY_FLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(INCLUDE_FLAGS)
	
%.o: %.c
	$(CC) $(CXXFLAGS) -c $< -o $@ $(INCLUDE_FLAGS)
	

