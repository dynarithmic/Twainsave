cmake_minimum_required(VERSION 3.25.1)

if(POLICY CMP0144)
    cmake_policy(SET CMP0144 NEW)
endif()

if(POLICY CMP0167)
    cmake_policy(SET CMP0167 OLD)
endif()

project(twainsave-opensource VERSION 1.4.1 LANGUAGES C CXX)

option(USE_DTWAIN_DEBUG_LIB
       "Build TwainSave to use the DTWAIN debug DLL (advanced)"
       OFF)

mark_as_advanced(USE_DTWAIN_DEBUG_LIB)
# -----------------------------------------------------------------------------
# Boost setup
# -----------------------------------------------------------------------------
# This project can download/install the matching prebuilt Boost binary package
# for the current Visual Studio toolset and platform.
#
# CMake-Gui workflow:
#   1. Press Configure once.
#   2. Set TWAIN_BOOST_CACHE_ROOT if you do not want the default C:/BoostDeps.
#   3. Set TWAIN_AUTO_DOWNLOAD_BOOST=ON.
#   4. Press Configure again, then Generate.
#
# The Boost install root is shared by Visual Studio toolset and bitness, for example:
#   C:/BoostDeps/boost_1_91_0/
#       boost/
#       lib32-msvc-14.2/
#       lib64-msvc-14.2/
#       lib32-msvc-14.3/
#       lib64-msvc-14.3/
#
# This avoids installing duplicate Boost headers/docs for Win32/x64 and VS2019/VS2022.
# Do not run multiple Boost installers into the same root concurrently.

set(TWAIN_BOOST_VERSION "1.91.0" CACHE STRING "Boost version to use")
set(TWAIN_BOOST_VERSION_UNDERSCORE "1_91_0")

set(TWAIN_BOOST_CACHE_ROOT
    "C:/BoostDeps"
    CACHE PATH
    "Short directory where Boost installers and binaries are stored")

option(TWAIN_AUTO_DOWNLOAD_BOOST
       "Automatically download/install the required Boost binary package if missing"
       OFF)

option(TWAIN_DELETE_BOOST_INSTALLER_AFTER_INSTALL
       "Delete the downloaded Boost installer after a successful automatic install"
       OFF)

option(TWAIN_DELETE_BOOST_INSTALL_LOG_AFTER_INSTALL
       "Delete the Boost installer log after a successful automatic install"
       OFF)

set(TWAIN_EXISTING_BOOST_ROOT
    ""
    CACHE PATH
    "Optional existing Boost root folder. If set, no Boost download/install is performed.")

if(NOT MSVC)
    message(FATAL_ERROR
        "This CMakeLists.txt currently expects MSVC because it downloads "
        "Boost Windows binaries for Visual Studio.")
endif()

# Visual Studio / MSVC Boost binary mapping:
#   VS 2019 v142 => msvc-14.2
#   VS 2022 v143 => msvc-14.3
#   VS 2026 v145 => msvc-14.5
if(MSVC_TOOLSET_VERSION MATCHES "^143")
    set(TWAIN_BOOST_MSVC "14.3")
elseif(MSVC_TOOLSET_VERSION MATCHES "^142")
    set(TWAIN_BOOST_MSVC "14.2")
elseif(MSVC_TOOLSET_VERSION MATCHES "^145")
    set(TWAIN_BOOST_MSVC "14.5")
else()
    message(FATAL_ERROR
        "Unsupported MSVC toolset version '${MSVC_TOOLSET_VERSION}'. "
        "Expected v142, v143, or v145.")
endif()

# Platform mapping. CMAKE_SIZEOF_VOID_P is valid after project() enables C/CXX.
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(TWAIN_BOOST_BITS "64")
else()
    set(TWAIN_BOOST_BITS "32")
endif()

if(TWAIN_EXISTING_BOOST_ROOT)
    file(TO_CMAKE_PATH
         "${TWAIN_EXISTING_BOOST_ROOT}"
         TWAIN_EFFECTIVE_BOOST_ROOT)
else()
    set(TWAIN_EFFECTIVE_BOOST_ROOT
        "${TWAIN_BOOST_CACHE_ROOT}/boost_${TWAIN_BOOST_VERSION_UNDERSCORE}")
endif()

set(TWAIN_REQUIRED_BOOST_LIBRARY_DIR
    "${TWAIN_EFFECTIVE_BOOST_ROOT}/lib${TWAIN_BOOST_BITS}-msvc-${TWAIN_BOOST_MSVC}")

if(TWAIN_EXISTING_BOOST_ROOT)
    if(NOT EXISTS "${TWAIN_EFFECTIVE_BOOST_ROOT}/boost")
        message(FATAL_ERROR
            "Existing Boost root does not contain the Boost header directory:\n"
            "  ${TWAIN_EFFECTIVE_BOOST_ROOT}/boost")
    endif()

    if(NOT EXISTS "${TWAIN_REQUIRED_BOOST_LIBRARY_DIR}")
        message(FATAL_ERROR
            "Existing Boost root does not contain the required library directory for this build:\n"
            "  ${TWAIN_REQUIRED_BOOST_LIBRARY_DIR}\n\n"
            "Install the matching Boost binary package or choose automatic Boost download.")
    endif()
endif()

if(TWAIN_EXISTING_BOOST_ROOT)
    # User-supplied Boost installation. Expected layout:
    #   <root>/boost/version.hpp
    #   <root>/lib32-msvc-14.x  or  <root>/lib64-msvc-14.x
    set(TWAIN_EFFECTIVE_BOOST_ROOT "${TWAIN_EXISTING_BOOST_ROOT}")
else()
    file(MAKE_DIRECTORY "${TWAIN_BOOST_CACHE_ROOT}")

    # Shared Boost root for this Boost version.
    # VS toolset and bitness are represented only by the library directory name:
    #   lib32-msvc-14.2, lib64-msvc-14.2, lib32-msvc-14.3, lib64-msvc-14.3, etc.
    set(TWAIN_EFFECTIVE_BOOST_ROOT
        "${TWAIN_BOOST_CACHE_ROOT}/boost_${TWAIN_BOOST_VERSION_UNDERSCORE}")
endif()

set(TWAIN_BOOST_INCLUDE_DIR "${TWAIN_EFFECTIVE_BOOST_ROOT}")
set(TWAIN_BOOST_LIBRARY_DIR
    "${TWAIN_EFFECTIVE_BOOST_ROOT}/lib${TWAIN_BOOST_BITS}-msvc-${TWAIN_BOOST_MSVC}")

set(TWAIN_BOOST_INSTALLER
    "boost_${TWAIN_BOOST_VERSION_UNDERSCORE}-msvc-${TWAIN_BOOST_MSVC}-${TWAIN_BOOST_BITS}.exe")

set(TWAIN_BOOST_URL
    "https://sourceforge.net/projects/boost/files/boost-binaries/${TWAIN_BOOST_VERSION}/${TWAIN_BOOST_INSTALLER}/download")

set(TWAIN_BOOST_DOWNLOADED_EXE
    "${TWAIN_BOOST_CACHE_ROOT}/${TWAIN_BOOST_INSTALLER}")

set(TWAIN_BOOST_INSTALL_LOG
    "${TWAIN_BOOST_CACHE_ROOT}/${TWAIN_BOOST_INSTALLER}.install.log")

set(TWAIN_BOOST_HEADERS_FOUND FALSE)
if(EXISTS "${TWAIN_BOOST_INCLUDE_DIR}/boost/version.hpp")
    set(TWAIN_BOOST_HEADERS_FOUND TRUE)
endif()

set(TWAIN_BOOST_LIBDIR_FOUND FALSE)
if(EXISTS "${TWAIN_BOOST_LIBRARY_DIR}")
    set(TWAIN_BOOST_LIBDIR_FOUND TRUE)
endif()

if(NOT TWAIN_EXISTING_BOOST_ROOT AND
   (NOT TWAIN_BOOST_HEADERS_FOUND OR NOT TWAIN_BOOST_LIBDIR_FOUND))

    if(NOT TWAIN_AUTO_DOWNLOAD_BOOST)
        message(STATUS "")
        message(STATUS "Boost is not installed for this configuration yet.")
        message(STATUS "")
        message(STATUS "Expected Boost root:")
        message(STATUS "  ${TWAIN_EFFECTIVE_BOOST_ROOT}")
        message(STATUS "Expected Boost library directory:")
        message(STATUS "  ${TWAIN_BOOST_LIBRARY_DIR}")
        message(STATUS "")
        message(STATUS "For CMake-Gui:")
        message(STATUS "  Option A: set TWAIN_EXISTING_BOOST_ROOT to an existing Boost root")
        message(STATUS "  Option B:")
        message(STATUS "    1. Set TWAIN_BOOST_CACHE_ROOT if desired, for example D:/BoostDeps")
        message(STATUS "    2. Set TWAIN_AUTO_DOWNLOAD_BOOST=ON")
        message(STATUS "    3. Optionally set TWAIN_DELETE_BOOST_INSTALLER_AFTER_INSTALL=ON")
        message(STATUS "    4. Optionally set TWAIN_DELETE_BOOST_INSTALL_LOG_AFTER_INSTALL=ON")
        message(STATUS "    5. Press Configure again")
        message(STATUS "")
        message(STATUS "For command-line CMake, add:")
        message(STATUS "  Existing Boost: -DTWAIN_EXISTING_BOOST_ROOT=D:/boost_1_90_0")
        message(STATUS "  Auto download:  -DTWAIN_BOOST_CACHE_ROOT=D:/BoostDeps -DTWAIN_AUTO_DOWNLOAD_BOOST=ON")
        message(STATUS "  Delete installer/log after install:")
        message(STATUS "    -DTWAIN_DELETE_BOOST_INSTALLER_AFTER_INSTALL=ON")
        message(STATUS "    -DTWAIN_DELETE_BOOST_INSTALL_LOG_AFTER_INSTALL=ON")
        message(STATUS "")
        return()
    endif()

    if(NOT EXISTS "${TWAIN_BOOST_DOWNLOADED_EXE}")
        message(STATUS "Downloading ${TWAIN_BOOST_INSTALLER}")
        file(DOWNLOAD
            "${TWAIN_BOOST_URL}"
            "${TWAIN_BOOST_DOWNLOADED_EXE}"
            SHOW_PROGRESS
            STATUS TWAIN_BOOST_DOWNLOAD_STATUS
            TLS_VERIFY ON)

        list(GET TWAIN_BOOST_DOWNLOAD_STATUS 0 TWAIN_BOOST_DOWNLOAD_RESULT)
        list(GET TWAIN_BOOST_DOWNLOAD_STATUS 1 TWAIN_BOOST_DOWNLOAD_MESSAGE)
        if(NOT TWAIN_BOOST_DOWNLOAD_RESULT EQUAL 0)
            message(FATAL_ERROR
                "Boost download failed: ${TWAIN_BOOST_DOWNLOAD_MESSAGE}\n"
                "URL: ${TWAIN_BOOST_URL}\n"
                "Destination: ${TWAIN_BOOST_DOWNLOADED_EXE}")
        endif()
    else()
        message(STATUS "Using existing downloaded Boost installer: ${TWAIN_BOOST_DOWNLOADED_EXE}")
    endif()

    message(STATUS "Installing Boost to ${TWAIN_EFFECTIVE_BOOST_ROOT}")
    execute_process(
        COMMAND "${TWAIN_BOOST_DOWNLOADED_EXE}"
                /VERYSILENT
                /SUPPRESSMSGBOXES
                /NORESTART
                /NOCANCEL
                /SP-
                "/DIR=${TWAIN_EFFECTIVE_BOOST_ROOT}"
                "/LOG=${TWAIN_BOOST_INSTALL_LOG}"
        RESULT_VARIABLE TWAIN_BOOST_INSTALL_RESULT)

    if(NOT TWAIN_BOOST_INSTALL_RESULT EQUAL 0)
        message(FATAL_ERROR
            "Boost installer failed with exit code ${TWAIN_BOOST_INSTALL_RESULT}.\n"
            "Installer: ${TWAIN_BOOST_DOWNLOADED_EXE}\n"
            "Log: ${TWAIN_BOOST_INSTALL_LOG}")
    endif()

    # Optional cleanup after a successful install.  These are OFF by default so
    # the installer and log remain available for troubleshooting unless the user
    # explicitly chooses to remove them.
    if(TWAIN_DELETE_BOOST_INSTALLER_AFTER_INSTALL AND
       EXISTS "${TWAIN_BOOST_DOWNLOADED_EXE}")
        file(REMOVE "${TWAIN_BOOST_DOWNLOADED_EXE}")
        message(STATUS "Deleted Boost installer: ${TWAIN_BOOST_DOWNLOADED_EXE}")
    endif()

    if(TWAIN_DELETE_BOOST_INSTALL_LOG_AFTER_INSTALL AND
       EXISTS "${TWAIN_BOOST_INSTALL_LOG}")
        file(REMOVE "${TWAIN_BOOST_INSTALL_LOG}")
        message(STATUS "Deleted Boost installer log: ${TWAIN_BOOST_INSTALL_LOG}")
    endif()

    # Make a later CMake-Gui Configure less likely to re-run installers accidentally.
    set(TWAIN_AUTO_DOWNLOAD_BOOST OFF CACHE BOOL
        "Automatically download/install the required Boost binary package if missing"
        FORCE)
endif()

if(NOT EXISTS "${TWAIN_BOOST_INCLUDE_DIR}/boost/version.hpp")
    message(FATAL_ERROR "Boost headers were not found at: ${TWAIN_BOOST_INCLUDE_DIR}")
endif()

if(NOT EXISTS "${TWAIN_BOOST_LIBRARY_DIR}")
    message(FATAL_ERROR "Boost library directory was not found at: ${TWAIN_BOOST_LIBRARY_DIR}")
endif()

# These are CMake variables, not machine-wide environment variables. CMake writes
# the resulting include/library paths into the generated Visual Studio project.
set(BOOST_ROOT "${TWAIN_EFFECTIVE_BOOST_ROOT}")
set(BOOST_INCLUDEDIR "${TWAIN_BOOST_INCLUDE_DIR}")
set(BOOST_LIBRARYDIR "${TWAIN_BOOST_LIBRARY_DIR}")

message(STATUS "Boost root: ${TWAIN_EFFECTIVE_BOOST_ROOT}")
message(STATUS "Boost include dir: ${TWAIN_BOOST_INCLUDE_DIR}")
message(STATUS "Boost library dir: ${TWAIN_BOOST_LIBRARY_DIR}")
if(TWAIN_EXISTING_BOOST_ROOT)
    message(STATUS "Boost mode: existing user-supplied installation")
else()
    message(STATUS "Boost mode: TwainSave cache / auto-download")
    if(EXISTS "${TWAIN_BOOST_DOWNLOADED_EXE}")
        message(STATUS "Boost installer: ${TWAIN_BOOST_DOWNLOADED_EXE}")
    endif()
    if(EXISTS "${TWAIN_BOOST_INSTALL_LOG}")
        message(STATUS "Boost installer log: ${TWAIN_BOOST_INSTALL_LOG}")
    endif()
endif()

set(TEXT_RESOURCE_FILES
  ${PROJECT_SOURCE_DIR}/text_resources/dtwain32.ini
  ${PROJECT_SOURCE_DIR}/text_resources/dtwain64.ini
  ${PROJECT_SOURCE_DIR}/text_resources/twaininfo.txt
  ${PROJECT_SOURCE_DIR}/text_resources/twainlanguage.txt
  ${PROJECT_SOURCE_DIR}/text_resources/twainsave.ini
  ${PROJECT_SOURCE_DIR}/text_resources/twainresourcestrings_dutch.txt
  ${PROJECT_SOURCE_DIR}/text_resources/twainresourcestrings_english.txt
  ${PROJECT_SOURCE_DIR}/text_resources/twainresourcestrings_french.txt
  ${PROJECT_SOURCE_DIR}/text_resources/twainresourcestrings_german.txt
  ${PROJECT_SOURCE_DIR}/text_resources/twainresourcestrings_greek.txt
  ${PROJECT_SOURCE_DIR}/text_resources/twainresourcestrings_italian.txt
  ${PROJECT_SOURCE_DIR}/text_resources/twainresourcestrings_japanese.txt
  ${PROJECT_SOURCE_DIR}/text_resources/twainresourcestrings_korean.txt
  ${PROJECT_SOURCE_DIR}/text_resources/twainresourcestrings_portuguese.txt
  ${PROJECT_SOURCE_DIR}/text_resources/twainresourcestrings_portuguese_br.txt
  ${PROJECT_SOURCE_DIR}/text_resources/twainresourcestrings_romanian.txt
  ${PROJECT_SOURCE_DIR}/text_resources/twainresourcestrings_russian.txt
  ${PROJECT_SOURCE_DIR}/text_resources/twainresourcestrings_simplified_chinese.txt
  ${PROJECT_SOURCE_DIR}/text_resources/twainresourcestrings_spanish.txt
  ${PROJECT_SOURCE_DIR}/text_resources/twainresourcestrings_traditional_chinese.txt
  ${PROJECT_SOURCE_DIR}/text_resources/twainresourcestrings_turkish.txt
  ${PROJECT_SOURCE_DIR}/text_resources/twaincustomresources_dutch.txt
  ${PROJECT_SOURCE_DIR}/text_resources/twaincustomresources_english.txt
  ${PROJECT_SOURCE_DIR}/text_resources/twaincustomresources_french.txt
  ${PROJECT_SOURCE_DIR}/text_resources/twaincustomresources_german.txt
  ${PROJECT_SOURCE_DIR}/text_resources/twaincustomresources_greek.txt
  ${PROJECT_SOURCE_DIR}/text_resources/twaincustomresources_italian.txt
  ${PROJECT_SOURCE_DIR}/text_resources/twaincustomresources_japanese.txt
  ${PROJECT_SOURCE_DIR}/text_resources/twaincustomresources_korean.txt
  ${PROJECT_SOURCE_DIR}/text_resources/twaincustomresources_portuguese.txt
  ${PROJECT_SOURCE_DIR}/text_resources/twaincustomresources_portuguese_br.txt
  ${PROJECT_SOURCE_DIR}/text_resources/twaincustomresources_romanian.txt
  ${PROJECT_SOURCE_DIR}/text_resources/twaincustomresources_russian.txt
  ${PROJECT_SOURCE_DIR}/text_resources/twaincustomresources_simplified_chinese.txt
  ${PROJECT_SOURCE_DIR}/text_resources/twaincustomresources_spanish.txt
  ${PROJECT_SOURCE_DIR}/text_resources/twaincustomresources_traditional_chinese.txt
  ${PROJECT_SOURCE_DIR}/text_resources/twaincustomresources_turkish.txt
)

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
        ${PROJECT_SOURCE_DIR}/get_dll_version.h
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
    ${PROJECT_SOURCE_DIR}/get_dll_version.cpp
    ${HEADER_FILES}
)

target_include_directories(twainsave-opensource PRIVATE
    ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib
    ${PROJECT_SOURCE_DIR}/c_cpp_includes
    ${TWAIN_BOOST_INCLUDE_DIR}
)

target_sources(twainsave-opensource PRIVATE ${TEXT_RESOURCE_FILES})

target_link_libraries(twainsave-opensource PRIVATE Version)

source_group(
    "Text Resources" FILES ${TEXT_RESOURCE_FILES}
)

target_link_directories(twainsave-opensource PRIVATE
    ${TWAIN_BOOST_LIBRARY_DIR}
)

if(USE_DTWAIN_DEBUG_LIB)
    target_compile_definitions(twainsave-opensource PRIVATE USE_DTWAIN_DEBUG_LIB)
endif()

target_compile_definitions(twainsave-opensource PRIVATE
    UNICODE
    _UNICODE
    DTWAIN_CPP_NOIMPORTLIB
)

set_property(TARGET twainsave-opensource PROPERTY CXX_STANDARD 17)
target_sources(twainsave-opensource PRIVATE ${PROJECT_SOURCE_DIR}/twainsave-opensource.rc)
if(MSVC)
    target_compile_options(twainsave-opensource PRIVATE /Zc:__cplusplus)
endif()

set_property(TARGET twainsave-opensource PROPERTY
    MSVC_RUNTIME_LIBRARY
    "$<$<CONFIG:Debug>:MultiThreadedDebug>$<$<NOT:$<CONFIG:Debug>>:MultiThreaded>"
)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(TWAINSAVE_EXE_NAME "twainsave64.exe")
    set(TWAINSAVE_EXE_NAME2 "twainsave64.exe")
else()
    set(TWAINSAVE_EXE_NAME "twainsave.exe")
endif()

add_custom_command(TARGET twainsave-opensource POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/twainsave-opensource.exe
        ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/${TWAINSAVE_EXE_NAME}
    COMMAND ${CMAKE_COMMAND} -E copy
      ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/twainsave-opensource.exe
      ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/${TWAINSAVE_EXE_NAME2}
)

add_custom_command(TARGET twainsave-opensource POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/text_resources/twainsave.ini "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>"
                   )

