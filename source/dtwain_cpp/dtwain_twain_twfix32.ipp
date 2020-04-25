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
class twain_twfix32
{
    TW_FIX32 m_fix32;
public:
    twain_twfix32(const TW_FIX32& f32) : m_fix32(f32) {}
    twain_twfix32() : m_fix32{} {}
    twain_twfix32(int16_t Whole, uint32_t Frac) 
    {
        m_fix32.Whole = Whole;
        m_fix32.Frac = Frac;
    }

    twain_twfix32(double val)
    {
        bool sign = (val < 0);
        int32_t value = (int32_t)(val * 65536.0 + (sign ? (-0.5) : 0.5));
        m_fix32.Whole = static_cast<int16_t>(value >> 16);
        m_fix32.Frac = static_cast<uint16_t>(value & 0x0000ffffL);
    }

    double to_double() const
    {
        return static_cast<double>(m_fix32.Whole) + static_cast<double>(m_fix32.Frac) / 65536.0;
    }

    TW_FIX32 to_TWFIX32() const { return m_fix32; }

    twain_twfix32& operator +=(const twain_twfix32& rhs)
    {
        *this = twain_twfix32(to_double() + rhs.to_double());
        return *this;
    }

    twain_twfix32 operator +(const twain_twfix32& rhs)
    {
        return twain_twfix32(*this) += rhs;
    }
};


