cmake_minimum_required(VERSION 3.25.1)
project(twainsave-opensource VERSION 0.1.0 LANGUAGES C CXX)
include_directories(${PROJECT_SOURCE_DIR}/cpp_wrapper_lib
                    ${PROJECT_SOURCE_DIR}/c_cpp_includes
                    $ENV{BOOST_INCLUDE_DIR})
link_directories($ENV{DTWAIN_LIBRARY_DIR}
                 $ENV{BOOST_LIBRARY_DIR_V142_64}
                 $ENV{BOOST_LIBRARY_DIR_V142_32}
                 $ENV{BOOST_LIBRARY_DIR_V143_64}
                 $ENV{BOOST_LIBRARY_DIR_V143_32})
add_definitions(-DUNICODE -D_UNICODE)
add_executable(twainsave-opensource
    ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/acquire_characteristics.cpp
    ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/buffered_transfer_info.cpp
    ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/logger_callback.cpp
    ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/twain_characteristics.cpp
    ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/options_base.cpp
    ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/paperhandling_info.cpp
    ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/pdf_text_element.cpp
    ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/twain_callback.cpp
    ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/twain_session.cpp
    ${PROJECT_SOURCE_DIR}/cpp_wrapper_lib/twain_source.cpp
    ${PROJECT_SOURCE_DIR}/generate_details.cpp
    ${PROJECT_SOURCE_DIR}/stdafx.cpp
    ${PROJECT_SOURCE_DIR}/twainsave-opensource.cpp
)
set_property(TARGET twainsave-opensource PROPERTY CXX_STANDARD 17)
target_sources(twainsave-opensource PRIVATE ${PROJECT_SOURCE_DIR}/twainsave-opensource.rc)