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
// TWAIN application info (used by both the TWAIN Data Source Manager and Data Source
class twain_identity
{
private:
    TW_IDENTITY m_identity;

public:
    explicit twain_identity(TW_IDENTITY& t);
    explicit twain_identity(TW_IDENTITY* t);
    twain_identity();
    twain_identity& operator =(TW_IDENTITY&) noexcept;
    twain_identity& operator =(TW_IDENTITY*) noexcept;
    twain_identity& set_protocol_major(uint16_t val);
    twain_identity& set_protocol_minor(uint16_t val);
    twain_identity& set_supported_groups(uint32_t val);
    twain_identity& set_manufacturer(const std::string& s);
    twain_identity& set_product_family(const std::string& s);
    twain_identity& set_product_name(const std::string& s);
    twain_identity& set_major_num(uint16_t val);
    twain_identity& set_minor_num(uint16_t val);
    twain_identity& set_language(uint16_t val);
    twain_identity& set_country(uint16_t val);
    twain_identity& set_version_info(const std::string& s);

    uint16_t get_protocol_major() const;
    uint16_t get_protocol_minor() const;
    uint32_t get_supported_groups() const;
    std::string get_manufacturer() const;
    std::string get_product_family() const;
    std::string get_product_name() const;
    uint16_t get_major_num() const;
    uint16_t get_minor_num() const;
    uint16_t get_language() const;
    uint16_t get_country() const;
    std::string get_version_info() const;

    TW_IDENTITY& get_identity() { return m_identity; };
    explicit operator TW_IDENTITY*()
    {
        TW_IDENTITY test = {};
        if (std::memcmp(&m_identity, &test, sizeof(TW_IDENTITY)) == 0) 
            return nullptr; 
        return &m_identity;
    }
};

using twain_source_info = twain_identity;
