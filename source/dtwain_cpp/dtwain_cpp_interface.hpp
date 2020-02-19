/*
This file is part of the Dynarithmic TWAIN Library (DTWAIN).
Copyright (c) 2002-2020 Dynarithmic Software.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

FOR ANY PART OF THE COVERED WORK IN WHICH THE COPYRIGHT IS OWNED BY
DYNARITHMIC SOFTWARE. DYNARITHMIC SOFTWARE DISCLAIMS THE WARRANTY OF NON INFRINGEMENT
OF THIRD PARTY RIGHTS.
*/
#ifndef DTWAIN_CPP_INTERFACE_HPP
#define DTWAIN_CPP_INTERFACE_HPP

#include <algorithm>
#include <utility>
#include <vector>
#include <set>
#include <iterator>
#include <memory>
#include <map>
#include <iostream>
#include <array>
#include <unordered_map>
#include <unordered_set>
#include <climits>
#include <string>
#include <cstring>
#include <bitset>
#include <cmath>
#include <functional>
#include <chrono>
#include <boost/concept_check.hpp>
#include <boost/variant.hpp>
#include <boost/logic/tribool.hpp>
#include <dtwain.h>

#undef min
#pragma warning (disable:4996)
namespace dynarithmic
{
    namespace twain
    {
        class acquire_characteristics;
        class buffered_transfer_info;
        class capability_interface;
        class filename_increment_rules;
        class image_handler;
        class logger_details;
        struct image_information;
        class pdf_options;
        class twain_array;
        class twain_characteristics;
        class twain_identity;
        class twain_listener;
        class twain_logger;
        class twain_printer_info;
        class twain_session;
        class twain_source;
        class twain_twfix32;
        class capability_listener;

        #include "dtwain_twtimer.ipp"
        #include "dtwain_twframe.ipp"
        #include "dtwain_twidentity.ipp"
        #include "dtwain_dsm.ipp"
        #include "dtwain_logger.ipp"
        #include "dtwain_twain_characteristics.ipp"
        #include "dtwain_filetype.ipp"
        #include "dtwain_twimage.ipp"
        #include "dtwain_twbasic.ipp"
        #include "dtwain_pdf_options.ipp"
        #include "dtwain_acquire_characteristics.ipp"
        #include "dtwain_twfix32.ipp"
        #include "dtwain_twrange.ipp"
        #include "dtwain_twarray.ipp"
        #include "dtwain_twprinter.ipp"
        #include "dtwain_capbasics1.ipp"
        #include "dtwain_buffered_transfer.ipp"
        #include "dtwain_capinterface.ipp"
        #include "dtwain_twsource.ipp"
        #include "dtwain_twlistener.ipp"
        #include "dtwain_twlogger.ipp"
        #include "dtwain_twsession.ipp"
        #include "dtwain_twsource_impl.ipp"
    };
}
#endif
