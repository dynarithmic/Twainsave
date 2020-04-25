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
        capability_interface::capability_interface(twain_source* s) : m_Source(s)
        {
            fill_caps();
        }

        capability_interface::capability_interface(twain_source& s) : m_Source(&s)
        {
            fill_caps();
        }

        capability_interface::capability_interface() : m_Source(nullptr)
        {
        }

        void capability_interface::initialize_cached_set()
        {
            static const int excluded_set[] = {CAP_DEVICEONLINE, CAP_DUPLEXENABLED, CAP_ENABLEDSUIONLY, ICAP_BITDEPTH, ICAP_FRAMES};
            for (int i = 0; i < sizeof(excluded_set) / sizeof(excluded_set[0]); ++i)
                m_cacheable_set.erase(excluded_set[i]);
        }

        bool capability_interface::fill_caps()
        {
            char szBuffer[256];
            m_caps.clear();
            m_cacheable_set.clear();
            std::vector<CAP_SUPPORTEDCAPS_::value_type> vCaps;
            get_supportedcaps(vCaps);
            std::for_each(vCaps.begin(), vCaps.end(), [&](const CAP_SUPPORTEDCAPS_::value_type capVal)
            {
                DTWAIN_GetNameFromCapA(capVal, szBuffer, 255);
                m_caps[capVal] = szBuffer;
                m_cacheable_set.insert(capVal);
            });
            initialize_cached_set();
            return !vCaps.empty();
        }

        const capability_interface::source_cap_info& capability_interface::get_source_cap_info() const
        {
            return m_caps;
        }

        void capability_interface::remove_from_cache(const std::vector<int>& removed)
        {
            std::for_each(removed.begin(), removed.end(), [&](int n)
            {
                m_cacheable_set.erase(n);
                m_cap_cache.erase(n);
            });
        }

        void capability_interface::add_to_cache(const std::vector<int>& added)
        {
            std::copy(added.begin(), added.end(), std::inserter(m_cacheable_set, m_cacheable_set.begin()));
        }

        void capability_interface::clear_cache()
        {
            m_cap_cache.clear();
        }

        capability_interface::source_cap_info::iterator capability_interface::begin() { return m_caps.begin(); }
        capability_interface::source_cap_info::iterator capability_interface::end() { return m_caps.end(); }
        capability_interface::source_cap_info::const_iterator capability_interface::cbegin() { return m_caps.cbegin(); }
        capability_interface::source_cap_info::const_iterator capability_interface::cend() { return m_caps.cend(); }

        bool capability_interface::attach(DTWAIN_SOURCE s)
        {
            m_Source = s;
            return fill_caps();
        }

        void capability_interface::detach()
        {
            m_Source = nullptr;
            m_cap_cache.clear();
            m_cacheable_set.clear();
        }

    }
}
