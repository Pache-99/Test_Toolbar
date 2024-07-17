cmake_minimum_required(VERSION 3.5.0)
project(Toolbar VERSION 0.1.0 LANGUAGES C CXX)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)
pkg_check_modules(SDL2_IMAGE REQUIRED SDL2_image)
pkg_check_modules(SDL3 REQUIRED sdl3)
pkg_check_modules(SDL3_IMAGE REQUIRED SDL3_image)

# message("SDL2_FOUND: ${SDL2_FOUND}")
# message("Header: ${SDL2_INCLUDE_DIRS}")
# message("Lib: ${SDL2_LIBRARIES}")

# message("SDL2_IMAGE_FOUND: ${SDL2_IMAGE_FOUND}")
# message("Header: ${SDL2_IMAGE_INCLUDE_DIRS}")
# message("Lib: ${SDL2_IMAGE_LIBRARIES}")

message("SDL3_FOUND: ${SDL3_FOUND}")
message("Header: ${SDL3_INCLUDE_DIRS}")
message("Lib: ${SDL3_LIBRARIES}")

message("SDL3_IMAGE_FOUND: ${SDL3_IMAGE_FOUND}")
message("Header: ${SDL3_IMAGE_INCLUDE_DIRS}")
message("Lib: ${SDL3_IMAGE_LIBRARIES}")

set(SOURCES
    main.c
)

add_executable(Toolbar ${SOURCES})

target_include_directories(Toolbar PRIVATE 
    # ${SDL2_INCLUDE_DIRS}
    # ${SDL2_IMAGE_INCLUDE_DIRS}
    ${SDL3_INCLUDE_DIRS}
    ${SDL3_IMAGE_INCLUDE_DIRS}
)

target_link_libraries(Toolbar PRIVATE 
    # ${SDL2_LIBRARIES}
    # ${SDL2_IMAGE_LIBRARIES}
    ${SDL3_LIBRARIES}
    ${SDL3_IMAGE_LIBRARIES}
)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
