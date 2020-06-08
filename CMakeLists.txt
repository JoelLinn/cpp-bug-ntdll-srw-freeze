cmake_minimum_required (VERSION 3.8)

project ("cpp-bug-ntdll-srw-freeze")

add_executable (works "main_works.c")
add_executable (fails "main_fails.c")
