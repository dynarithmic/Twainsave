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
#ifndef DTWAIN_BUFFERED_TRANSFER_INFO_HPP
#define DTWAIN_BUFFERED_TRANSFER_INFO_HPP

#include <unordered_set>
#include <dynarithmic/twain/twain_values.hpp>
#include <dynarithmic/twain/source/twain_source_base.hpp>

namespace dynarithmic
{
    namespace twain
    {
        struct acquired_strip_data
        {
            LONG Compression;
            LONG BytesPerRow;
            LONG Columns;
            LONG Rows;
            LONG XOffset;
            LONG YOffset;
            LONG BytesWritten;
            acquired_strip_data() : Compression(0), BytesPerRow(0), Columns(0), 
                                    Rows(0), XOffset(0), YOffset(0), BytesWritten() {}
        };

        class buffered_transfer_info
        {
            private:
                acquired_strip_data m_stripData;
                HANDLE m_hStrip;
                LONG m_nStripSize;
                LONG m_nCurrentStripSize;
                LONG m_nMinSize, m_nMaxSize, m_nPrefSize;
                std::unordered_set<compression_value::value_type> all_compression_types;
                DTWAIN_SOURCE m_twain_source;
            
            public:
                buffered_transfer_info() : m_hStrip(nullptr), m_nStripSize(0),
                                            m_nMinSize(0), m_nMaxSize(0), m_nPrefSize(0), 
                                            m_twain_source(nullptr), m_nCurrentStripSize(0)
                {}

                buffered_transfer_info::~buffered_transfer_info()
                {
                    if ( m_hStrip )
                        API_INSTANCE DTWAIN_FreeMemory(m_hStrip);
                }

                LONG stripsize() const { return m_nStripSize; }
                LONG minstripsize() const { return m_nMinSize; }
                LONG maxstripsize() const { return m_nMaxSize; }
                LONG preferredsize() const { return m_nPrefSize; }
                HANDLE getstrip() const { return m_hStrip; }

                acquired_strip_data& buffered_transfer_info::get_strip_data()
                {
                    API_INSTANCE DTWAIN_GetAcquireStripData(m_twain_source, &m_stripData.Compression, &m_stripData.BytesPerRow,
                        &m_stripData.Columns, &m_stripData.Rows, &m_stripData.XOffset,
                        &m_stripData.YOffset, &m_stripData.BytesWritten);
                    return m_stripData;
                }

                buffered_transfer_info& set_stripsize(long sz) { m_nStripSize = sz; return *this; }
            
                bool init_transfer(compression_value::value_type compression)
                {
                    if (!m_twain_source)
                        return false;
                    // Check strip size here
                    if (m_nStripSize > 0)
                    {
                        if (m_nStripSize < m_nMinSize || m_nStripSize > m_nMaxSize)
                            return false;
                    }

                    if (all_compression_types.find(compression) == all_compression_types.end())
                        return false;

                    if (m_nStripSize > 0)
                    {
                        // Allocate memory for strip here
                        if (m_hStrip)
                            API_INSTANCE DTWAIN_FreeMemory(m_hStrip);

                        m_hStrip = API_INSTANCE DTWAIN_AllocateMemory(m_nStripSize);
                        if (!m_hStrip)
                            return false;

                        if (!API_INSTANCE DTWAIN_SetAcquireStripBuffer(m_twain_source, m_hStrip))
                        {
                            API_INSTANCE DTWAIN_FreeMemory(m_hStrip);
                            return false;
                        }
                    }
                    return true;
                }

                void buffered_transfer_info::attach(twain_source_base& ts)
                {
                    capability_interface& ci = ts.get_capability_interface();
                    auto vc = ci.get_cap_values<std::unordered_set<ICAP_COMPRESSION_::value_type>>(ICAP_COMPRESSION);
                    std::copy(vc.begin(), vc.end(), std::inserter(all_compression_types, all_compression_types.end()));
                    API_INSTANCE DTWAIN_GetAcquireStripSizes(ts.get_source(), &m_nMinSize, &m_nMaxSize, &m_nPrefSize);
                    m_nStripSize = m_nPrefSize;
                    m_twain_source = ts.get_source();
                }
        };
    }
}
#endif
