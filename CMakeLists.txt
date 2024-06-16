cmake_minimum_required(VERSION 3.12)

project(WakeOnLan C)

file(GLOB sources *.c)
file(GLOB headers *.h)

add_executable(WakeOnLan ${sources} ${headers})
