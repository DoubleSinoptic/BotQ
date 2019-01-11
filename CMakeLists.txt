cmake_minimum_required(VERSION 3.1)
project(Sge)

find_package(OpenAL REQUIRED)
find_package(glfw3 REQUIRED)

set(CMAKE_CXX_STANDARD 14)
#clang skip
IF (WIN32)
	IF (MSVC)
		
	ELSE()
		
	ENDIF()
ELSE()
  
ENDIF()
link_directories(/usr/local/lib/) 
include_directories(/usr/local/include/)

include_directories(./ThirdParty/include)
include_directories(./BulletPhysics/Code)
include_directories(./BotQEngine/Code)
include_directories(./BotQGraphics/Code)
include_directories(./BotQSystemTools/Code)
include_directories(./zlib_static/Code)
include_directories(./MinimalistStart/Code)

file(GLOB_RECURSE ZLibC ./zlib_static/Code/*.c)
file(GLOB_RECURSE ZLibH ./zlib_static/Code/*.h)
add_library(ZLib STATIC ${ZLibC} ${ZLibH})

file(GLOB_RECURSE PhysicsCpp ./BulletPhysics/Code/*.cpp)
file(GLOB_RECURSE PhysicsH ./BulletPhysics/Code/*.h)
add_library(Physics SHARED ${PhysicsCpp} ${PhysicsH})

file(GLOB_RECURSE BotQGraphicsC ./BotQGraphics/Code/*.c)
file(GLOB_RECURSE BotQGraphicsCpp ./BotQGraphics/Code/*.cpp)
file(GLOB_RECURSE BotQGraphicsH ./BotQGraphics/Code/*.h)
add_library(BotQGraphics SHARED ${BotQGraphicsCpp} ${BotQGraphicsH} ${BotQGraphicsC})

file(GLOB_RECURSE BotQSystemToolsCpp ./BotQSystemTools/Code/*.cpp)
file(GLOB_RECURSE BotQSystemToolsH ./BotQSystemTools/Code/*.h)
add_library(BotQSystemTools SHARED ${BotQSystemToolsCpp} ${BotQSystemToolsH})

file(GLOB_RECURSE BotQEngineCpp ./BotQEngine/Code/*.cpp)
file(GLOB_RECURSE BotQEngineH ./BotQEngine/Code/*.h)
add_library(BotQEngine SHARED ${BotQEngineCpp} ${BotQEngineH})
target_link_libraries(BotQEngine Physics BotQGraphics BotQSystemTools ZLib ${OPENAL_LIBRARY} assimp)

file(GLOB_RECURSE MinimalistStartCpp ./MinimalistStart/*.cpp)
file(GLOB_RECURSE MinimalistStartH ./MinimalistStart/*.h)
add_executable(MinimalistStart ${MinimalistStartCpp} ${MinimalistStartH})
target_link_libraries(MinimalistStart BotQEngine glfw)














