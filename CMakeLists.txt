CMAKE_MINIMUM_REQUIRED(VERSION 3.21)

PROJECT(cstf)

FUNCTION(add_example example)
    ADD_EXECUTABLE(${example} "./examples/${example}.cpp")
    TARGET_INCLUDE_DIRECTORIES(${example} SYSTEM PUBLIC "./include")

    SET_TARGET_PROPERTIES(${example} PROPERTIES CXX_STANDARD 23)
    SET_TARGET_PROPERTIES(${example} PROPERTIES LINKER_LANGUAGE CXX)
ENDFUNCTION()

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")

SET(CMAKE_CXX_FLAGS_RELEASE "-Os -flto")
SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -fsanitize=address,leak,undefined")

IF(PROJECT_IS_TOP_LEVEL)
    add_example(events)
    add_example(serialize)
ENDIF()