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
        void buffered_transfer_info::attach(twain_source& ts)
        {
            capability_interface ci;
            ci.attach(ts.get_source());
            std::unordered_set<capability_interface::compression_type> vc;
            ci.get_compression(vc);
            std::transform(vc.begin(), vc.end(), std::inserter(all_compression_types, all_compression_types.end()),
                           [&](LONG value) { return static_cast<twain_compression_type>(value); });
            DTWAIN_GetAcquireStripSizes(ts.get_source(), &m_nMinSize, &m_nMaxSize, &m_nPrefSize);
            m_twain_source = ts.get_source();
        }

        bool buffered_transfer_info::init_transfer()
        {
            if (!m_twain_source)
                return false;
            // Check strip size here
            if (m_nStripSize > 0)
            {
                if (m_nStripSize < m_nMinSize || m_nStripSize > m_nMaxSize)
                    return false;
            }

            // Check if compression type is supported
            if (all_compression_types.find(m_compression_type) == all_compression_types.end())
                return false;

            if (m_nStripSize > 0)
            {
                // Allocate memory for strip here
                if (m_hStrip)
                    DTWAIN_FreeMemory(m_hStrip);

                m_hStrip = DTWAIN_AllocateMemory(m_nStripSize);
                if (!m_hStrip)
                    return false;

                if (DTWAIN_SetAcquireStripBuffer(m_twain_source, m_hStrip))
                {
                    DTWAIN_FreeMemory(m_hStrip);
                    return false;
                }
            }
            return true;
        }
    };
}
