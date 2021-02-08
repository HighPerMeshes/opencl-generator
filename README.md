# OpenCL Code Generator

This is the OpenCL code generator for the [HighPerMeshes domain-specific language](https://github.com/HighPerMeshes/highpermeshes-dsl).

# Requirements

This tool is meant to be included as a Clang tool for LLVM 11. 
Further details on how to compile a Clang tool can be found [here](https://clang.llvm.org/docs/LibASTMatchersTutorial.html). 

## Usage

The tool can be used with `./hpm-opencl-transformation hpm-source-file`. Executing this creates two files, the device and host code for `hpm-source-file`, that must be integrated into a new project separately. 
Note that this only works with the sequential version of HighPerMeshes.
