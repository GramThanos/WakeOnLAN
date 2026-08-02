cmake_minimum_required(VERSION 3.12)

project(WakeOnLAN C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()

add_executable(WakeOnLAN WakeOnLAN.c)

# Size optimizations for GCC / LLVM Clang (not AppleClang, whose linker
# rejects these flags): optimize for size, drop unused sections, strip symbols.
if(CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
	target_compile_options(WakeOnLAN PRIVATE -Os -ffunction-sections -fdata-sections)
	set_target_properties(WakeOnLAN PROPERTIES LINK_FLAGS "-Wl,--gc-sections -s")
endif()

if(WIN32)
	target_link_libraries(WakeOnLAN ws2_32)
endif()
