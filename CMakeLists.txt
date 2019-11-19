cmake_minimum_required(VERSION 3.12)

project(tgnews)

set(Boost_DEBUG on)  
find_package(Boost)

include_directories(
    ${PROJECT_SOURCE_DIR}
    )

include_directories(${Boost_INCLUDE_DIRS}) 

add_executable(tgnews tgnews.cpp) 
 
target_link_libraries(tgnews ${Boost_LIBRARIES})
set_target_properties(tgnews PROPERTIES CXX_STANDARD 17)


find_package(LAPACK)

if(LAPACK_LIBRARIES)
	target_link_libraries(tgnews PRIVATE ${LAPACK_LIBRARIES})	
endif(LAPACK_LIBRARIES)
 
if(CMAKE_SYSTEM_NAME MATCHES Windows)
	target_link_directories(tgnews PRIVATE ${PROJECT_SOURCE_DIR}/mkl/lib/intel64_win)
	target_link_libraries(tgnews PRIVATE mkl_core.lib mkl_sequential.lib mkl_intel_lp64.lib)
	target_link_libraries(tgnews PRIVATE ${Boost_LIBRARIES})
endif()

