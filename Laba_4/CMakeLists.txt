cmake_minimum_required(VERSION 3.10)
project(Laba_4)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Динамические библиотеки
add_library(basic SHARED lib/basic.cpp)
add_library(advanced SHARED lib/advanced.cpp)

# Статически линкуемая программа
add_executable(static_app app/static.cpp)
target_link_libraries(static_app basic)

# Динамически загружающая программа
add_executable(dynamic_app app/dynamic.cpp)
target_link_libraries(dynamic_app dl)

# Копирование библиотек в директорию с исполняемыми файлами
add_custom_command(TARGET dynamic_app POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:basic> ${CMAKE_CURRENT_BINARY_DIR}/libbasic.so
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:advanced> ${CMAKE_CURRENT_BINARY_DIR}/libadvanced.so
)