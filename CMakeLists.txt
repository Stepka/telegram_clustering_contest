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