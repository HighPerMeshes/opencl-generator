add_clang_executable(hpm-tests tests.cpp)

target_link_libraries(hpm-tests PRIVATE
  clangTooling
  clangBasic
  clangASTMatchers
)

target_include_directories( hpm-tests PRIVATE ../include )

target_compile_options( hpm-tests PRIVATE -Werror=return-type)