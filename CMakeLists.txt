cmake_minimum_required(VERSION 3.10)

project(tgnews)

set(Boost_DEBUG on)  
set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_LIB_PREFIX "lib") 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
find_package(Boost REQUIRED COMPONENTS locale) 

include_directories(
    ${PROJECT_SOURCE_DIR}
    )  
	  
file(COPY "assets" DESTINATION ".")

include_directories(${Boost_INCLUDE_DIRS}) 

add_executable(tgnews tgnews.cpp) 
  
set_target_properties(tgnews PROPERTIES CXX_STANDARD 17)

find_package(LAPACK) 

if(LAPACK_LIBRARIES)
	target_link_libraries(tgnews PRIVATE ${LAPACK_LIBRARIES})	
endif(LAPACK_LIBRARIES)
 
if(CMAKE_SYSTEM_NAME MATCHES Windows)
	target_link_directories(tgnews PRIVATE ${PROJECT_SOURCE_DIR}/mkl/lib/intel64_win)
	target_link_libraries(tgnews PRIVATE ${Boost_LIBRARIES} mkl_core.lib mkl_sequential.lib mkl_intel_lp64.lib)
endif()


if(UNIX)
	target_link_libraries(tgnews PRIVATE ${Boost_LIBRARIES})
endif()

