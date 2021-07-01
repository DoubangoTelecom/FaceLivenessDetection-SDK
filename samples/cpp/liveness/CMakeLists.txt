cmake_minimum_required (VERSION 3.0.0)
 
project (liveness VERSION 1.0.0 LANGUAGES CXX C)
set(EXECUTABLE_OUTPUT_PATH ./${CMAKE_BUILD_TYPE})
 
include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/../../../cpp
	${CMAKE_CURRENT_SOURCE_DIR}/..
)

set (SOURCE liveness.cxx)

add_executable (${PROJECT_NAME} ${SOURCE})

if (WIN32)
target_link_libraries(${PROJECT_NAME} 
	${CMAKE_CURRENT_SOURCE_DIR}/../../../binaries/windows/x86_64/FaceLivenessDetectionSDK.lib
)
else ()
target_link_libraries(${PROJECT_NAME} 
	${CMAKE_CURRENT_SOURCE_DIR}/../../../binaries/linux/x86_64/libFaceLivenessDetectionSDK.so
)
endif ()

