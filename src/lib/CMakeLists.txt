include_directories(../include)
add_definitions(-fno-rtti)

add_library(ray
  bitmap.cpp
  objects.cpp
  scene.cpp
  scene-generators.cpp
  support.cpp
  test.cpp
  )
