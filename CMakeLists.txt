cmake_minimum_required(VERSION 3.10)

project(tgnews)

set(Boost_DEBUG on)  
find_package(Boost REQUIRED COMPONENTS locale) 

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g0 -O3")

include_directories(
    ${PROJECT_SOURCE_DIR}
    )   
	  
file(COPY "assets" DESTINATION ".")

include_directories(${Boost_INCLUDE_DIRS}) 

add_executable(tgnews tgnews.cpp) 
add_executable(cluster_word2vec tools/cluster_word2vec.cpp) 
add_executable(cut_word2vec tools/cut_word2vec.cpp) 
add_executable(convert_tags_corpora tools/convert_tags_corpora.cpp) 
  
set_target_properties(tgnews PROPERTIES CXX_STANDARD 17)
set_target_properties(cluster_word2vec PROPERTIES CXX_STANDARD 17)
set_target_properties(cut_word2vec PROPERTIES CXX_STANDARD 17)
set_target_properties(convert_tags_corpora PROPERTIES CXX_STANDARD 17)

find_package(LAPACK) 

if(LAPACK_LIBRARIES)
	target_link_libraries(tgnews PRIVATE ${LAPACK_LIBRARIES})	
	target_link_libraries(cluster_word2vec PRIVATE ${LAPACK_LIBRARIES})	
	target_link_libraries(cut_word2vec PRIVATE ${LAPACK_LIBRARIES})	
	target_link_libraries(convert_tags_corpora PRIVATE ${LAPACK_LIBRARIES})	
endif(LAPACK_LIBRARIES)
 
if(CMAKE_SYSTEM_NAME MATCHES Windows)
	target_link_directories(tgnews PRIVATE ${PROJECT_SOURCE_DIR}/mkl/lib/intel64_win)
	target_link_libraries(tgnews PRIVATE ${Boost_LIBRARIES} mkl_core.lib mkl_sequential.lib mkl_intel_lp64.lib)
	
	target_link_directories(cluster_word2vec PRIVATE ${PROJECT_SOURCE_DIR}/mkl/lib/intel64_win)
	target_link_libraries(cluster_word2vec PRIVATE ${Boost_LIBRARIES} mkl_core.lib mkl_sequential.lib mkl_intel_lp64.lib)
	
	target_link_directories(cut_word2vec PRIVATE ${PROJECT_SOURCE_DIR}/mkl/lib/intel64_win)
	target_link_libraries(cut_word2vec PRIVATE ${Boost_LIBRARIES} mkl_core.lib mkl_sequential.lib mkl_intel_lp64.lib)
	
	target_link_directories(convert_tags_corpora PRIVATE ${PROJECT_SOURCE_DIR}/mkl/lib/intel64_win)
	target_link_libraries(convert_tags_corpora PRIVATE ${Boost_LIBRARIES} mkl_core.lib mkl_sequential.lib mkl_intel_lp64.lib)
endif()


if(UNIX)
	target_link_libraries(tgnews PRIVATE ${Boost_LIBRARIES})
	target_link_libraries(cluster_word2vec PRIVATE ${Boost_LIBRARIES})
	target_link_libraries(cut_word2vec PRIVATE ${Boost_LIBRARIES})
	target_link_libraries(convert_tags_corpora PRIVATE ${Boost_LIBRARIES})
endif()
 
