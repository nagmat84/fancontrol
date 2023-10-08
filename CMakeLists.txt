cmake_minimum_required(VERSION 3.0)

project(amdgpu-fan-control)

add_executable(amdgpu-fan-control main.cpp)

install(TARGETS amdgpu-fan-control RUNTIME DESTINATION bin)
