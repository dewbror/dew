cmake_minimum_required(VERSION 3.5...3.10)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE TEST_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.c)

# Helper function: create a test executable with a given language standard
function(add_dewlog_test_target target_name language standard)
    add_executable(${target_name} ${TEST_SRC})

    if(language STREQUAL "CXX")
        # Force compiler to treat .c files as C++ via flag, not source property
        if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
            target_compile_options(${target_name} PRIVATE -x c++)
        elseif(MSVC)
            target_compile_options(${target_name} PRIVATE /TP)
        endif()
        set_target_properties(${target_name} PROPERTIES
            LINKER_LANGUAGE CXX
            CXX_STANDARD ${standard}
            CXX_STANDARD_REQUIRED YES
            CXX_EXTENSIONS NO
        )
    else()
        set_target_properties(${target_name} PROPERTIES
            C_STANDARD ${standard}
            C_STANDARD_REQUIRED YES
            C_EXTENSIONS NO
        )
    endif()
    # Apply your existing warning flags
    if(CMAKE_C_COMPILER_ID MATCHES "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        target_compile_options(${target_name} PRIVATE
            -pedantic -Wall -Wextra -Wshadow -Wundef
            -Wpointer-arith -Wcast-align -Wwrite-strings -Wcast-qual
            -Wno-format-zero-length
        )
        # C-only flags
        if(NOT language STREQUAL "CXX")
            target_compile_options(${target_name} PRIVATE
                -Wduplicated-cond -Wduplicated-branches -Wvla
                -pedantic -Wall -Wextra -fmessage-length=0 
                -Wno-unused-variable -Wfloat-equal 
                -Wundef -Wshadow -Wpointer-arith -Wcast-align 
                -Wwrite-strings -Wcast-qual 
                # -Wswitch-enum 
                -Wconversion -Wunreachable-code -Wunused-macros 
                -Wduplicated-cond -Wduplicated-branches
                -Wvector-operation-performance
                -Wsuggest-attribute=pure
                -Wsuggest-attribute=const
                -Wsuggest-attribute=noreturn
                -Wsuggest-attribute=malloc
                -Wsuggest-attribute=format
                -Wsuggest-attribute=cold
                -Wmissing-noreturn
                -Wmissing-format-attribute
                -fstack-protector-strong
                # suppress
                -Wno-format-zero-length
            )
        endif()
    elseif(CMAKE_C_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        target_compile_options(${target_name} PRIVATE
            -Wall -Wextra -Wpedantic -Wshadow -Wundef
        )
    elseif(MSVC)
        target_compile_options(${target_name} PRIVATE /W4)
    endif()

    # Build type flags (your existing logic)
    if(CMAKE_BUILD_TYPE MATCHES "Debug")
        target_compile_options(${target_name} PRIVATE -O0 -ggdb3)
    elseif(CMAKE_BUILD_TYPE MATCHES "DebWithAsan")
        target_compile_options(${target_name} PRIVATE -O0 -ggdb3 -fsanitize=address,undefined)
        target_link_options(${target_name} PRIVATE -fsanitize=address,undefined)
    elseif(CMAKE_BUILD_TYPE MATCHES "DebWithCov")
        target_compile_options(${target_name} PRIVATE -O0 -ggdb3 --coverage)
        target_link_options(${target_name} PRIVATE --coverage)
    endif()

    target_include_directories(${target_name} PRIVATE ${CMAKE_SOURCE_DIR})
endfunction()

# Create two test executables from the same sources
add_dewlog_test_target(dewlog_tests_c99  C   99)
add_dewlog_test_target(dewlog_tests_cxx11 CXX 11)

# Optional: register with CTest so you can run all three with `ctest`
add_test(NAME c99  COMMAND dewlog_tests_c99)
add_test(NAME cxx11 COMMAND dewlog_tests_cxx11)
