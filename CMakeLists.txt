cmake_minimum_required(VERSION 3.20)
project(SubAV)

add_library(sbavcore STATIC "")
target_compile_features(sbavcore PRIVATE cxx_std_20)
target_sources(sbavcore PRIVATE "AVCore/BitStream.hpp" "AVCore/DCT.hpp" "AVCore/MaxFOG.hpp" "AVCore/FrameSequence.hpp" "AVCore/StandaloneImage.hpp" "AVCore/JPEG.hpp"
    "AVCore/BitStream.cpp" "AVCore/DCT.cpp" "AVCore/MaxFOG.cpp" "AVCore/FrameSequence.cpp" "AVCore/StandaloneImage.cpp"
)

find_package(Vulkan REQUIRED)
# add_subdirectory(glfw)
add_executable(sbavtool "")
target_compile_features(sbavtool PUBLIC cxx_std_20)
target_sources(sbavtool PUBLIC "AVTool/RFSGenerator.hpp" "AVTool/SIGenerator.hpp" "AVTool/VideoPlayer.hpp"
                               "AVTool/RFSGenerator.cpp" "AVTool/SIGenerator.cpp" "AVTool/VideoPlayer.cpp"
                               "AVTool/Main.cpp")
target_link_libraries(sbavtool PUBLIC glfw Vulkan::Vulkan sbavcore)
