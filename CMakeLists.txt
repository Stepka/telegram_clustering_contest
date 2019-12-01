cmake_minimum_required(VERSION 3.10)

project(tgnews)

set(Boost_DEBUG on)  
#set(ICU_DEBUG on)   

#find_package(ICU)

if(STATIC_LINKING)
	message("Static linking")
	add_definitions(-D_Boost_USE_STATIC_LIBS=ON)
	set(Boost_USE_STATIC_LIBS ON)
	set(Boost_USE_STATIC_RUNTIME OFF)
	find_package(Boost COMPONENTS locale thread system REQUIRED)
else()
	message("Shared linking")
	set(Boost_USE_STATIC_LIBS OFF)
	set(Boost_USE_STATIC_RUNTIME OFF)
	find_package(Boost COMPONENTS locale REQUIRED)
endif()


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
if(STATIC_LINKING)
	set_target_properties(tgnews PROPERTIES LINK_SEARCH_START_STATIC 1)
	set_target_properties(tgnews PROPERTIES LINK_SEARCH_END_STATIC 1)
	set(CMAKE_FIND_LIBRARY_SUFFIXES .a)
	set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
endif()

set_target_properties(cluster_word2vec PROPERTIES CXX_STANDARD 17)
set_target_properties(cut_word2vec PROPERTIES CXX_STANDARD 17)
set_target_properties(convert_tags_corpora PROPERTIES CXX_STANDARD 17)

if(STATIC_LINKING)
	set_target_properties(cluster_word2vec PROPERTIES LINK_SEARCH_START_STATIC 1)
	set_target_properties(cluster_word2vec PROPERTIES LINK_SEARCH_END_STATIC 1)
	set_target_properties(cut_word2vec PROPERTIES LINK_SEARCH_START_STATIC 1)
	set_target_properties(cut_word2vec PROPERTIES LINK_SEARCH_END_STATIC 1)
	set_target_properties(convert_tags_corpora PROPERTIES LINK_SEARCH_START_STATIC 1)
	set_target_properties(convert_tags_corpora PROPERTIES LINK_SEARCH_END_STATIC 1)
endif()


if(UNIX)
	target_compile_options(tgnews PRIVATE -pthread -g0 -O3)
	set_target_properties(tgnews PROPERTIES LINK_FLAGS -pthread)
	
	target_compile_options(cluster_word2vec PRIVATE -pthread -g0 -O3)
	set_target_properties(cluster_word2vec PROPERTIES LINK_FLAGS -pthread)
	
	target_compile_options(cut_word2vec PRIVATE -pthread -g0 -O3)
	set_target_properties(cut_word2vec PROPERTIES LINK_FLAGS -pthread)
	
	target_compile_options(convert_tags_corpora PRIVATE -pthread -g0 -O3)
	set_target_properties(convert_tags_corpora PROPERTIES LINK_FLAGS -pthread)
	
	if(STATIC_LINKING)
	
		#find_package(LAPACK)

		#if(LAPACK_LIBRARIES)
			target_link_libraries(tgnews PRIVATE liblapack.a)
			target_link_libraries(cluster_word2vec PRIVATE liblapack.a)
			target_link_libraries(cut_word2vec PRIVATE liblapack.a)
			target_link_libraries(convert_tags_corpora PRIVATE liblapack.a)
		#endif(LAPACK_LIBRARIES)

		target_link_libraries(tgnews PRIVATE ${Boost_LIBRARIES} icuuc.a icui18n.a)
		target_link_libraries(cluster_word2vec PRIVATE ${Boost_LIBRARIES} icuuc.a icui18n.a)
		target_link_libraries(cut_word2vec PRIVATE ${Boost_LIBRARIES} icuuc.a icui18n.a)
		target_link_libraries(convert_tags_corpora PRIVATE ${Boost_LIBRARIES} icuuc.a icui18n.a)
	else()

		find_package(LAPACK)

		if(LAPACK_LIBRARIES)
			target_link_libraries(tgnews PRIVATE ${LAPACK_LIBRARIES})
			target_link_libraries(cluster_word2vec PRIVATE ${LAPACK_LIBRARIES})
			target_link_libraries(cut_word2vec PRIVATE ${LAPACK_LIBRARIES})
			target_link_libraries(convert_tags_corpora PRIVATE ${LAPACK_LIBRARIES})
		endif(LAPACK_LIBRARIES)

		target_link_libraries(tgnews PRIVATE ${Boost_LIBRARIES})
		target_link_libraries(cluster_word2vec PRIVATE ${Boost_LIBRARIES})
		target_link_libraries(cut_word2vec PRIVATE ${Boost_LIBRARIES})
		target_link_libraries(convert_tags_corpora PRIVATE ${Boost_LIBRARIES})
	endif(STATIC_LINKING)
 
endif(UNIX)

if(CMAKE_SYSTEM_NAME MATCHES Windows)
	find_package(LAPACK)

	if(LAPACK_LIBRARIES)
		target_link_libraries(tgnews PRIVATE liblapack.a)
		target_link_libraries(cluster_word2vec PRIVATE liblapack.a)
		target_link_libraries(cut_word2vec PRIVATE liblapack.a)
		target_link_libraries(convert_tags_corpora PRIVATE liblapack.a)
	endif(LAPACK_LIBRARIES)

	target_link_directories(tgnews PRIVATE ${PROJECT_SOURCE_DIR}/mkl/lib/intel64_win)
	target_link_libraries(tgnews PRIVATE ${Boost_LIBRARIES} mkl_core.lib mkl_sequential.lib mkl_intel_lp64.lib)
	
	target_link_directories(cluster_word2vec PRIVATE ${PROJECT_SOURCE_DIR}/mkl/lib/intel64_win)
	target_link_libraries(cluster_word2vec PRIVATE ${Boost_LIBRARIES} mkl_core.lib mkl_sequential.lib mkl_intel_lp64.lib)
	
	target_link_directories(cut_word2vec PRIVATE ${PROJECT_SOURCE_DIR}/mkl/lib/intel64_win)
	target_link_libraries(cut_word2vec PRIVATE ${Boost_LIBRARIES} mkl_core.lib mkl_sequential.lib mkl_intel_lp64.lib)
	
	target_link_directories(convert_tags_corpora PRIVATE ${PROJECT_SOURCE_DIR}/mkl/lib/intel64_win)
	target_link_libraries(convert_tags_corpora PRIVATE ${Boost_LIBRARIES} mkl_core.lib mkl_sequential.lib mkl_intel_lp64.lib)
endif()


 
