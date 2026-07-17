# Библиотека с функциями замены байтовых последовательностей
add_library(replace_lib
    src/replace.c
)

# Публичные заголовки
target_include_directories(replace_lib PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

# BLOCK_SIZE задаётся из корневого CMakeLists через -DBLOCK_SIZE=N.
if(DEFINED BLOCK_SIZE)
    target_compile_definitions(replace_lib PRIVATE BLOCK_SIZE=${BLOCK_SIZE})
endif()

# Имя выходного файла
set_target_properties(replace_lib PROPERTIES OUTPUT_NAME replace)
set_target_properties(replace_lib PROPERTIES PREFIX "")

# Для Windows при сборке динамической библиотеки подключаем .def
if(WIN32 AND BUILD_SHARED_LIBS)
    target_sources(replace_lib PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/lib.def")
endif()