cmake_minimum_required(VERSION 3.22.1)
project(TuanMeta)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Architecture-specific
set(TARGET_ARCH_ABI ${CMAKE_ANDROID_ARCH_ABI})

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/ImGui
    ${CMAKE_SOURCE_DIR}/ImGui/backends
    ${CMAKE_SOURCE_DIR}/Includes
    ${CMAKE_SOURCE_DIR}/TuanMeta
    ${CMAKE_SOURCE_DIR}/Includes/Libraries/curl/${TARGET_ARCH_ABI}/include
    ${CMAKE_SOURCE_DIR}/Includes/Libraries/openssl/${TARGET_ARCH_ABI}/include
)

# Source files
set(SOURCE_FILES
    Main.cpp
    ImGui/imgui.cpp
    ImGui/imgui_draw.cpp
    ImGui/imgui_tables.cpp
    ImGui/imgui_widgets.cpp
    ImGui/backends/imgui_impl_opengl3.cpp
    ImGui/backends/imgui_impl_android.cpp
    Includes/KittyMemory/KittyMemory.cpp
    Includes/KittyMemory/MemoryPatch.cpp
    Includes/KittyMemory/MemoryBackup.cpp
    Includes/KittyMemory/KittyUtils.cpp
    Includes/KittyMemory/KittyScanner.cpp
    Includes/KittyMemory/KittyArm64.cpp
    Includes/xdl/xdl.c
    Includes/xdl/xdl_iterate.c
    Includes/xdl/xdl_linker.c
    Includes/xdl/xdl_lzma.c
    Includes/xdl/xdl_util.c
    Includes/oxorany/oxorany.cpp
    TuanMeta/Utils/Tools.cpp
    TuanMeta/IL2CppSDKGenerator/Il2Cpp.cpp
)

# Prebuilt static libraries
add_library(curl STATIC IMPORTED)
set_target_properties(curl PROPERTIES IMPORTED_LOCATION
    ${CMAKE_SOURCE_DIR}/Includes/Libraries/curl/${TARGET_ARCH_ABI}/lib/libcurl.a)

add_library(ssl STATIC IMPORTED)
set_target_properties(ssl PROPERTIES IMPORTED_LOCATION
    ${CMAKE_SOURCE_DIR}/Includes/Libraries/openssl/${TARGET_ARCH_ABI}/lib/libssl.a)

add_library(crypto STATIC IMPORTED)
set_target_properties(crypto PROPERTIES IMPORTED_LOCATION
    ${CMAKE_SOURCE_DIR}/Includes/Libraries/openssl/${TARGET_ARCH_ABI}/lib/libcrypto.a)

add_library(dobby STATIC IMPORTED)
set_target_properties(dobby PROPERTIES IMPORTED_LOCATION
    ${CMAKE_SOURCE_DIR}/Includes/Libraries/Dobby/${TARGET_ARCH_ABI}/libdobby.a)

set(CURL_LIB ${CMAKE_SOURCE_DIR}/Includes/Libraries/curl/${TARGET_ARCH_ABI}/lib/libcurl.a)
set(OPENSSL_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/Includes/Libraries/openssl/${TARGET_ARCH_ABI}/include)
set(SSL_BACKEND_USED OpenSSL)

add_library(TuanMeta SHARED ${SOURCE_FILES})

add_definitions(-DVERSION_NAME=${VERSION_NAME})

target_compile_options(TuanMeta PRIVATE
    -O2
    -fvisibility=hidden
    -ffunction-sections
    -fdata-sections
    -fstack-protector-strong
    -fPIE
    -fPIC
    -ftrapv
    -D_FORTIFY_SOURCE=2
    -Wno-error=format-security
)

target_link_options(TuanMeta PRIVATE
    -Wl,-z,relro
    -Wl,-z,now
    -Wl,-z,noexecstack
    -Wl,--gc-sections
    -pie
    -s
)

# Link libraries
target_link_libraries(TuanMeta
    curl
    ssl
    crypto
    dobby
    log
    android
    OpenSLES
    EGL
    GLESv3
    GLESv2
    GLESv1_CM
    z
)

set_target_properties(TuanMeta PROPERTIES CXX_EXCEPTIONS ON)