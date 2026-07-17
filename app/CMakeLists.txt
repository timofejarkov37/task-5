# Исполняемый файл приложения
add_executable(replace
    src/main.c
)

# Приватные заголовки (если есть локальные хедеры)
target_include_directories(replace PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

# Линкуем с библиотекой
target_link_libraries(replace PRIVATE replace_lib)