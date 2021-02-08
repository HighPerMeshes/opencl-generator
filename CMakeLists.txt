set(LLVM_LINK_COMPONENTS support)

set(CMAKE_CXX_STANDARD 17)

add_clang_executable(hpm-opencl-transformation hpm-opencl-transformation.cpp)

target_link_libraries(hpm-opencl-transformation PRIVATE
  clangTooling
  clangBasic
  clangASTMatchers
)

target_include_directories(
  hpm-opencl-transformation
  PRIVATE include)

target_compile_options(
  hpm-opencl-transformation
  PRIVATE -Werror=return-type
)

add_subdirectory(tests)
