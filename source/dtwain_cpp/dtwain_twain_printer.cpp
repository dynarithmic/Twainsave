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
        twain_printer_info::twain_printer_info() : m_printerIndex(0), m_printerMaxValue(0), m_charRotation(0), m_printerNumDigits(0),
            m_printerIndexStep(0), m_indexTrigger(printer_index_trigger::page), m_stringMode(printer_string_mode::single),
            m_vertical_offset(0,0) {}

        bool twain_printer_info::attach(const twain_source& source)
        {
            static high_level_cap printer_caps[] =
            {
                high_level_cap::printer,
                high_level_cap::printercharrotation,
                high_level_cap::printerindex,
                high_level_cap::printerindexleadchar,
                high_level_cap::printerindexmaxvalue,
                high_level_cap::printerindexnumdigits,
                high_level_cap::printerindexstep,
                high_level_cap::printerindextrigger,
                high_level_cap::printermode,
                high_level_cap::printerstring,
                high_level_cap::printerstringpreview,
                high_level_cap::printersuffix,
                high_level_cap::printerverticaloffset
            };
        
            if (source.is_open())
            {
                m_support_map.clear();
                std::for_each(std::begin(printer_caps), std::end(printer_caps), [&](high_level_cap hcap)
                { 
                    auto info = source.get_cap_status(hcap);
                    m_support_map.insert({ hcap, info.is_supported() });
                });
                return true;
            }
            return false;
        }

        twain_printer_info& twain_printer_info::set_suffix_string(const std::string& s)
        { m_suffixString = s; return *this; }

        twain_printer_info& twain_printer_info::set_index(uint32_t idx)
        { m_printerIndex = idx; return *this; }

        twain_printer_info& twain_printer_info::set_index_max(uint32_t maxValue)
        { m_printerMaxValue = maxValue; return *this; }

        twain_printer_info& twain_printer_info::set_num_digits(uint32_t numDigits)
        { m_printerNumDigits = numDigits; return *this; }

        twain_printer_info& twain_printer_info::set_lead_character(const std::string& s)
        { m_printerLeadChar = s; return *this; }
            
        twain_printer_info& twain_printer_info::set_string_mode(printer_string_mode mode)
        { m_stringMode = mode; return *this; }

        twain_printer_info& twain_printer_info::set_vertical_offset(double offset)
        { m_vertical_offset = offset; return *this; }

        twain_printer_info& twain_printer_info::set_vertical_offset(const twain_twfix32& offset)
        { m_vertical_offset = offset; return *this; }

        twain_printer_info& twain_printer_info::set_index_trigger(printer_index_trigger trigger)
        { m_indexTrigger = trigger; return *this; }

        twain_printer_info& twain_printer_info::set_index_step(uint32_t step)
        { m_printerIndexStep = step; return *this; }

        twain_printer_info& twain_printer_info::set_char_rotation(uint32_t rotation)
        { m_charRotation = rotation; return *this; }

        std::string twain_printer_info::get_suffix_string() const { return m_suffixString; }
        uint32_t twain_printer_info::get_index() const { return m_printerIndex; }
        uint32_t twain_printer_info::get_index_max() const { return m_printerMaxValue; }
        uint32_t twain_printer_info::get_index_step() const { return m_printerIndexStep; }
        uint32_t twain_printer_info::get_num_digits() const { return m_printerNumDigits; }
        uint32_t twain_printer_info::get_char_rotation() const { return m_charRotation; }
        std::string twain_printer_info::set_lead_character() const { return m_printerLeadChar; }
        printer_string_mode twain_printer_info::get_string_mode() const { return m_stringMode; }
        twain_twfix32 twain_printer_info::get_vertical_offset() const { return m_vertical_offset; }
        printer_index_trigger twain_printer_info::get_index_trigger() const { return m_indexTrigger; }
    };
}
