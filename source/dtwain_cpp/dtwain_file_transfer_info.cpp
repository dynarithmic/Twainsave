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
#include "dtwain_cpp_interface.hpp"

namespace dynarithmic
{
    namespace twain
    {
        void file_transfer_info::attach(twain_source& ts)
        {
            capability_interface ci;
            ci.attach(ts.get_source());
            std::unordered_set<capability_interface::imagefileformat_type> vFormat;
            ci.get_imagefileformat(vFormat);
            std::transform(vFormat.begin(), vFormat.end(), std::inserter(all_file_types, all_file_types.end()),
                           [&](LONG value) { return static_cast<file_type>(value); });
            m_twain_source = ts.get_source();
        }
    };
}
