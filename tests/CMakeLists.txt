# Исполняемый файл для тестов
add_executable(replace_tests
    src/test_replace.c
    src/test_main.c   # если есть отдельный драйвер (опционально)
)

# Подключаем заголовки библиотеки (чтобы видеть replace.h)
target_include_directories(replace_tests PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../lib/include"
)

# Линкуем с библиотекой
target_link_libraries(replace_tests PRIVATE replace_lib)

# Список имён тестов (соответствуют именам в массиве tests[] в test_replace.c)
set(TEST_NAMES
    simple_replace
    replace_at_start
    replace_at_end
    no_match
    fallback
    self_overlap
    null_in_search
    null_in_replace
    empty_replace
    empty_input
)

foreach(test_name ${TEST_NAMES})
    add_test(NAME ${test_name} COMMAND replace_tests ${test_name})
endforeach()