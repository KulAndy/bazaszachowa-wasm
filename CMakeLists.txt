cmake_minimum_required(VERSION 3.10)

project(chess_wasm LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CHESS_LIBRARY_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/chess-library
)

set(SRC_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

function(add_wasm_target TARGET SOURCE)
    add_executable(${TARGET} ${SOURCE})

    target_include_directories(${TARGET}
        PRIVATE
        ${CHESS_LIBRARY_DIR}/include
    )

    target_compile_options(${TARGET}
        PRIVATE
        -O3
    )

    set_target_properties(${TARGET} PROPERTIES
        OUTPUT_NAME "${TARGET}"
        SUFFIX ".js"
        LINK_FLAGS
            "--bind -sMODULARIZE=1 -sEXPORT_ES6=1 -sENVIRONMENT=web -sALLOW_MEMORY_GROWTH=1"
    )
endfunction()

add_wasm_target(
    chess_processor
    ${SRC_DIR}/game2fens/main.cpp
)

add_wasm_target(
    uci2pgn
    ${SRC_DIR}/js-chess-uci2san/main.cpp
)

add_wasm_target(
    game_stats
    ${SRC_DIR}/game_stats/main.cpp
)

add_wasm_target(
    stats
    ${SRC_DIR}/stats/main.cpp
)
