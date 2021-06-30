cmake_minimum_required (VERSION 3.0.0)
 
project (liveness)
set(EXECUTABLE_OUTPUT_PATH ./${CMAKE_BUILD_TYPE})
 
include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/../../../cpp
	${CMAKE_CURRENT_SOURCE_DIR}/..
)

set (SOURCE liveness.cxx)

add_executable (${PROJECT_NAME} ${SOURCE})

if (WIN32)
	set (OS_NAME, windows)
else ()
	set (OS_NAME, linux)
endif ()

find_library(FLD_LIB NAMES "FaceLivenessDetectionSDK" "FaceLivenessDetection-SDK" HINTS "${CMAKE_CURRENT_SOURCE_DIR}/../../../binaries/${OS_NAME}/x86_64")
 
target_link_libraries(${PROJECT_NAME} ${FLD_LIB})

