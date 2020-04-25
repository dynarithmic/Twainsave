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
#include <algorithm>
#include <functional>
#include <random>

namespace dynarithmic
{
    namespace twain
    {
        std::string pdf_options::pdf_encryption_options::generate_random()
        {
            char s[100] = {};
            struct rnd_gen
            {
                rnd_gen() : range(nullptr), len(0)
                {}

                char operator()() const
                {
                    static const char range[] = "abcdefghijklmnopqrstuvwxyz0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ!@#$%^&()_+=-{}[];:'\\\",<.>/?`~";
                    static const size_t len = std::size(range);
                    static std::random_device rand_dev;
                    static std::mt19937 generator(rand_dev());
                    std::uniform_int_distribution<int>  distr(0, len - 1);
                    auto val = distr(generator);
                    return range[val];
                }
            private:
                const char *range;
                std::size_t len;
            };
            std::generate_n(s, 32, rnd_gen());
            return std::string(s, 32);
        }

		pdf_options::pdf_encryption_options& pdf_options::pdf_encryption_options::set_password(std::string& pwdtoset, const std::string& pwdString)
        {
            if (pwdString.empty() && m_bUseRandomUser)
                pwdtoset = generate_random();
            else
                pwdtoset = pwdString;
            return *this;
        }

		pdf_options::pdf_encryption_options::pdf_encryption_options() : m_useStrong(true), m_useEncryption(false), m_permissions(0), m_bUseRandomOwner(false), m_bUseRandomUser(false) {}
		pdf_options::pdf_encryption_options& pdf_options::pdf_encryption_options::set_use_encryption(bool bSet, bool useStrong) { m_useEncryption = bSet; m_useStrong = useStrong; return *this; }
		pdf_options::pdf_encryption_options& pdf_options::pdf_encryption_options::set_use_strong_encryption(bool bSet) { m_useEncryption = true; m_useStrong = bSet; return *this; }
		pdf_options::pdf_encryption_options& pdf_options::pdf_encryption_options::set_use_random_owner(bool bSet) { m_bUseRandomOwner = bSet; return *this; }
		pdf_options::pdf_encryption_options& pdf_options::pdf_encryption_options::set_use_random_user(bool bSet) { m_bUseRandomUser = bSet; return *this; }
		pdf_options::pdf_encryption_options& pdf_options::pdf_encryption_options::set_user_password(const std::string& pwd) { return set_password(m_user_password, pwd);}
		pdf_options::pdf_encryption_options& pdf_options::pdf_encryption_options::set_owner_password(const std::string& pwd) { return set_password(m_owner_password, pwd); }

        std::string pdf_options::pdf_encryption_options::get_user_password() const { return m_user_password; }
        std::string pdf_options::pdf_encryption_options::get_owner_password() const { return m_owner_password; }
        bool pdf_options::pdf_encryption_options::get_use_encryption() const { return m_useEncryption; }
        bool pdf_options::pdf_encryption_options::get_use_strong_encryption() const { return m_useStrong; }
        int32_t pdf_options::pdf_encryption_options::get_all_permissions() const { return m_permissions; }
        bool pdf_options::pdf_encryption_options::is_permission_set(pdf_permission perm) const { return (m_permissions & static_cast<int32_t>(perm)) ? true : false; }

        pdf_options::pdf_page_size_options::pdf_page_size_options() : m_pagesize(paper_size::USLETTER),
                                                         m_size_opt(pdf_paper_size_custom::none),
                                                         m_pagesize_custom({ pdf_page_size_options::default_size, pdf_page_size_options::default_size }) {}

		pdf_options::pdf_page_size_options& pdf_options::pdf_page_size_options::set_custom_size(uint32_t width, uint32_t height) { m_pagesize_custom = { width, height }; return *this; }
		pdf_options::pdf_page_size_options& pdf_options::pdf_page_size_options::set_page_size(paper_size ps) { m_pagesize = ps; return *this; }
		pdf_options::pdf_page_size_options& pdf_options::pdf_page_size_options::set_custom_option(pdf_paper_size_custom ps) { m_size_opt = ps; return *this; }
		pdf_options::pdf_page_size_options& pdf_options::pdf_page_size_options::reset_sizes() { *this = pdf_page_size_options(); return *this; }

        paper_size pdf_options::pdf_page_size_options::get_page_size() const { if (!is_custom_size_used()) return m_pagesize; return paper_size::CUSTOM; }
        pdf_options::pdf_paper_size_custom pdf_options::pdf_page_size_options::get_custom_option() const { return m_size_opt; }
		pdf_options::pdf_page_size_options::custom_size_type pdf_options::pdf_page_size_options::get_custom_size() const { return m_pagesize_custom; }
        bool pdf_options::pdf_page_size_options::is_custom_size_used() const { return m_pagesize_custom.first != default_size && m_pagesize_custom.second != default_size; }

		pdf_options::pdf_page_scale_options::pdf_page_scale_options() : m_pagescale(pdf_page_scale::none),
                                                            m_pagescale_custom({ default_scale, default_scale }) {}

		pdf_options::pdf_page_scale_options& pdf_options::pdf_page_scale_options::set_custom_scale(double xScale, double yScale) { m_pagescale_custom = { xScale, yScale }; return *this; }
		pdf_options::pdf_page_scale_options& pdf_options::pdf_page_scale_options::set_page_scale(pdf_page_scale ps) { m_pagescale = ps; return *this; }
		pdf_options::pdf_page_scale_options& pdf_options::pdf_page_scale_options::reset_scaling() { *this = pdf_page_scale_options(); return *this; }

		pdf_options::pdf_page_scale pdf_options::pdf_page_scale_options::get_page_scale() const { return m_pagescale; }
		pdf_options::pdf_page_scale_options::custom_scale_type pdf_options::pdf_page_scale_options::get_custom_scale() const { return m_pagescale_custom; }
        bool pdf_options::pdf_page_scale_options::is_custom_scale_used() const { return m_pagescale_custom.first != default_scale && m_pagescale_custom.second != default_scale; }

        pdf_options::pdf_options() : m_useASCII(false), m_jpegQuality(70), m_orientation(pdf_orientation::portrait) {}
        pdf_options& pdf_options::set_author(const std::string& value) { m_author = value; return *this; }
        pdf_options& pdf_options::set_creator(const std::string& value) { m_creator = value; return *this; }
        pdf_options& pdf_options::set_producer(const std::string& value) { m_producer = value; return *this; }
        pdf_options& pdf_options::set_keywords(const std::string& value) { m_keywords = value; return *this; }
        pdf_options& pdf_options::set_subject(const std::string& value) { m_subject = value; return *this; }
        pdf_options& pdf_options::set_title(const std::string& value) { m_title = value; return *this; }
        pdf_options& pdf_options::set_use_ASCII(bool bSet) { m_useASCII = bSet; return *this; }
        pdf_options& pdf_options::set_jpeg_quality(int quality) { m_jpegQuality = quality; return *this; }
        pdf_options& pdf_options::set_orientation(pdf_orientation orient) { m_orientation = orient; return *this; }

		pdf_options::pdf_encryption_options& pdf_options::get_encryption_options() { return m_encryptOptions; }
		pdf_options::pdf_page_size_options& pdf_options::get_page_size_options() { return m_size_options; }
		pdf_options::pdf_page_scale_options& pdf_options::get_page_scale_options() { return m_scale_options; }
        std::string pdf_options::get_author() const { return m_author; }
        std::string pdf_options::get_creator() const { return m_creator; }
        std::string pdf_options::get_producer() const { return m_producer; }
        std::string pdf_options::get_keywords() const { return m_keywords; }
        std::string pdf_options::get_subject() const { return m_subject; }
        std::string pdf_options::get_title() const { return m_title; }
        bool pdf_options::is_use_ASCII() const { return m_useASCII; }
        int pdf_options::get_jpeg_quality() const { return m_jpegQuality; }
		pdf_options::pdf_orientation pdf_options::get_orientation() const { return m_orientation; }
    }
}