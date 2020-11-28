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

#ifndef PDF_TEXT_ELEMENT_HPP
#define PDF_TEXT_ELEMENT_HPP

#include <string>
#include <utility>
#include <cstdlib>
#include <vector>
#include <numeric>
#include <algorithm>
#include <dynarithmic/twain/source/twain_source_base.hpp>

namespace dynarithmic
{
    namespace twain
    {
        struct pdf_rendermode_value
        {
            static constexpr LONG fill = DTWAIN_PDFRENDER_FILL;
            static constexpr LONG stroke = DTWAIN_PDFRENDER_STROKE;
            static constexpr LONG fillstroke = DTWAIN_PDFRENDER_FILLSTROKE;
            static constexpr LONG invisible = DTWAIN_PDFRENDER_INVISIBLE;
        };

        struct pdf_printpage_value
        {
            static constexpr LONG allpages = DTWAIN_PDFTEXT_ALLPAGES;
            static constexpr LONG evenpages = DTWAIN_PDFTEXT_EVENPAGES;
            static constexpr LONG oddpages = DTWAIN_PDFTEXT_ODDPAGES;
            static constexpr LONG firstpage = DTWAIN_PDFTEXT_FIRSTPAGE;
            static constexpr LONG lastpage = DTWAIN_PDFTEXT_LASTPAGE;
            static constexpr LONG currentpage = DTWAIN_PDFTEXT_CURRENTPAGE;
        };

        enum class pdf_pageignore_flag : uint32_t
        {
            ignorenone  = 0,
            scaling     = DTWAIN_PDFTEXT_NOSCALING,
            charspacing = DTWAIN_PDFTEXT_NOCHARSPACING,
            wordspacing = DTWAIN_PDFTEXT_NOWORDSPACING,
            rendermode  = DTWAIN_PDFTEXT_NORENDERMODE,
            rgbcolor    = DTWAIN_PDFTEXT_NORGBCOLOR,
            fontsize    = DTWAIN_PDFTEXT_NOFONTSIZE,
            ignoreall   = DTWAIN_PDFTEXT_IGNOREALL
        };

        class pdf_text_element
        {
            std::string m_text;
            std::pair<uint32_t, uint32_t> m_position;
            std::string m_font;
            int m_fontsize;
            uint32_t m_color;
            LONG m_rendermode;
            double m_scaling;
            double m_charspacing;
            double m_wordspacing;
            uint32_t m_strokewidth;
            LONG m_printpage;
            std::vector<pdf_pageignore_flag> m_vIgnoreflags;

            public:
                typedef std::vector<pdf_pageignore_flag> pdf_ignoreflag_container;

                pdf_text_element() : m_font("Helvetica"), 
                                     m_fontsize(10), 
                                     m_color(RGB(0, 0, 0)), 
                                     m_rendermode(pdf_rendermode_value::fill),
                                     m_scaling(100.0),
                                     m_charspacing(0),
                                     m_wordspacing(0),
                                     m_strokewidth(0),
                                     m_printpage(pdf_printpage_value::currentpage),
                                     m_vIgnoreflags{pdf_pageignore_flag::ignorenone},
                                     m_position{} {}

                pdf_text_element& set_text(const std::string& s) { m_text = s; return *this; }
                pdf_text_element& set_position(uint32_t x, uint32_t y) { m_position = { x,y }; return *this; }
                pdf_text_element& set_font(const std::string& s) { m_font = s; return *this; }
                pdf_text_element& set_color(uint32_t color) { m_color = color; return *this; }
                pdf_text_element& set_fontsize(int sz) { m_fontsize = sz; return *this; }
                pdf_text_element& set_rendermode(LONG rm) { m_rendermode = rm; return *this; }
                pdf_text_element& set_scaling(double val) { m_scaling = val; return *this; }
                pdf_text_element& set_charspacing(double val) { m_charspacing = val; return *this; }
                pdf_text_element& set_wordspacing(double val) { m_wordspacing = val; return *this; }
                pdf_text_element& set_strokewidth(uint32_t val) { m_strokewidth = val; return *this; }
                pdf_text_element& set_whichpages(LONG whichpage) { m_printpage = whichpage; return *this; }
                pdf_text_element& set_ignoreflags(const pdf_ignoreflag_container& vectFlags) { m_vIgnoreflags = vectFlags; return *this; }

                std::string get_text() const { return m_text; }
                std::pair<uint32_t, uint32_t> get_position() const { return m_position; } 
                std::string get_font() const  { return m_font; }
                uint32_t get_color() const { return m_color; }
                int get_fontsize() const { return m_fontsize; }
                LONG get_rendermode() const { return m_rendermode; }
                double get_scaling() const { return m_scaling; }
                double get_charspacing() const { return m_charspacing; }
                double get_wordspacing() const { return m_wordspacing; }
                uint32_t get_strokewidth() const { return m_strokewidth; }
                LONG get_whichpages() const { return m_printpage; }
                pdf_ignoreflag_container get_ignoreflags() const { return m_vIgnoreflags; }
                pdf_ignoreflag_container& get_ignoreflags_ref() { return m_vIgnoreflags; }

                bool add_text(twain_source_base& ts)
                {
                    DTWAIN_SOURCE src = ts.get_source();
                    LONG flags = 0;

                    if (std::find(m_vIgnoreflags.begin(), m_vIgnoreflags.end(), pdf_pageignore_flag::ignorenone) == m_vIgnoreflags.end())
                    {
                        for (auto fv : m_vIgnoreflags)
                            flags |= static_cast<LONG>(fv);
                    }
                    flags |= m_printpage;

                    BOOL retval = DTWAIN_AddPDFTextA(src,
                                                    m_text.c_str(),
                                                    m_position.first,
                                                    m_position.second,
                                                    m_font.c_str(),
                                                    m_fontsize,
                                                    m_color,
                                                    m_rendermode,
                                                    m_scaling,
                                                    m_charspacing,
                                                    m_wordspacing,
                                                    m_strokewidth,
                                                    flags);
                    return retval ? true : false;
                }
        };
    }
}
#endif
