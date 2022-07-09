cmake_minimum_required(VERSION 3.22)
project(DetectOverFlow C)

set(CMAKE_C_STANDARD 23)

add_executable(DetectOverFlow main.c run_target_program.c run_tracer.c wait_for_systemcall.c header.h)
