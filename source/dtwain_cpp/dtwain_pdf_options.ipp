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
using pdf_page_size = paper_size;

enum class pdf_orientation
{
   landscape = DTWAIN_PDF_LANDSCAPE,
   portrait = DTWAIN_PDF_PORTRAIT
};

enum class pdf_page_scale
{
  none = DTWAIN_PDF_NOSCALING,
  fitpage = DTWAIN_PDF_FITPAGE,
  custom = DTWAIN_PDF_CUSTOMSCALE

};

enum class pdf_permission
{
   print = DTWAIN_PDF_ALLOWPRINTING,
   modify = DTWAIN_PDF_ALLOWMOD,
   copy = DTWAIN_PDF_ALLOWCOPY,
   modifyannotations = DTWAIN_PDF_ALLOWMODANNOTATIONS,
   fillin = DTWAIN_PDF_ALLOWFILLIN,
   extract = DTWAIN_PDF_ALLOWEXTRACTION,
   assembly = DTWAIN_PDF_ALLOWASSEMBLY,
   degradedprint = DTWAIN_PDF_ALLOWDEGRADEDPRINTING,
   all = print | modify | copy | modifyannotations | fillin | extract | assembly | degradedprint
};

enum class pdf_paper_size_custom
{
    none = 0,
    custom = DTWAIN_PDF_CUSTOMSIZE,
    pixelspermeter = DTWAIN_PDF_PIXELSPERMETERSIZE,
    variable = DTWAIN_PDF_VARIABLEPAGESIZE
};

class pdf_encryption_options
{
    std::string m_user_password;
    std::string m_owner_password;
    bool m_useStrong;
    bool m_useEncryption;
    bool m_bUseRandomUser;
    bool m_bUseRandomOwner;
    int32_t m_permissions;
    
    pdf_encryption_options& set_password(std::string& pwdtoset, const std::string& pwdString);  
    std::string generate_random();

    public:
       pdf_encryption_options();
       pdf_encryption_options& set_use_encryption(bool bSet, bool useStrong = true);
       pdf_encryption_options& set_use_strong_encryption(bool bSet = true);
       pdf_encryption_options& set_use_random_owner(bool bSet = true);
       pdf_encryption_options& set_use_random_user(bool bSet = true);
       pdf_encryption_options& set_user_password(const std::string& pwd = ""); 
       pdf_encryption_options& set_owner_password(const std::string& pwd = "");
       
       template <typename Container>
           pdf_encryption_options& set_permissions(const Container& ctr)
       {       
           auto iter = ctr.begin();
           while (iter != ctr.end())
           {
                      m_permissions |= static_cast<LONG>(*iter);
                      std::advance(iter, 1);
           }
           return *this;
       }
       
       std::string get_user_password() const;
       std::string get_owner_password() const;
       bool get_use_encryption() const;
       bool get_use_strong_encryption() const;
       int32_t get_all_permissions() const ;
       bool is_permission_set(pdf_permission perm) const;
};

#undef max
class pdf_page_size_options
{
    public:
        using custom_size_type = std::pair<uint32_t, uint32_t>;
    private:
        paper_size m_pagesize;
        custom_size_type m_pagesize_custom;
        pdf_paper_size_custom m_size_opt;
    public:
        static constexpr int default_size = std::numeric_limits<uint32_t>::max();
        pdf_page_size_options();        
        pdf_page_size_options& set_custom_size(uint32_t width, uint32_t height);
        pdf_page_size_options& set_page_size(paper_size ps);
        pdf_page_size_options& set_custom_option(pdf_paper_size_custom ps);
        pdf_page_size_options& reset_sizes();
        paper_size get_page_size() const;
        pdf_paper_size_custom get_custom_option() const;
        custom_size_type get_custom_size() const;
        bool is_custom_size_used() const;
};

class pdf_page_scale_options
{
    public:
        using custom_scale_type = std::pair<double, double>;
    private:
        pdf_page_scale m_pagescale;
        custom_scale_type m_pagescale_custom;
    public:
        static constexpr double default_scale = std::numeric_limits<double>::max();
        pdf_page_scale_options();
        pdf_page_scale_options& set_custom_scale(double xScale, double yScale);
        pdf_page_scale_options& set_page_scale(pdf_page_scale ps);
        pdf_page_scale_options& reset_scaling();
        pdf_page_scale get_page_scale() const;
        custom_scale_type get_custom_scale() const;
        bool is_custom_scale_used() const;
};

class pdf_options
{
    public:
        using pdf_custom_scale = std::pair<double, double>;
        using pdf_page_size = std::pair<double, double>;
        
    private:
        std::string m_author;
        std::string m_creator;
        std::string m_producer;
        std::string m_keywords;
        std::string m_subject;
        std::string m_title;
        bool m_useASCII;
        int m_jpegQuality;
        pdf_orientation m_orientation;
        pdf_page_scale_options m_scale_options;
        pdf_page_size_options m_size_options;
        pdf_encryption_options m_encryptOptions;
        
    public:
        pdf_options();
        pdf_options& set_author(const std::string& value);
        pdf_options& set_creator(const std::string& value);
        pdf_options& set_producer(const std::string& value);
        pdf_options& set_keywords(const std::string& value);
        pdf_options& set_subject(const std::string& value);
        pdf_options& set_title(const std::string& value); 
        pdf_options& set_use_ASCII(bool bSet = true);
        pdf_options& set_jpeg_quality(int quality);
        pdf_options& set_orientation(pdf_orientation orient);
        
        pdf_encryption_options& get_encryption_options();
        pdf_page_size_options& get_page_size_options();  
        pdf_page_scale_options& get_page_scale_options();
        std::string get_author() const;          
        std::string get_creator() const;         
        std::string get_producer() const;        
        std::string get_keywords() const;        
        std::string get_subject() const;         
        std::string get_title() const;           
        bool is_use_ASCII() const;               
        int get_jpeg_quality() const;            
        pdf_orientation get_orientation() const; 
};
