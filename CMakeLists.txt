cmake_minimum_required(VERSION 3.12)

project(WakeOnLan C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(WakeOnLan WakeOnLAN.c)

if(WIN32)
	target_link_libraries(WakeOnLan ws2_32)
endif()
