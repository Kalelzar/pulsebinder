cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 17)

project(pulsebinder VERSION 0.0.1)

add_executable(pulsebinder exec/main.cpp)
target_link_libraries(pulsebinder PUBLIC pulsebind)

INSTALL(FILES ${CMAKE_HOME_DIRECTORY}/res/pulsebinder.1
  DESTINATION ${CMAKE_INSTALL_PREFIX}/share/man/man1)

INSTALL(TARGETS pulsebinder
  RUNTIME DESTINATION bin)
