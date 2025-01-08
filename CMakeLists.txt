# CMakeList.txt : CMake project for raytracingSimple, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("raytracingSimple")

# Add source to this project's executable.
add_executable (raytracing "main.cpp" "camera/boundingboxes.h" "camera/bvh.h" "texture/texture.h" "external/stb_image.h" "texture/rtwImage.h" "texture/perlin.h" "primitives/quad.h" "volumes/constantMedium.h")
add_custom_target(CopyEarthmap ALL
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/earthmap.jpg
    ${CMAKE_CURRENT_BINARY_DIR}
)
if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET raytracing PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
