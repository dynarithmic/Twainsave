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
// Data Source Managers that can exist (old 1,x, new 2.x, and whatever is installed (auto detect)
enum class dsm_type
{
    legacy_dsm = DTWAIN_TWAINDSM_LEGACY,
    version2_dsm = DTWAIN_TWAINDSM_VERSION2,
    default_dsm = DTWAIN_TWAINDSM_LATESTVERSION
};

// Customize how to search for the Data source manager
enum class dsm_search_order
{
    search_wso = DTWAIN_TWAINDSMSEARCH_WSO,
    search_wos = DTWAIN_TWAINDSMSEARCH_WOS,
    search_swo = DTWAIN_TWAINDSMSEARCH_SWO,
    search_sow = DTWAIN_TWAINDSMSEARCH_SOW,
    search_ows = DTWAIN_TWAINDSMSEARCH_OWS,
    search_osw = DTWAIN_TWAINDSMSEARCH_OSW,
    search_w = DTWAIN_TWAINDSMSEARCH_W,
    search_s = DTWAIN_TWAINDSMSEARCH_S,
    search_o = DTWAIN_TWAINDSMSEARCH_O,
    search_ws = DTWAIN_TWAINDSMSEARCH_WS,
    search_wo = DTWAIN_TWAINDSMSEARCH_WO,
    search_sw = DTWAIN_TWAINDSMSEARCH_SW,
    search_so = DTWAIN_TWAINDSMSEARCH_SO,
    search_ow = DTWAIN_TWAINDSMSEARCH_OW,
    search_os = DTWAIN_TWAINDSMSEARCH_OS
};
