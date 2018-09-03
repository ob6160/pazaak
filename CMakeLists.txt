cmake_minimum_required(VERSION 3.7)

project(pazaak)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${pazaak_SOURCE_DIR}/cmake")

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)

include_directories(${SDL2_INCLUDE_DIR}
                    ${SDL2_IMAGE_INCLUDE_DIR}
                    ${SDL2_TTF_INCLUDE_DIR})

add_executable(pazaak main.cpp pazaak.cpp card.cpp player.cpp button.cpp constants.h helpers.h)
target_link_libraries(pazaak ${SDL2_LIBRARY}
                             ${SDL2_IMAGE_LIBRARIES}
                             ${SDL2_TTF_LIBRARIES})

