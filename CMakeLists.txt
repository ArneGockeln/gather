cmake_minimum_required(VERSION 3.7)

set(CMAKE_CXX_STANDARD 14)
# release build type
set(CMAKE_BUILD_TYPE Release)

set(INC_DIR "/usr/local/include")
set(OPT_DIR "/usr/local/opt")
set(LIB_DIR "/usr/local/lib")
set(BIN_DIR "/usr/local/bin")

# prevent losing xtbclient rpath link
if(UNIX)
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH true)
endif(UNIX)

# xtbclient library
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I${INC_DIR}/xtbclient")
# date dependency
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I${INC_DIR}/date/include")

set(EXEC_NAME gather)

set(SOURCE_FILES src/main.cpp)
add_executable(${EXEC_NAME} ${SOURCE_FILES})

if(APPLE)
    target_link_libraries(${EXEC_NAME} ${LIB_DIR}/xtbclient/libxtbclient.dylib)
elseif(UNIX)
    target_link_libraries(${EXEC_NAME} ${LIB_DIR}/xtbclient/libxtbclient.so)
endif(APPLE)

install(TARGETS ${EXEC_NAME} DESTINATION ${BIN_DIR}/)
