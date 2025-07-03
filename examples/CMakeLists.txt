FUNCTION(add_example example)
    ADD_EXECUTABLE(${example} "./${example}.cpp")
    TARGET_INCLUDE_DIRECTORIES(${example} SYSTEM PUBLIC "../include")

    SET_TARGET_PROPERTIES(${example} PROPERTIES CXX_STANDARD 23)
    SET_TARGET_PROPERTIES(${example} PROPERTIES LINKER_LANGUAGE CXX)
ENDFUNCTION()

add_example(cstf)
add_example(serialize)