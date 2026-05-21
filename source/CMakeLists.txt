cmake_minimum_required(VERSION 3.25.1)

if(POLICY CMP0144)
    cmake_policy(SET CMP0144 NEW)
endif()

if(POLICY CMP0167)
    cmake_policy(SET CMP0167 OLD)
endif()

project(twainsave-opensource VERSION 0.1.0 LANGUAGES C CXX)

# -----------------------------------------------------------------------------
# Boost bootstrap support
# -----------------------------------------------------------------------------
# TwainSave uses the prebuilt Boost binary installers from SourceForge on Windows.
# This block downloads and silently installs the matching Boost package for the
# selected Visual Studio toolset and platform.
#
# CMake-GUI workflow:
#   1. Press Configure once.
#   2. Set TWAIN_BOOST_CACHE_ROOT if the default is not desired.
#   3. Enable TWAIN_AUTO_DOWNLOAD_BOOST.
#   4. Press Configure again.
#   5. Press Generate.
#
# The Boost root is shared by Win32/x64 builds for the same MSVC toolset, e.g.:
#   C:/BoostDeps/boost_1_91_0_msvc_14.3/
#       boost/
#       lib32-msvc-14.3/
#       lib64-msvc-14.3/
# -----------------------------------------------------------------------------

set(TWAIN_BOOST_VERSION "1.91.0" CACHE STRING "Boost version to download/use")
string(REPLACE "." "_" TWAIN_BOOST_VERSION_UNDERSCORE "${TWAIN_BOOST_VERSION}")

set(TWAIN_BOOST_CACHE_ROOT
    "C:/BoostDeps"
    CACHE PATH
    "Short directory where Boost installers and installed Boost binaries are stored")

option(TWAIN_AUTO_DOWNLOAD_BOOST
       "Automatically download and silently install the matching Boost binary package if missing"
       OFF)

if(NOT MSVC)
    message(FATAL_ERROR "This CMakeLists.txt currently supports automatic Boost setup only for MSVC/Visual Studio builds.")
endif()

# Map Visual Studio toolset to the Boost binary naming convention.
if(MSVC_TOOLSET_VERSION MATCHES "^143")
    set(TWAIN_BOOST_MSVC "14.3") # Visual Studio 2022
elseif(MSVC_TOOLSET_VERSION MATCHES "^142")
    set(TWAIN_BOOST_MSVC "14.2") # Visual Studio 2019
else()
    message(FATAL_ERROR "Unsupported MSVC toolset version: ${MSVC_TOOLSET_VERSION}. Expected v142 or v143.")
endif()

# Determine the target platform bitness selected by CMake-GUI / generator.
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(TWAIN_BOOST_BITS "64")
else()
    set(TWAIN_BOOST_BITS "32")
endif()

set(TWAIN_EFFECTIVE_BOOST_ROOT
    "${TWAIN_BOOST_CACHE_ROOT}/boost_${TWAIN_BOOST_VERSION_UNDERSCORE}_msvc_${TWAIN_BOOST_MSVC}")

set(TWAIN_EFFECTIVE_BOOST_LIBDIR
    "${TWAIN_EFFECTIVE_BOOST_ROOT}/lib${TWAIN_BOOST_BITS}-msvc-${TWAIN_BOOST_MSVC}")

set(TWAIN_BOOST_EXE
    "boost_${TWAIN_BOOST_VERSION_UNDERSCORE}-msvc-${TWAIN_BOOST_MSVC}-${TWAIN_BOOST_BITS}.exe")

set(TWAIN_BOOST_URL
    "https://sourceforge.net/projects/boost/files/boost-binaries/${TWAIN_BOOST_VERSION}/${TWAIN_BOOST_EXE}/download")

set(TWAIN_BOOST_DOWNLOADED_EXE
    "${TWAIN_BOOST_CACHE_ROOT}/${TWAIN_BOOST_EXE}")

set(TWAIN_BOOST_INSTALL_LOG
    "${TWAIN_BOOST_CACHE_ROOT}/${TWAIN_BOOST_EXE}.install.log")

message(STATUS "Boost toolset selected: msvc-${TWAIN_BOOST_MSVC}")
message(STATUS "Boost architecture selected: ${TWAIN_BOOST_BITS}-bit")
message(STATUS "Boost root: ${TWAIN_EFFECTIVE_BOOST_ROOT}")
message(STATUS "Boost library dir: ${TWAIN_EFFECTIVE_BOOST_LIBDIR}")

# Check for the architecture-specific library directory.  This lets Win32 and x64
# installs share the same Boost root while still installing the missing bitness.
if(NOT EXISTS "${TWAIN_EFFECTIVE_BOOST_LIBDIR}")
    if(NOT TWAIN_AUTO_DOWNLOAD_BOOST)
        message(STATUS "")
        message(STATUS "Boost is not installed for this configuration:")
        message(STATUS "  ${TWAIN_EFFECTIVE_BOOST_LIBDIR}")
        message(STATUS "")
        message(STATUS "For CMake-GUI:")
        message(STATUS "  1. Set TWAIN_BOOST_CACHE_ROOT to a short path, if needed.")
        message(STATUS "     Example: C:/BoostDeps or D:/BoostDeps")
        message(STATUS "  2. Enable TWAIN_AUTO_DOWNLOAD_BOOST.")
        message(STATUS "  3. Press Configure again.")
        message(STATUS "")
        message(STATUS "For command-line CMake, add for example:")
        message(STATUS "  -DTWAIN_BOOST_CACHE_ROOT=D:/BoostDeps -DTWAIN_AUTO_DOWNLOAD_BOOST=ON")
        message(STATUS "")
        return()
    endif()

    file(MAKE_DIRECTORY "${TWAIN_BOOST_CACHE_ROOT}")

    if(NOT EXISTS "${TWAIN_BOOST_DOWNLOADED_EXE}")
        message(STATUS "Downloading Boost installer:")
        message(STATUS "  ${TWAIN_BOOST_URL}")
        message(STATUS "to:")
        message(STATUS "  ${TWAIN_BOOST_DOWNLOADED_EXE}")

        file(DOWNLOAD
            "${TWAIN_BOOST_URL}"
            "${TWAIN_BOOST_DOWNLOADED_EXE}"
            SHOW_PROGRESS
            STATUS boost_download_status
        )

        list(GET boost_download_status 0 boost_download_result)
        list(GET boost_download_status 1 boost_download_message)
        if(NOT boost_download_result EQUAL 0)
            message(FATAL_ERROR
                "Boost download failed.\n"
                "URL: ${TWAIN_BOOST_URL}\n"
                "Destination: ${TWAIN_BOOST_DOWNLOADED_EXE}\n"
                "Status: ${boost_download_status}\n"
                "Message: ${boost_download_message}")
        endif()
    else()
        message(STATUS "Using existing Boost installer: ${TWAIN_BOOST_DOWNLOADED_EXE}")
    endif()

    message(STATUS "Installing Boost to ${TWAIN_EFFECTIVE_BOOST_ROOT}")
    message(STATUS "Boost installer log: ${TWAIN_BOOST_INSTALL_LOG}")

    # Boost's Windows binary packages are Inno Setup installers.
    execute_process(
        COMMAND
            "${TWAIN_BOOST_DOWNLOADED_EXE}"
            /VERYSILENT
            /SUPPRESSMSGBOXES
            /NORESTART
            /NOCANCEL
            /SP-
            "/DIR=${TWAIN_EFFECTIVE_BOOST_ROOT}"
            "/LOG=${TWAIN_BOOST_INSTALL_LOG}"
        RESULT_VARIABLE boost_install_result
    )

    if(NOT boost_install_result EQUAL 0)
        message(FATAL_ERROR
            "Boost installer failed with exit code ${boost_install_result}.\n"
            "Installer: ${TWAIN_BOOST_DOWNLOADED_EXE}\n"
            "Log: ${TWAIN_BOOST_INSTALL_LOG}\n"
            "Try running the installer manually with the same command line.")
    endif()

    if(NOT EXISTS "${TWAIN_EFFECTIVE_BOOST_LIBDIR}")
        message(FATAL_ERROR
            "Boost installation completed, but the expected library directory was not found.\n"
            "Expected: ${TWAIN_EFFECTIVE_BOOST_LIBDIR}\n"
            "Installer log: ${TWAIN_BOOST_INSTALL_LOG}")
    endif()

    # Avoid reinstalling on later Configure runs unless the user explicitly turns it back on.
    set(TWAIN_AUTO_DOWNLOAD_BOOST OFF CACHE BOOL
        "Automatically download and silently install the matching Boost binary package if missing"
        FORCE)
endif()

# Make Boost discoverable without requiring user environment variables.
set(BOOST_ROOT "${TWAIN_EFFECTIVE_BOOST_ROOT}")
set(BOOST_INCLUDEDIR "${TWAIN_EFFECTIVE_BOOST_ROOT}")
set(BOOST_LIBRARYDIR "${TWAIN_EFFECTIVE_BOOST_LIBDIR}")
set(Boost_NO_SYSTEM_PATHS ON)
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)

find_package(Boost REQUIRED)

set(HEADER_FILES ${PROJECT_SOURCE_DIR}/c_cpp_includes/
        ${PROJECT_SOURCE_DIR}/c_cpp_includes/capconst.h
        ${PROJECT_SOURCE_DIR}/c_cpp_includes/dtwain.h
        ${PROJECT_SOURCE_DIR}/c_cpp_includes/dtwainc.h
        ${PROJECT_SOURCE_DIR}/c_cpp_includes/dtwaindefs.h
        ${PROJECT_SOURCE_DIR}/c_cpp_includes/dtwainx.h
        ${PROJECT_SOURCE_DIR}/c_cpp_includes/dtwainx2.h
        ${PROJECT_SOURCE_DIR}/c_cpp_includes/dtwain_filetypes.h
        ${PROJECT_SOURCE_DIR}/c_cpp_includes/dtwain_library_selector.h
        ${PROJECT_SOURCE_DIR}/c_cpp_includes/dtwain_standard_defs.h
        ${PROJECT_SOURCE_DIR}/c_cpp_includes/dtwain_version.h
        ${PROJECT_SOURCE_DIR}/c_cpp_includes/dtwpdft.h
        ${PROJECT_SOURCE_DIR}/c_cpp_includes/dtwstrfn.h
        ${PROJECT_SOURCE_DIR}/c_cpp_includes/dtwtype.h
        ${PROJECT_SOURCE_DIR}/c_cpp_includes/twain.h
        ${PROJECT_SOURCE_DIR}/c_cpp_includes/winconst.h
        ${PROJECT_SOURCE_DIR}/twainsave_broker.h
        ${PROJECT_SOURCE_DIR}/commandline.h
        ${PROJECT_SOURCE_DIR}/twainsave_app.h
        ${PROJECT_SOURCE_DIR}/twainsave.h
        ${PROJECT_SOURCE_DIR}/twainsave_verinfo.h
        ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/dynarithmic/twain/acquire_characteristics.hpp
        ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/dynarithmic/twain/capability_interface.hpp
        ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/dynarithmic/twain/compiler_details.hpp
        ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/dynarithmic/twain/dtwain_twain.hpp
        ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/dynarithmic/twain/twain_callback.hpp
        ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/dynarithmic/twain/twain_details.hpp
        ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/dynarithmic/twain/twain_session.hpp
        ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/dynarithmic/twain/twain_source.hpp
        ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/dynarithmic/twain/twain_values.hpp
        ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/dynarithmic/twain/acquire_characteristics/acquire_characteristics.hpp
        ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/dynarithmic/twain/capability_interface/capability_interface.hpp
        ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/dynarithmic/twain/characteristics/twain_select_dialog.hpp
        ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/dynarithmic/twain/extimageinfo/extendedimage_info.hpp
        ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/dynarithmic/twain/identity/twain_identity.hpp
        ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/dynarithmic/twain/imagehandler/image_handler.hpp
        ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/dynarithmic/twain/info/asyncdeviceevents_info.hpp
        ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/dynarithmic/twain/info/buffered_transfer_info.hpp
        ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/dynarithmic/twain/info/capinfo_base2.hpp
        ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/dynarithmic/twain/info/file_transfer_info.hpp
        ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/dynarithmic/twain/info/imageinformation_info.hpp
        ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/dynarithmic/twain/info/imprinter_info.hpp
        ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/dynarithmic/twain/info/paperhandling_info.hpp
        ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/dynarithmic/twain/logging/error_logger.hpp
        ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/dynarithmic/twain/logging/error_logger_details.hpp
        ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/dynarithmic/twain/logging/logger_callback.hpp
        ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/dynarithmic/twain/logging/twain_logger.hpp
        ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/dynarithmic/twain/messaging/twain_loop.hpp
        ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/dynarithmic/twain/options/audiblealarms_options.hpp
        ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/dynarithmic/twain/options/autoadjust_options.hpp
        ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/dynarithmic/twain/options/autocapture_options.hpp
        ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/dynarithmic/twain/options/autoscanning_options.hpp
        ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/dynarithmic/twain/options/barcodedetection_options.hpp
        ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/dynarithmic/twain/options/blankpage_options.hpp
        ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/dynarithmic/twain/options/buffered_transfer_options.hpp
        ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/dynarithmic/twain/options/capnegotiation_options.hpp
        ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/dynarithmic/twain/options/color_options.hpp
        ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/dynarithmic/twain/options/compression_options.hpp
        ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/dynarithmic/twain/options/deviceevents_options.hpp
        ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/dynarithmic/twain/options/deviceparams_options.hpp
        ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/dynarithmic/twain/options/doublefeed_options.hpp
        ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/dynarithmic/twain/options/file_transfer_options.hpp
        ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/dynarithmic/twain/options/general_options.hpp
        ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/dynarithmic/twain/options/imageinformation_options.hpp
        ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/dynarithmic/twain/options/imageparameter_options.hpp
        ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/dynarithmic/twain/options/imagetype_options.hpp
        ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/dynarithmic/twain/options/imprinter_options.hpp
        ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/dynarithmic/twain/options/jobcontrol_options.hpp
        ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/dynarithmic/twain/options/language_options.hpp
        ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/dynarithmic/twain/options/micr_options.hpp
        ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/dynarithmic/twain/options/options_base.hpp
        ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/dynarithmic/twain/options/pages_options.hpp
        ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/dynarithmic/twain/options/paperhandling_options.hpp
        ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/dynarithmic/twain/options/patchcode_options.hpp
        ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/dynarithmic/twain/options/pdf_options.hpp
        ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/dynarithmic/twain/options/powermonitor_options.hpp
        ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/dynarithmic/twain/options/resolution_options.hpp
        ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/dynarithmic/twain/options/ui_options.hpp
        ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/dynarithmic/twain/pdf/pdf_text_element.hpp
        ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/dynarithmic/twain/session/twain_characteristics.hpp
        ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/dynarithmic/twain/session/twain_session.hpp
        ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/dynarithmic/twain/session/twain_session_base.hpp
        ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/dynarithmic/twain/source/twain_source.hpp
        ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/dynarithmic/twain/source/twain_source_pimpl.hpp
        ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/dynarithmic/twain/tostring/tostring.hpp
        ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/dynarithmic/twain/types/twain_array.hpp
        ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/dynarithmic/twain/types/twain_callback.hpp
        ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/dynarithmic/twain/types/twain_capbasics.hpp
        ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/dynarithmic/twain/types/twain_frame.hpp
        ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/dynarithmic/twain/types/twain_range.hpp
        ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/dynarithmic/twain/types/twain_timer.hpp
        ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/dynarithmic/twain/types/twain_types.hpp
        ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/dynarithmic/twain/types/underlying_type.hpp
        ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/dynarithmic/twain/utilities/string_utilities.hpp
        ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/dynarithmic/twain/utilities/misc_utilities.hpp
        ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/dynarithmic/twain/types/constexpr_utils.hpp
        ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/dynarithmic/twain/types/eternal_map/include/mapbox/eternal.hpp
)
 
add_executable(twainsave-opensource
    ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/acquire_characteristics.cpp
    ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/buffered_transfer_info.cpp
    ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/extendedimage_info.cpp
    ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/imprinter_info.cpp
    ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/logger_callback.cpp
    ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/twain_characteristics.cpp
    ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/options_base.cpp
    ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/paperhandling_info.cpp
    ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/pdf_text_element.cpp
    ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/string_utilities.cpp
    ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/twain_callback.cpp
    ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/twain_session.cpp
    ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/twain_source.cpp
    ${PROJECT_SOURCE_DIR}/generate_details.cpp
    ${PROJECT_SOURCE_DIR}/stdafx.cpp
    ${PROJECT_SOURCE_DIR}/twainsave-opensource.cpp
    ${PROJECT_SOURCE_DIR}/twainsave_app.cpp
    ${PROJECT_SOURCE_DIR}/twainsave_broker.cpp
    ${PROJECT_SOURCE_DIR}/dtwimpl.cpp
    ${PROJECT_SOURCE_DIR}/commandline.cpp
    ${HEADER_FILES}
)

target_include_directories(twainsave-opensource PRIVATE
    ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib
    ${PROJECT_SOURCE_DIR}/c_cpp_includes
    ${Boost_INCLUDE_DIRS}
)

target_link_directories(twainsave-opensource PRIVATE
    ${BOOST_LIBRARYDIR}
)

target_compile_definitions(twainsave-opensource PRIVATE
    UNICODE
    _UNICODE
    DTWAIN_CPP_NOIMPORTLIB
)

set_property(TARGET twainsave-opensource PROPERTY CXX_STANDARD 17)
target_sources(twainsave-opensource PRIVATE ${PROJECT_SOURCE_DIR}/twainsave-opensource.rc)
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:__cplusplus")
endif()
add_custom_command(TARGET twainsave-opensource POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/twainsave.ini "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>"
                   )
add_custom_command(TARGET twainsave-opensource POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/twainsave-opensource.exe "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/twainsave_${CMAKE_GENERATOR_PLATFORM}.exe"
                   )
