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
        twain_array::twain_array() : m_theArray(nullptr)
        {
        }

        twain_array::twain_array(DTWAIN_ARRAY a) : m_theArray(a)
        {
            LONG nStatus = 0;
            m_isRange = DTWAIN_RangeIsValid(a, &nStatus) ? true : false;
        }

        twain_array::twain_array(const twain_array& rhs)
        {
            if (rhs.m_theArray)
                m_theArray = DTWAIN_ArrayCreateCopy(rhs.m_theArray);
            m_isRange = rhs.m_isRange;
        }

        twain_array::twain_array(twain_array&& rhs) noexcept
        {
            m_theArray = rhs.m_theArray;
            m_isRange = rhs.m_isRange;
            rhs.m_theArray = nullptr;
        }

        twain_array& twain_array::operator=(twain_array&& rhs) noexcept
        {
            swap(*this, rhs);
            rhs.m_theArray = nullptr;
            return *this;
        }

        twain_array& twain_array::operator=(const twain_array& rhs)
        {
            if (&rhs != this)
            {
                auto temp(rhs);
                swap(temp, *this);
            }
            return *this;
        }

        void twain_array::swap(twain_array& t1, twain_array& t2) const
        {
            std::swap(t1.m_theArray, t2.m_theArray);
            std::swap(t1.m_isRange, t2.m_isRange);
        }

        void twain_array::set_array(DTWAIN_ARRAY a) { m_theArray = a; m_isRange = DTWAIN_RangeIsValid(a, nullptr)?true:false; }
        void twain_array::resize(size_t n) const { if (m_theArray) DTWAIN_ArrayResize(m_theArray, n); }
        bool twain_array::is_range() const { return m_isRange; }

        DTWAIN_ARRAY twain_array::get_array() const { return m_theArray; }
        LPDTWAIN_ARRAY twain_array::get_array_ptr() { return &m_theArray; }
        twain_array::~twain_array() { if (m_theArray) DTWAIN_ArrayDestroy(m_theArray); }

        long twain_array::get_count() const
        {
            if (m_theArray) return DTWAIN_ArrayGetCount(m_theArray);
            return -1L;
        }

        bool twain_array::expand_range_replace()
        {
            if (m_isRange)
            {
                DTWAIN_ARRAY temp;
                if (DTWAIN_RangeExpand(m_theArray, &temp))
                {
                    const twain_array temp_arr(temp);
                    *this = temp_arr;
                    return true;
                }
            }
            return false;
        }

        long twain_array::get_expanded_count() const
        {
            if (m_isRange)
                return DTWAIN_RangeGetCount(m_theArray);
            return 0;
        }
    };
}
