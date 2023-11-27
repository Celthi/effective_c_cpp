project(LinkTwice )
cmake_minimum_required(VERSION 3.15)
if(WIN32)
else(WIN32)
    ADD_DEFINITIONS(-fPIC)
endif(WIN32)
set(CXX_STANDARD_REQUIRED 17)
set(CMAKE_CXX_STANDARD 17)
ADD_LIBRARY( static_lib STATIC shared_library.cpp header.h)

ADD_LIBRARY( shared_lib SHARED shared_library.cpp header.h)

TARGET_LINK_LIBRARIES( shared_lib static_lib )

ADD_EXECUTABLE( main_exe main.cpp )
TARGET_LINK_LIBRARIES( main_exe static_lib shared_lib )
