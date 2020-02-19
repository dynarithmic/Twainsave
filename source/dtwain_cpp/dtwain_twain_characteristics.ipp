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
// TWAIN characteristics when a session has been started
using twain_app_info = twain_identity;

class twain_characteristics
{
        dsm_type dsmToUse;
        twain_app_info app_info;
        dsm_search_order search_order;
        logger_details log_details;
        std::string temporary_dir;
    
    public:
        twain_characteristics() : dsmToUse(dsm_type::legacy_dsm), search_order(dsm_search_order::search_wso)
        {}
        twain_characteristics& use_dsm(dsm_type dsm){ dsmToUse = dsm; return *this;}
        twain_characteristics& set_temporary_directory(const std::string& dir) { temporary_dir = dir; return *this; }
        twain_characteristics& set_app_info(const twain_app_info& info) { app_info = info; return *this; }
        twain_characteristics& set_logger_details(const logger_details& lo) { log_details = lo; return *this; }
        twain_characteristics& set_dsm_search_order(dsm_search_order so) { search_order = so; return *this; }
        twain_app_info& get_app_info_ref() { return app_info; }
        twain_app_info get_app_info() const { return app_info; }
        dsm_type get_used_dsm() const { return dsmToUse; }
        dsm_search_order get_dsm_search_order() const { return search_order; }
        logger_details& get_logger_details() { return log_details; }
        std::string get_temporary_directory() const { return temporary_dir; }
};
