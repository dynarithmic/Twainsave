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
// Image formats that are supported by DTWAIN

// miscellaneous file flags
enum class file_transfer_flags
{
    use_name = DTWAIN_USELONGNAME,
    use_prompt = DTWAIN_USEPROMPT
};

enum class multipage_save_mode
{
    save_default = DTWAIN_FILESAVE_DEFAULT,
    save_uiclose = DTWAIN_FILESAVE_UICLOSE,
    save_sourceclose = DTWAIN_FILESAVE_SOURCECLOSE,
    save_manual	 = DTWAIN_FILESAVE_MANUALSAVE,
    save_endacquire = DTWAIN_FILESAVE_ENDACQUIRE,
    save_incomplete = DTWAIN_FILESAVE_SAVEINCOMPLETE
};

class multipage_save_info
{
    multipage_save_mode m_save_mode;
    bool m_save_incomplete;
    public:
        multipage_save_info() : m_save_mode(multipage_save_mode::save_default), m_save_incomplete(false) {}
        multipage_save_info& set_save_mode(multipage_save_mode mt) { m_save_mode = mt; return *this; }
        multipage_save_info& set_save_incomplete(bool bSet = true) { m_save_incomplete = bSet; return *this; }
        multipage_save_mode  get_save_mode() const { return m_save_mode; }
        bool is_save_incomplete() const { return m_save_incomplete; }
};

enum class file_type
{
    none = -1,

    // These files are supported by all TWAIN devices using DTWAIN
    bmp = DTWAIN_BMP,
    dcx = DTWAIN_DCX,
    enhancedmetafile = DTWAIN_EMF,
    gif = DTWAIN_GIF,
    googlewebp = DTWAIN_WEBP,
    jpeg = DTWAIN_JPEG,
    jpeg2k = DTWAIN_JPEG2000,
    pcx = DTWAIN_PCX,
    pdf = DTWAIN_PDF,
    png = DTWAIN_PNG,
    postscript1 = DTWAIN_POSTSCRIPT1,
    postscript2 = DTWAIN_POSTSCRIPT2,
    psd = DTWAIN_PSD,
    targa = DTWAIN_TGA,
    text = DTWAIN_TEXT,
    tiffdeflate = DTWAIN_TIFFDEFLATE,
    tiffgroup3 = DTWAIN_TIFFG3,
    tiffgroup4 = DTWAIN_TIFFG4,
    tiffjpeg = DTWAIN_TIFFJPEG,
    tifflzw = DTWAIN_TIFFLZW,
    tiffnocompress = DTWAIN_TIFFNONE,
    tiffpackbits = DTWAIN_TIFFPACKBITS,
    windowsicon = DTWAIN_ICO,
    windowsmetafile = DTWAIN_WMF,
    wirelessbmp = DTWAIN_WBMP,

    // These files are only supported by the TWAIN driver itself (DTWAIN requires
    // your TWAIN device to support one or more of these types)
    bmp_source_mode = DTWAIN_FF_BMP,
    tiff_source_mode = DTWAIN_FF_TIFF,
    pict_source_mode = DTWAIN_FF_PICT,
    xbm_source_mode = DTWAIN_FF_XBM,
    jfif_source_mode = DTWAIN_FF_JFIF,
    fpx_source_mode = DTWAIN_FF_FPX,
    tiffmulti_source_mode = DTWAIN_FF_TIFFMULTI,
    png_source_mode = DTWAIN_FF_PNG,
    spiff_source_mode = DTWAIN_FF_SPIFF,
    exif_source_mode = DTWAIN_FF_EXIF,
    pdf_source_mode = DTWAIN_FF_PDF,
    jp2_source_mode = DTWAIN_FF_JP2,
    jpx_source_mode = DTWAIN_FF_JPX,
    dejavu_source_mode = DTWAIN_FF_DEJAVU,
    pdfa_source_mode = DTWAIN_FF_PDFA,
    pdfa2_source_mode = DTWAIN_FF_PDFA2,
    pdfraster_source_mode = DTWAIN_FF_PDFRASTER
};

// Information on various file types
struct file_type_info
{
    static LONG get_multipage_type(file_type ft)
    {
        static std::unordered_map<file_type, LONG> multipage_map =
        {
            {file_type::dcx, DTWAIN_DCX},
            {file_type::pdf, DTWAIN_PDFMULTI},
            {file_type::postscript1, DTWAIN_POSTSCRIPT1MULTI},
            {file_type::postscript2, DTWAIN_POSTSCRIPT2MULTI},
            {file_type::tiffdeflate, DTWAIN_TIFFDEFLATEMULTI},
            {file_type::tiffgroup3, DTWAIN_TIFFG3MULTI},
            {file_type::tiffgroup4, DTWAIN_TIFFG4MULTI},
            {file_type::tiffjpeg, DTWAIN_TIFFJPEGMULTI},
            {file_type::tifflzw, DTWAIN_TIFFLZWMULTI},
            {file_type::tiffnocompress, DTWAIN_TIFFNONEMULTI},
            {file_type::tiffpackbits, DTWAIN_TIFFPACKBITSMULTI},
        };

        auto iter = multipage_map.find(ft);
        if (iter != multipage_map.end())
            return iter->second;
        return static_cast<LONG>(ft);
    }

    // returns true if the image type is supported by all TWAIN devices
    static bool is_universal_support(file_type ft)
    {
        static std::unordered_set<file_type> supported_set =
        {
            file_type::bmp,
            file_type::dcx,
            file_type::enhancedmetafile,
            file_type::gif,
            file_type::googlewebp,
            file_type::jpeg,
            file_type::jpeg2k,
            file_type::pcx,
            file_type::pdf,
            file_type::png,
            file_type::postscript1,
            file_type::postscript2,
            file_type::psd,
            file_type::targa,
            file_type::text,
            file_type::tiffdeflate,
            file_type::tiffgroup3,
            file_type::tiffgroup4,
            file_type::tiffjpeg,
            file_type::tifflzw,
            file_type::tiffnocompress,
            file_type::tiffpackbits,
            file_type::windowsicon,
            file_type::windowsmetafile,
            file_type::wirelessbmp
        };
        return supported_set.find(ft) != supported_set.end();
    }
};

// Class that controls the naming of image files when generated
class filename_increment_rules
{
    bool m_bEnable = false;
    int increment_value = 1;
    bool m_resetOnStartup = false;
public:
    filename_increment_rules& set_enabled(bool b = true)
    { m_bEnable = b; return *this; }
    
    filename_increment_rules& set_increment(int value)
    { increment_value = value; return *this; }
    
    filename_increment_rules& set_reset_count(bool bSet = true)
    { m_resetOnStartup = bSet; return *this; }
    
    bool is_enabled() const { return m_bEnable; }
    int get_increment() const { return increment_value; }
    bool is_reset_count() const { return m_resetOnStartup; }
};

