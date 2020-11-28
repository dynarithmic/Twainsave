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
#ifndef DTWAIN_TWAIN_SESSION_BASE_HPP
#define DTWAIN_TWAIN_SESSION_BASE_HPP

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

#include <dynarithmic/twain/characteristics/twain_characteristics.hpp>
#include <dynarithmic/twain/identity/twain_identity.hpp>

namespace dynarithmic {
namespace twain {
    class twain_source;
    class twain_logger;
    class twain_listener;
    class error_logger;

    /**
    The twain_session_base class serves as the base class to twain_session.
    */
    class twain_session_base
    {
        public:
            using source_basic_info = twain_app_info;
            using logger_callback_type = std::pair<twain_session_base*, std::unique_ptr<twain_logger>>;
            using callback_map_type = std::unordered_map<twain_source*, std::unique_ptr<twain_listener>>;

        protected:
            bool m_bStarted = false;
            twain_characteristics m_twain_characteristics;
            std::string m_dsm_path;
            std::string m_long_name;
            std::string m_short_name;
            std::string m_dtwain_path;
            DTWAIN_HANDLE m_Handle = nullptr;
            logger_callback_type m_logger_callback;
            callback_map_type m_mapcallback;
            mutable std::vector<source_basic_info> m_source_cache;

        public:
            twain_session_base() = default;
            virtual ~twain_session_base() {}
            logger_callback_type& get_logger_callback() noexcept { return m_logger_callback; }

            /// Reference to the characteristics of the TWAIN session
            /// 
            /// @returns Reference to the characteristics of the TWAIN session
            twain_characteristics& get_twain_characteristics() noexcept { return m_twain_characteristics; }

            callback_map_type& get_callback_map() noexcept { return m_mapcallback; }
            virtual void log_error(LONG msg) {}
    };
}
}
#endif
