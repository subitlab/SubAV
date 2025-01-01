cmake_minimum_required(VERSION 3.20)
project(SubAV)

add_library(sbavcore STATIC "")
target_compile_features(sbavcore PRIVATE cxx_std_20)
target_sources(sbavcore PRIVATE "AVCore/BitStream.hpp" "AVCore/DCT.hpp" "AVCore/MaxFOG.hpp" "AVCore/MacaqueMixture.hpp" "AVCore/OwlVision.hpp" "AVCore/DolphinAudition.hpp" "AVCore/IKP.hpp" "AVCore/SIMD.hpp"
                                "AVCore/BitStream.cpp" "AVCore/DCT.cpp" "AVCore/MaxFOG.cpp" "AVCore/MacaqueMixture.cpp" "AVCore/OwlVision.cpp" "AVCore/DolphinAudition.cpp" "AVCore/IKP.cpp"
)

add_executable(sbavtool "")
target_compile_features(sbavtool PUBLIC cxx_std_20)
target_sources(sbavtool PUBLIC "AVTool/FFmpeg.hpp" "AVTool/FFmpeg.cpp" "AVTool/Main.cpp")
target_link_libraries(sbavtool PUBLIC sbavcore)

