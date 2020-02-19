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
class buffered_transfer_info
{
    private:
        HANDLE m_hStrip;
        LONG m_nStripSize;
        LONG m_nMinSize, m_nMaxSize, m_nPrefSize;
        twain_compression_type m_compression_type;
        std::unordered_set<twain_compression_type> all_compression_types;
        twain_source* m_twain_source;
    
    public:
        buffered_transfer_info() : m_hStrip(nullptr), m_nStripSize(0),
                                    m_nMinSize(0), m_nMaxSize(0), m_nPrefSize(0),
                                    m_compression_type(twain_compression_type::none),
                                    m_twain_source(nullptr)
        {
        }
    
        LONG stripsize() const { return m_nStripSize; }
        LONG minstripsize() const { return m_nMinSize; }
        LONG maxstripsize() const { return m_nMaxSize; }
        LONG preferredsize() const { return m_nPrefSize; }
        HANDLE getstrip() const { return m_hStrip; }
    
        void attach(twain_source& ts);
    
        buffered_transfer_info& set_stripsize(long sz)
        { m_nStripSize = sz; return *this; }
    
        buffered_transfer_info& set_compression_type(twain_compression_type ct)
        { m_compression_type = ct; return *this; }
    
        bool init_transfer();
};
