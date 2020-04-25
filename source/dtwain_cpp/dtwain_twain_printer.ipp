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
enum class printer_font_style
{
   normal = TWPF_NORMAL,
   italic = TWPF_ITALIC,
   bold = TWPF_BOLD,
   smallsize = TWPF_SMALLSIZE,
   largesize = TWPF_LARGESIZE
};

enum class printer_index_trigger
{
   page = TWCT_PAGE,
   patch1 = TWCT_PATCH1,
   patch2 = TWCT_PATCH2,
   patch3 = TWCT_PATCH3,
   patch4 = TWCT_PATCH4,
   patcht = TWCT_PATCHT,
   patch6 = TWCT_PATCH6
};

enum class printer_string_mode
{
  single = TWPM_SINGLESTRING,
  multi = TWPM_MULTISTRING,
  compound = TWPM_COMPOUNDSTRING
};

class twain_printer_info
{

public:
    using cap_support_map = std::unordered_map<high_level_cap, bool>;

private:
    std::vector<std::string> m_printerStrings;
    std::string m_suffixString;
    uint32_t m_printerIndex;
    uint32_t m_printerMaxValue;
    uint32_t m_charRotation;
    uint32_t m_printerNumDigits;
    uint32_t m_printerIndexStep;
    std::vector<printer_font_style> m_fontStyles;
    std::string m_printerLeadChar;
    printer_index_trigger m_indexTrigger;
    printer_string_mode m_stringMode;
    twain_twfix32 m_vertical_offset;
    std::vector<std::string> m_string_preview;
    cap_support_map m_support_map;

public:
    twain_printer_info();
    bool attach(const dynarithmic::twain::twain_source& source);
    twain_printer_info& set_suffix_string(const std::string& s);
    twain_printer_info& set_index(uint32_t idx);
    twain_printer_info& set_index_max(uint32_t maxValue);
    twain_printer_info& set_index_step(uint32_t step);
    twain_printer_info& set_num_digits(uint32_t numDigits);
    twain_printer_info& set_lead_character(const std::string& s);
    twain_printer_info& set_string_mode(printer_string_mode mode);
    twain_printer_info& set_vertical_offset(double offset);
    twain_printer_info& set_vertical_offset(const twain_twfix32& offset);
    twain_printer_info& set_index_trigger(printer_index_trigger trigger);
    twain_printer_info& set_char_rotation(uint32_t rotation);

    template <typename Container>
    twain_printer_info& set_printer_strings(const Container& c)
    {
        m_printerStrings.clear();
        std::copy(std::begin(c), std::end(c), std::back_inserter(m_printerStrings));
        return *this;
    }

    template <typename Container>
    twain_printer_info& set_font_styles(const Container& c)
    {
        m_fontStyles.clear();
        std::copy(std::begin(c), std::end(c), std::back_inserter(m_fontStyles));
        return *this;
    }

    std::string get_suffix_string() const;
    uint32_t get_index() const;
    uint32_t get_index_max() const;
    uint32_t get_index_step() const;
    uint32_t get_num_digits() const;
    uint32_t get_char_rotation() const;
    const cap_support_map& get_support_map() const { return m_support_map; }

    std::string set_lead_character() const;
    printer_string_mode get_string_mode() const;
    twain_twfix32 get_vertical_offset() const;
    printer_index_trigger get_index_trigger() const;

    template <typename Container>
    void get_printer_strings(Container& c)
    {
        c.clear();
        std::copy(std::begin(m_printerStrings), std::end(m_printerStrings), std::inserter(c, c.end()));
    }

    template <typename Container>
    void get_printer_string_preview(Container& c)
    {
        c.clear();
        std::copy(std::begin(m_string_preview), std::end(m_string_preview), std::inserter(c, c.end()));
    }

    template <typename Container>
    void get_font_styles(Container& c)
    {
        c.clear();
        std::copy(std::begin(m_fontStyles), std::end(m_fontStyles), std::inserter(c, c.end()));
    }
};