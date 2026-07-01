cmake_minimum_required(VERSION 3.21)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (NOT DEFINED CMAKE_OSX_ARCHITECTURES)
    set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "" FORCE)
endif()

project(Trakines VERSION 1.0.0)

# Trakines is Windows-only because Spout2 (D3D/OpenGL interop) is Windows-only.
if (NOT WIN32)
    message(FATAL_ERROR "Trakines only supports Windows (Spout2 dependency).")
endif()

include(FetchContent)

# --- Bundle the Spout2 SDK automatically (nothing to download by hand) -------
# We compile only the SpoutGL portion of the SDK directly into the mod so the
# final .geode is self-contained and requires no external DLLs.
FetchContent_Declare(
    Spout2
    GIT_REPOSITORY https://github.com/leadedge/Spout2.git
    GIT_TAG        2.007.015
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(Spout2)

set(SPOUT_SDK_DIR "${spout2_SOURCE_DIR}/SPOUTSDK/SpoutGL")

# Glob the actual .cpp files present in this SDK tag. The file set differs
# between versions (e.g. there is no SpoutCommon.cpp), so a hardcoded list is
# fragile. GLOB here is fine because the SDK is fetched at configure time.
file(GLOB SPOUT_SOURCES "${SPOUT_SDK_DIR}/*.cpp")

if (NOT SPOUT_SOURCES)
    message(FATAL_ERROR "No Spout2 sources found in ${SPOUT_SDK_DIR}")
endif()

file(GLOB_RECURSE SOURCES
    src/*.cpp
)

add_library(${PROJECT_NAME} SHARED ${SOURCES} ${SPOUT_SOURCES})

# Include SpoutGL so <SpoutGL/Spout.h> resolves. The SDK's headers reference
# each other relatively from the SPOUTSDK root, so add both.
target_include_directories(${PROJECT_NAME} PRIVATE
    src
    "${spout2_SOURCE_DIR}/SPOUTSDK"
    "${SPOUT_SDK_DIR}"
)

# Spout2 needs these Windows libs for D3D interop and OpenGL.
target_link_libraries(${PROJECT_NAME}
    opengl32
    d3d11
    dxgi
)

if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "Unable to find Geode SDK! Set GEODE_SDK env var to the Geode SDK path.")
else()
    message(STATUS "Found Geode: $ENV{GEODE_SDK}")
endif()

add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)

setup_geode_mod(${PROJECT_NAME})
