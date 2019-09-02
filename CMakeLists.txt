project(VideoReader LANGUAGES CXX)

# set(CMAKE_BUILD_TYPE debug)

add_subdirectory(reader)

add_executable(main main.cpp)

find_library(avformat avformat)
find_library(avcodec avcodec)
find_library(avutil avutil)
find_library(swscale swscale)

find_library(opencv_core opencv_core)
find_library(opencv_highgui opencv_highgui)

target_link_libraries(
    main
    videoreader
    ${avformat}
    ${avcodec}
    ${avutil}
    ${swscale}
    ${opencv_core}
    ${opencv_highgui}
)