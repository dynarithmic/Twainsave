cmake_minimum_required(VERSION 3.25.1)
project(twainsave-opensource VERSION 0.1.0 LANGUAGES C CXX)
include_directories(${PROJECT_SOURCE_DIR}/cpp_wrapper_lib
                    ${PROJECT_SOURCE_DIR}/c_cpp_includes
                    $ENV{BOOST_INCLUDE_DIR})
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
 
link_directories($ENV{BOOST_LIBRARY_DIR_V142_64}
                 $ENV{BOOST_LIBRARY_DIR_V142_32}
                 $ENV{BOOST_LIBRARY_DIR_V143_64}
                 $ENV{BOOST_LIBRARY_DIR_V143_32})
add_definitions(-DUNICODE -D_UNICODE -DDTWAIN_CPP_NOIMPORTLIB)
add_executable(twainsave-opensource
    ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/acquire_characteristics.cpp
    ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/buffered_transfer_info.cpp
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
    ${PROJECT_SOURCE_DIR}/dtwimpl.cpp
    ${HEADER_FILES}
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
