include_directories(../include)

add_executable(render render.cpp)
target_link_libraries(render ray)
