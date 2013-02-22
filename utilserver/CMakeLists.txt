CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

SET(utilserver_sources
    database.cpp
    utilserver.cpp
    utilserverconfig.cpp
    worldserver.cpp
)

SET(utilserver_headers
    database.h
    utilserverconfig.h
    worldserver.h
)

ADD_EXECUTABLE(utilserver ${utilserver_sources} ${utilserver_headers})

TARGET_LINK_LIBRARIES(utilserver Common debug ${MySQL_LIBRARY_DEBUG} optimized ${MySQL_LIBRARY_RELEASE})

IF(MSVC)
    SET_TARGET_PROPERTIES(utilserver PROPERTIES LINK_FLAGS_RELEASE "/OPT:REF /OPT:ICF")
    TARGET_LINK_LIBRARIES(utilserver "Ws2_32.lib")
ENDIF(MSVC)

IF(MINGW)
    TARGET_LINK_LIBRARIES(utilserver "WS2_32")
ENDIF(MINGW)

IF(UNIX)
    TARGET_LINK_LIBRARIES(utilserver "dl")
    TARGET_LINK_LIBRARIES(utilserver "z")
    TARGET_LINK_LIBRARIES(utilserver "m")
    TARGET_LINK_LIBRARIES(utilserver "rt")
    TARGET_LINK_LIBRARIES(utilserver "pthread")
    ADD_DEFINITIONS(-fPIC)
ENDIF(UNIX)

SET(EXECUTABLE_OUTPUT_PATH ../Bin)