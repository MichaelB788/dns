# lib
set(SOURCE_LIB ${CMAKE_PROJECT_NAME}Source)
add_library(${SOURCE_LIB}
  dns_query.c
  hostname.c
)

# exe
set(EXE "client")
add_executable(${EXE} main.c)
target_link_libraries(${EXE} PRIVATE ${SOURCE_LIB})
target_compile_options(${EXE} PRIVATE -Wall -Wextra -Wpedantic -Werror)
