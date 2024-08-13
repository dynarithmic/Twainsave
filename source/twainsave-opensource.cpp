/*
This file is part of the Twainsave-opensource version
Copyright (c) 2002-2024 Dynarithmic Software.

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
// twainsave-opensource.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <boost/program_options/cmdline.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/filesystem.hpp>
#include <boost/uuid/uuid.hpp>            
#include <boost/uuid/uuid_generators.hpp> 
#include <boost/uuid/uuid_io.hpp>         
#include <boost/dll/runtime_symbol_info.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <dynarithmic/twain/twain_session.hpp>
#include <dynarithmic/twain/twain_source.hpp>
#include <dynarithmic/twain/options/pdf_options.hpp>
#include <dynarithmic/twain/acquire_characteristics/acquire_characteristics.hpp>
#include <string>
#include <iostream>
#include <utility>
#include <iomanip>
#include <numeric>
#include <memory>
#include <sstream>
#include <type_traits>
#include <unordered_map>
#include <algorithm>
#include <fstream>
#include <iostream>
#include "..\simpleini\SimpleIni.h"
#include "twainsave_verinfo.h"

std::string generate_details();

template <typename E>
constexpr auto to_underlying(E e) noexcept
{
    return static_cast<std::underlying_type_t<E>>(e);
}

#define INIT_TYPE(x, thetype, y) {#x, dynarithmic::twain::##thetype::##y}
#define INIT_TYPE_2(x, thetype, y) {x, dynarithmic::twain::##thetype::##y}
#define OVERWRITE_ALWAYS    1
#define OVERWRITE_EXIT      2
#define OVERWRITE_CREATENEW 3
#define OVERWRITE_PROMPT    4
#define OVERWRITE_PREFIXSTRING "__"

#define RETURN_OK                          0
#define RETURN_FILENAME_MISSING            1
#define RETURN_TWAIN_INIT_ERROR            2
#define RETURN_TWAIN_SOURCE_ERROR          3
#define RETURN_FILESAVE_ERROR              4
#define RETURN_BAD_COMMAND_LINE            5
#define RETURN_TWAIN_SOURCE_CANCEL         6
#define RETURN_TWAIN_ACQUIRE_ERROR         7
#define RETURN_COLORSPACE_NOT_SUPPORTED    8
#define RETURN_CONSOLE_NOT_AVAILABLE       9
#define RETURN_TIMEOUT_REACHED             10
#define RETURN_TWAIN_UIOPEN_ERROR          11
#define RETURN_FILESAVE_FILEEXISTS          12
#define RETURN_MODE2FILETYPE_NOT_SUPPORTED 13
#define RETURN_UIONLY_SUPPORT_ERROR     14  
#define RETURN_COMMANDFILE_NOT_FOUND    15
#define RETURN_COMMANDFILE_OPEN_ERROR   16

#define TWAINSAVE_DEFAULT_TITLE "TwainSave - OpenSource"
#define TWAINSAVE_INI_FILE "twainsave.ini"

std::unique_ptr<dynarithmic::twain::twain_source> g_source;

struct scanner_options
{
    std::string m_filetype;
    std::string m_filename;
    std::string m_area;
    bool m_bUseADF;
    bool m_bUseADFOrFlatbed;
    bool m_bCreateDir;
    bool m_bAutobrightMode;
    double m_brightness;
    double m_dContrast;
    double m_dGamma;
    int m_bitsPerPixel;
    int m_color;
    int m_nPrinter;
    bool m_bUseDuplex;
    bool m_bDeskew;
    bool m_bShowDetails;
    bool m_bShowHelp;
    bool m_bAutoRotateMode;
    std::string m_strHalftone;
    int m_Orientation;
    bool m_bNoUI;
    double m_dHighlight;
    double m_dThreshold;
    double m_dRotation;
    bool m_bShowIndicator;
    bool m_bShowUIOnly;
    double m_dShadow;
    bool m_bNoBlankPages;
    bool m_bNoConsole;
    bool m_bNoPause;
    double m_dResolution;
    double m_dBlankThreshold;
    bool m_bNegateImage;
    bool m_bSelectByDialog;
    bool m_bSelectDefault;
    bool m_bUIPerm;
    std::string m_strSelectName;
    std::string m_strImprinter;
    int m_nOverwriteMode;
    int m_nOverwriteMax;
    bool m_bOverscanMode;
    bool m_bOptionCheck;
    int m_NumPages;
    std::string m_strPaperSize;
    std::string m_strDetailsFile;
    bool m_bNoUIWait;
    int m_NoUIWaitTime;
    bool m_bSaveOnCancel;
    bool m_bShowVersion;
    bool m_bMultiPage;
    bool m_bMultiPage2;
    bool m_bUseDSM2;
    std::string m_strTempDirectory;
    int m_DSMSearchOrder;
    std::unordered_map<std::string, dynarithmic::twain::filetype_value::value_type> m_FileTypeMap;
    std::unordered_map<int, dynarithmic::twain::color_value::value_type> m_ColorTypeMap;
    std::unordered_map<int, dynarithmic::twain::orientation_value::value_type> m_OrientationTypeMap;
    std::unordered_map<std::string, dynarithmic::twain::supportedsizes_value::value_type> m_PageSizeMap;
    std::array<long, 4> m_errorLevels;
    bool m_bUseFileInc;
    int m_FileIncrement;
    int m_nTransferMode;
    int m_nDiagnose;
    std::string m_DiagnoseLog;
    std::string m_scaling;
    bool m_bUseTransparencyUnit;
    std::string m_strUnitOfMeasure;
    int m_nJobControl;
    int m_nJpegQuality;
    int m_nOverwriteCount;
    int m_nOverwriteWidth;
    std::string m_strLanguage;
    std::unordered_map<std::string, dynarithmic::twain::units_value::value_type> m_MeasureUnitMap;
    std::unordered_map<int, dynarithmic::twain::jobcontrol_value::value_type> m_JobControlMap;
    std::unordered_map<std::string, dynarithmic::twain::pdf_options::pdf_permission> m_PDFEncryptMap;
    std::unordered_map<std::string, dynarithmic::twain::pdf_options::pdf_permission> m_PDFEncryptMapOff;
    std::unordered_map<std::string, std::pair<dynarithmic::twain::filetype_value::value_type, dynarithmic::twain::compression_value::value_type>> m_MapMode2Map;
    std::unordered_map<std::string, TW_UINT16> m_OptionToCapMap;
    int twainsave_return_value;
    std::string m_strConfigFile;
    struct TwainDialogConfig
    {
        std::string m_strTwainTitle = TWAINSAVE_DEFAULT_TITLE;
        bool m_sortedNames = true;
        bool m_horizscroll = false;
        bool m_topmost = true;
        std::string m_language = "default";
        std::pair<int, int> m_position = { (std::numeric_limits<int>::min)(), (std::numeric_limits<int>::min)() };
    };
    TwainDialogConfig m_DialogConfig;

    bool m_bUseVerbose;

    scanner_options() : twainsave_return_value(RETURN_OK),
                            m_nOverwriteCount(1),
                            m_nOverwriteWidth(1),
                            m_FileTypeMap{ 
                            INIT_TYPE(bmp, filetype_value, bmp),
                            INIT_TYPE(bmprle, filetype_value, bmprle),
                            INIT_TYPE(gif, filetype_value,gif),
                            INIT_TYPE(pcx, filetype_value,pcx),
                            INIT_TYPE(dcx, filetype_value,dcx),
                            INIT_TYPE(pdf, filetype_value,pdf),
                            INIT_TYPE(ico, filetype_value,windowsicon),
                            INIT_TYPE(png, filetype_value,png),
                            INIT_TYPE(tga, filetype_value,targa),
                            INIT_TYPE(tgarle, filetype_value, targarle),
                            INIT_TYPE(psd, filetype_value,psd),
                            INIT_TYPE(emf, filetype_value,enhancedmetafile),
                            INIT_TYPE(wbmp, filetype_value,wirelessbmp),
                            INIT_TYPE(wmf, filetype_value,windowsmetafile),
                            INIT_TYPE(jpeg, filetype_value,jpeg),
                            INIT_TYPE(jp2, filetype_value,jpeg2k),
                            INIT_TYPE(tif1, filetype_value,tiffnocompress),
                            INIT_TYPE(tif2, filetype_value,tiffpackbits),
                            INIT_TYPE(tif3, filetype_value, tiffgroup3),
                            INIT_TYPE(tif4, filetype_value, tiffgroup4),
                            INIT_TYPE(tif5, filetype_value, tiffjpeg),
                            INIT_TYPE(tif6, filetype_value, tiffdeflate),
                            INIT_TYPE(tif7, filetype_value, tifflzw),
                            INIT_TYPE(ps1, filetype_value, postscript1),
                            INIT_TYPE(ps2, filetype_value, postscript2),
                            INIT_TYPE(webp, filetype_value, googlewebp) },

                        m_ColorTypeMap{
                            INIT_TYPE_2(0, color_value, bw),
                            INIT_TYPE_2(1, color_value, gray),
                            INIT_TYPE_2(2, color_value, rgb),
                            INIT_TYPE_2(3, color_value, palette),
                            INIT_TYPE_2(4, color_value, cmy),
                            INIT_TYPE_2(5, color_value, cmyk) },

                            m_PageSizeMap{
                            INIT_TYPE(custom, papersize_value, CUSTOM),
                            INIT_TYPE(variable, papersize_value, VARIABLE),
                            INIT_TYPE(letter, papersize_value, USLETTER),
                            INIT_TYPE(none, papersize_value, NONE),
                            INIT_TYPE(legal, papersize_value, USLEGAL),
                            INIT_TYPE(ledger, papersize_value, USLEDGER),
                            INIT_TYPE(executive, papersize_value, USEXECUTIVE),
                            INIT_TYPE(statement, papersize_value, USSTATEMENT),
                            INIT_TYPE(businesscard, papersize_value, BUSINESSCARD),
                            INIT_TYPE(4A0, papersize_value, FOURA0),
                            INIT_TYPE(2A0, papersize_value, TWOA0),
                            INIT_TYPE(JISB0, papersize_value, JISB0),
                            INIT_TYPE(JISB1, papersize_value, JISB1),
                            INIT_TYPE(JISB2, papersize_value, JISB2),
                            INIT_TYPE(JISB3, papersize_value, JISB3),
                            INIT_TYPE(JISB4, papersize_value, JISB4),
                            INIT_TYPE(JISB5, papersize_value, JISB5),
                            INIT_TYPE(JISB6, papersize_value, JISB6),
                            INIT_TYPE(JISB7, papersize_value, JISB7),
                            INIT_TYPE(JISB8, papersize_value, JISB8),
                            INIT_TYPE(JISB9, papersize_value, JISB9),
                            INIT_TYPE(JISB10, papersize_value, JISB10),
                            INIT_TYPE(A0, papersize_value, A0),
                            INIT_TYPE(A1, papersize_value, A1),
                            INIT_TYPE(A2, papersize_value, A2),
                            INIT_TYPE(A3, papersize_value, A3),
                            INIT_TYPE(A4, papersize_value, A4),
                            INIT_TYPE(A5, papersize_value, A5),
                            INIT_TYPE(A6, papersize_value, A6),
                            INIT_TYPE(A7, papersize_value, A7),
                            INIT_TYPE(A8, papersize_value, A8),
                            INIT_TYPE(A9, papersize_value, A9),
                            INIT_TYPE(A10, papersize_value, A10),
                            INIT_TYPE(B0, papersize_value, ISOB0),
                            INIT_TYPE(B1, papersize_value, ISOB1),
                            INIT_TYPE(B2, papersize_value, ISOB2),
                            INIT_TYPE(B3, papersize_value, ISOB3),
                            INIT_TYPE(B4, papersize_value, ISOB4),
                            INIT_TYPE(B5, papersize_value, ISOB5),
                            INIT_TYPE(B6, papersize_value, ISOB6),
                            INIT_TYPE(B7, papersize_value, ISOB7),
                            INIT_TYPE(B8, papersize_value, ISOB8),
                            INIT_TYPE(B9, papersize_value, ISOB9),
                            INIT_TYPE(B10, papersize_value, ISOB10),
                            INIT_TYPE(C0, papersize_value, C0),
                            INIT_TYPE(C1, papersize_value, C1),
                            INIT_TYPE(C2, papersize_value, C2),
                            INIT_TYPE(C3, papersize_value, C3),
                            INIT_TYPE(C4, papersize_value, C4),
                            INIT_TYPE(C5, papersize_value, C5),
                            INIT_TYPE(C6, papersize_value, C6),
                            INIT_TYPE(C7, papersize_value, C7),
                            INIT_TYPE(C8, papersize_value, C8),
                            INIT_TYPE(C9, papersize_value, C9),
                            INIT_TYPE(C10, papersize_value, C10) },

                        m_MeasureUnitMap{
                            INIT_TYPE(inch, units_value, inches),
                            INIT_TYPE(cm, units_value, centimeters),
                            INIT_TYPE(pica, units_value, picas),
                            INIT_TYPE(point, units_value, points),
                            INIT_TYPE(twip, units_value, twips),
                            INIT_TYPE(pixel, units_value, pixels),
                            INIT_TYPE(mm, units_value, millimeters) },

                        m_OrientationTypeMap{
                            INIT_TYPE_2(0, orientation_value, orient_0),
                            INIT_TYPE_2(90, orientation_value, orient_90),
                            INIT_TYPE_2(180, orientation_value, orient_180),
                            INIT_TYPE_2(270, orientation_value, orient_270) },

                        m_JobControlMap{
                            INIT_TYPE_2(0, jobcontrol_value, none),
                            INIT_TYPE_2(1, jobcontrol_value, include_separator),
                            INIT_TYPE_2(2, jobcontrol_value, exclude_separator) },

                        m_PDFEncryptMap{
                            INIT_TYPE(modify, pdf_options::pdf_permission, modify),
                            INIT_TYPE(copy, pdf_options::pdf_permission, copy),
                            INIT_TYPE(modannot, pdf_options::pdf_permission, modifyannotations),
                            INIT_TYPE(fillin, pdf_options::pdf_permission, fillin),
                            INIT_TYPE(extract, pdf_options::pdf_permission, extract),
                            INIT_TYPE(assembly, pdf_options::pdf_permission, assembly),
                            INIT_TYPE(degradeprint, pdf_options::pdf_permission, degradedprint),
                            INIT_TYPE(print, pdf_options::pdf_permission, print),
                            INIT_TYPE(all, pdf_options::pdf_permission, all)
                        },

                        m_PDFEncryptMapOff{
                            INIT_TYPE(nomodify, pdf_options::pdf_permission, modify),
                            INIT_TYPE(nocopy, pdf_options::pdf_permission, copy),
                            INIT_TYPE(nomodannot, pdf_options::pdf_permission, modifyannotations),
                            INIT_TYPE(nofillin, pdf_options::pdf_permission, fillin),
                            INIT_TYPE(noextract, pdf_options::pdf_permission, extract),
                            INIT_TYPE(noassembly, pdf_options::pdf_permission, assembly),
                            INIT_TYPE(nodegradeprint, pdf_options::pdf_permission, degradedprint),
                            INIT_TYPE(noprint, pdf_options::pdf_permission, print),
                            INIT_TYPE(none, pdf_options::pdf_permission, all)
                        },

        m_MapMode2Map{ {"bmp1_mode2",{dynarithmic::twain::filetype_value::bmp_source_mode, dynarithmic::twain::compression_value::none}},
                   {"bmp2_mode2",{ dynarithmic::twain::filetype_value::bmp_source_mode, dynarithmic::twain::compression_value::rle4}},
                   {"bmp3_mode2",{ dynarithmic::twain::filetype_value::bmp_source_mode, dynarithmic::twain::compression_value::rle8}},
                   {"bmp4_mode2",{ dynarithmic::twain::filetype_value::bmp_source_mode, dynarithmic::twain::compression_value::bitfields}},
                   {"dejavu_mode2",{ dynarithmic::twain::filetype_value::dejavu_source_mode, dynarithmic::twain::compression_value::none}},
                   { "exif_mode2",{ dynarithmic::twain::filetype_value::exif_source_mode, dynarithmic::twain::compression_value::none }},
                   { "fpx_mode2",{ dynarithmic::twain::filetype_value::fpx_source_mode, dynarithmic::twain::compression_value::none }},
                   { "jfif_mode2",{ dynarithmic::twain::filetype_value::jfif_source_mode, dynarithmic::twain::compression_value::jpeg }},
                   { "jpeg_mode2",{ dynarithmic::twain::filetype_value::jpeg, dynarithmic::twain::compression_value::none }},
                   { "jp2_mode2",{ dynarithmic::twain::filetype_value::jp2_source_mode, dynarithmic::twain::compression_value::jpeg2000 }},
                   { "jpx_mode2",{ dynarithmic::twain::filetype_value::jpx_source_mode, dynarithmic::twain::compression_value::none }},
                   { "pdf_mode2",{ dynarithmic::twain::filetype_value::pdf_source_mode, dynarithmic::twain::compression_value::none } },
                   { "pdfa1_mode2",{ dynarithmic::twain::filetype_value::pdfa_source_mode, dynarithmic::twain::compression_value::none } },
                   { "pdfa2_mode2",{ dynarithmic::twain::filetype_value::pdfa2_source_mode, dynarithmic::twain::compression_value::none } },
                   { "pict_mode2",{ dynarithmic::twain::filetype_value::pict_source_mode, dynarithmic::twain::compression_value::none } },
                   { "png_mode2",{ dynarithmic::twain::filetype_value::png_source_mode, dynarithmic::twain::compression_value::png } },
                   { "spiff1_mode2",{ dynarithmic::twain::filetype_value::spiff_source_mode, dynarithmic::twain::compression_value::jpeg } },
                   { "spiff2_mode2",{ dynarithmic::twain::filetype_value::spiff_source_mode, dynarithmic::twain::compression_value::jbig } },
                   { "tiff1_mode2",{ dynarithmic::twain::filetype_value::tiff_source_mode, dynarithmic::twain::compression_value::none } },
                   { "tiff2_mode2",{ dynarithmic::twain::filetype_value::tiff_source_mode, dynarithmic::twain::compression_value::group31D } },
                   { "tiff3_mode2",{ dynarithmic::twain::filetype_value::tiff_source_mode, dynarithmic::twain::compression_value::group31DEOL } },
                   { "tiff4_mode2",{ dynarithmic::twain::filetype_value::tiff_source_mode, dynarithmic::twain::compression_value::group32D } },
                   { "tiff5_mode2",{ dynarithmic::twain::filetype_value::tiff_source_mode, dynarithmic::twain::compression_value::group4} },
                   { "tiff6_mode2",{ dynarithmic::twain::filetype_value::tiff_source_mode, dynarithmic::twain::compression_value::jpeg } },
                   { "tiff7_mode2",{ dynarithmic::twain::filetype_value::tiff_source_mode, dynarithmic::twain::compression_value::lzw } },
                   { "tiff8_mode2",{ dynarithmic::twain::filetype_value::tiff_source_mode, dynarithmic::twain::compression_value::jbig } },
                   { "tiff9_mode2",{ dynarithmic::twain::filetype_value::tiff_source_mode, dynarithmic::twain::compression_value::zip } },
                   { "xbm_mode2",{ dynarithmic::twain::filetype_value::xbm_source_mode, dynarithmic::twain::compression_value::none} } },

        m_OptionToCapMap{ {"autobright", ICAP_AUTOBRIGHT},
                         {"deskew", ICAP_AUTOMATICDESKEW},
                         {"autorotate", ICAP_AUTOMATICROTATE},
                         {"brightness", ICAP_BRIGHTNESS},
                         {"transparency", ICAP_LIGHTPATH},
                         {"contrast", ICAP_CONTRAST},
                         {"highlight", ICAP_HIGHLIGHT},
                         {"threshold", ICAP_THRESHOLD},
                         {"gamma", ICAP_GAMMA},
                         {"halftone", ICAP_HALFTONES},
                         {"resolution", ICAP_XRESOLUTION},
                         {"rotation", ICAP_ROTATION},
                         {"shadow", ICAP_SHADOW},
                         {"overscan", ICAP_OVERSCAN},
                         {"showindicator", CAP_INDICATORS},
                         {"color", ICAP_PIXELTYPE},
                         {"unitofmeasure", ICAP_UNITS},
                         {"papersize", ICAP_SUPPORTEDSIZES},
                         {"orientation", ICAP_ORIENTATION},
                         {"imprinterstring", CAP_PRINTER},
                         {"jobcontrol", CAP_JOBCONTROL},
                         {"nouiwait",CAP_PAPERDETECTABLE},
                         {"nouiwaittime",0} }
    {
        m_errorLevels[0] = DTWAIN_LOG_USEFILE | DTWAIN_LOG_CALLSTACK;
        m_errorLevels[1] = m_errorLevels[0] | DTWAIN_LOG_DECODE_DEST | DTWAIN_LOG_DECODE_SOURCE;
        m_errorLevels[2] = m_errorLevels[1] | DTWAIN_LOG_DECODE_TWMEMREF;
        m_errorLevels[3] = m_errorLevels[2] | DTWAIN_LOG_DECODE_TWEVENT;
    }

    void set_return_code(int returncode) { twainsave_return_value = returncode; }
    int get_return_code() const { return twainsave_return_value; }
};

struct pdf_controls
{
    bool m_bAscii;
    bool m_bStrong;
    bool m_bWeak;
    bool m_bEncrypt;
    bool m_bRandomOwner;
    bool m_bRandomUser;
    int m_quality;
    std::string m_strAuthor;
    std::string m_strCreator;
    std::string m_strProducer;
    std::string m_strKeywords;
    std::string m_strSubject;
    std::string m_strTitle;
    std::string m_strOwnerPass;
    std::string m_strUserPass;
    std::string m_strPermissions;
    std::string m_strPaperSize;
    std::string m_strOrient;
    std::string m_strScale;
};


using namespace dynarithmic::twain;
namespace po = boost::program_options;

std::unique_ptr<po::options_description> desc2;

int NumDigits(int x)
{
    x = abs(x);
    return (x < 10 ? 1 :
        (x < 100 ? 2 :
        (x < 1000 ? 3 :
            (x < 10000 ? 4 :
            (x < 100000 ? 5 :
                (x < 1000000 ? 6 :
                (x < 10000000 ? 7 :
                    (x < 100000000 ? 8 :
                    (x < 1000000000 ? 9 :
                        10)))))))));
}

std::vector<std::string> SplitPath(const boost::filesystem::path &src) 
{
    std::vector<std::string> elements;
    for (const auto &p : src)
        elements.push_back(p.filename().string()); 
    return elements;
}

std::string GetNewFileName(const std::string& fullpath, int inc, int maxWidth)
{
    auto vString = SplitPath(boost::filesystem::path(fullpath));
    auto newName = vString.back();
    boost::filesystem::path theStem(newName);

    // Get just the filename part
    std::string file_part = theStem.stem().string();

    // Get just the extension
    auto ext = theStem.extension().string();
    std::ostringstream strm;
    strm << ((vString.size()>1)?"/":"") << file_part << OVERWRITE_PREFIXSTRING << std::setw(maxWidth) << std::setfill('0') << inc;
    vString.back() = strm.str();
    auto retval = std::accumulate(vString.begin(), vString.end(), std::string());
    return retval += ext;
}

scanner_options s_options = {};
pdf_controls pdf_commands = {};
std::string default_name;
std::string descript_name;
std::string details_name;

using parse_return_type = std::pair<bool, po::variables_map>;

parse_return_type parse_options(int argc, char *argv[])
{
    boost::uuids::uuid uuid = boost::uuids::random_generator()();
    default_name = boost::uuids::to_string(uuid);
    descript_name = default_name;
    std::replace_if(descript_name.begin(), descript_name.end(), [&](char ch) { return ch != '-'; }, 'x');
    po::command_line_style::style_t style = po::command_line_style::style_t(po::command_line_style::unix_style);
    desc2 = std::make_unique<po::options_description>();
    try
    {
        desc2->add_options()
            ("area", po::value< std::string >(&s_options.m_area)->default_value(""), "set acquisition area of image to acquire")
            ("autobright", po::bool_switch(&s_options.m_bAutobrightMode)->default_value(false), "turn on autobright feature")
            ("autofeed", po::bool_switch(&s_options.m_bUseADF)->default_value(false), "turn on automatic document feeder")
            ("autofeedorflatbed", po::bool_switch(&s_options.m_bUseADFOrFlatbed)->default_value(false), "use feeder if not empty, else use flatbed")
            ("autorotate", po::bool_switch(&s_options.m_bAutoRotateMode)->default_value(false), "Detect if document should be rotated.  Device must support autorotate")
            ("bitsperpixel", po::value< int >(&s_options.m_bitsPerPixel)->default_value(0), "Image bits-per-pixel.  Default is current device setting")
            ("blankthreshold", po::value< double >(&s_options.m_dBlankThreshold)->default_value(98), "Percentage threshold to determine if page is blank")
            ("brightness", po::value< double >(&s_options.m_brightness)->default_value(0), "Brightness level (device must support brightness)")
            ("color", po::value< int >(&s_options.m_color)->default_value(0), "Color. 0=B/W, 1=Grayscale, 2=RGB, 3=Palette, 4=CMY, 5=CMYK. Default is 0")
            ("contrast", po::value< double >(&s_options.m_dContrast)->default_value(0), "Contrast level (device must support contrast)")
            ("createdir", po::bool_switch(&s_options.m_bCreateDir)->default_value(false), "Create the directory specified by --filename if directory does not exist")
            ("deskew", po::bool_switch(&s_options.m_bDeskew)->default_value(false), "Deskew image if skewed.  Device must support deskew")
            ("details", po::bool_switch(&s_options.m_bShowDetails)->default_value(false), "Detail information on all available TWAIN devices.")
            ("diagnose", po::value< int >(&s_options.m_nDiagnose)->default_value(0), "Create diagnostic log.  Level values 1, 2, 3 or 4.")
            ("diagnoselog", po::value< std::string >(&s_options.m_DiagnoseLog)->default_value("stddiag.log"), "file name to store -diagnose messages")
            ("dsmsearchorder", po::value< int >(&s_options.m_DSMSearchOrder)->default_value(0), "Directories TwainSave will search when locating TWAIN_32.DLL or TWAINDSM.DLL")
            ("duplex", po::bool_switch(&s_options.m_bUseDuplex)->default_value(false), "turn on duplex unit")
            ("filename", po::value< std::string >(&s_options.m_filename)->default_value(descript_name), "file name to save acquired image(s)")
            ("filetype", po::value< std::string >(&s_options.m_filetype)->default_value("bmp"), "Image file type")
            ("gamma", po::value< double >(&s_options.m_dGamma)->default_value(0), "Gamma level (device must support gamma levels)")
            ("help", po::bool_switch(&s_options.m_bShowHelp)->default_value(false), "Show help screen")
            ("halftone", po::value< std::string >(&s_options.m_strHalftone)->default_value("none"), "Halftone effect to use when acquiring low resolution images")
            ("highlight", po::value< double >(&s_options.m_dHighlight)->default_value(255), "Highlight level (device must support highlight)")
            ("imprinter", po::value< int >(&s_options.m_nPrinter)->default_value(-1), "Select imprinter to use (0-7)")
            ("imprinterstring", po::value< std::string >(&s_options.m_strImprinter)->default_value(""), "Set imprinter string")
            ("incvalue", po::value< int >(&s_options.m_FileIncrement)->default_value(1), "File name counter")
            ("jobcontrol", po::value< int >(&s_options.m_nJobControl)->default_value(0), "0=none, 1=include job page, 2=exclude job page")
            ("jquality", po::value< int >(&s_options.m_nJpegQuality)->default_value(75), "Quality Factor when acquiring JPEG images.  Default is 75")
            ("language", po::value< std::string >(&s_options.m_strLanguage)->default_value("english"), "Set language in Twain dialog")
            ("multipage", po::bool_switch(&s_options.m_bMultiPage)->default_value(false), "Save to multipage file")
            ("multipage2", po::bool_switch(&s_options.m_bMultiPage2)->default_value(false), "Save to multipage file only after closing UI")
            ("negate", po::bool_switch(&s_options.m_bNegateImage)->default_value(false), "Negates (reverses polarity) of acquired images")
            ("noblankpages", po::bool_switch(&s_options.m_bNoBlankPages)->default_value(false), "Remove blank pages")
            ("noconsole", po::bool_switch(&s_options.m_bNoConsole)->default_value(false), "Start TwainSave without a console window")
            ("nopause", po::bool_switch(&s_options.m_bNoPause)->default_value(false), "Do not pause TwainSave if --noconsole is used")
            ("noui", po::bool_switch(&s_options.m_bNoUI)->default_value(false), "turn off device user interface")
            ("nouiwait", po::bool_switch(&s_options.m_bNoUIWait)->default_value(false), "Do not display Source user interface and wait for feeder loaded before acquiring")
            ("nouiwaittime", po::value< int >(&s_options.m_NoUIWaitTime)->default_value(120), "Time to wait (in seconds) for feeder loaded.")
            ("numpages", po::value< int >(&s_options.m_NumPages)->default_value(0), "Number of pages to acquire.  Default is 0 (acquire all pages)")
            ("optioncheck", po::bool_switch(&s_options.m_bOptionCheck)->default_value(false), "Check what options selected device supports")
            ("orientation", po::value< int >(&s_options.m_Orientation)->default_value(0), "Clockwise orientation in degrees (0, 90, 180, 270)")
            ("overscanmode", po::bool_switch(&s_options.m_bOverscanMode)->default_value(false), "Turn on overscan mode.  Device must support overscan")
            ("overwritemax", po::value< int >(&s_options.m_nOverwriteMax)->default_value(9999), "Sets the maximum number of files created per acquisition for \"--overwritemode 3\"")
            ("overwritemode", po::value< int >(&s_options.m_nOverwriteMode)->default_value(1), "Mode to use when file already exists.  Default is 1 (always overwrite existing file)")
            ("papersize", po::value< std::string >(&s_options.m_strPaperSize)->default_value("letter"), "Paper size.  Default is \"letter\"")
            ("pdfascii", po::bool_switch(&pdf_commands.m_bAscii)->default_value(false), "create ASCII compressed (text-based) PDF files")
            ("pdf128", po::bool_switch(&pdf_commands.m_bStrong)->default_value(false), "use PDF 128-bit (strong) encryption")
            ("pdf40", po::bool_switch(&pdf_commands.m_bWeak)->default_value(true), "use PDF 40-bit encryption")
            ("pdfauthor", po::value< std::string >(&pdf_commands.m_strAuthor)->default_value(""), "Sets the PDF Author field")
            ("pdfcreator", po::value< std::string >(&pdf_commands.m_strCreator)->default_value(""), "Sets the PDF Creator field")
            ("pdfkeywords", po::value< std::string >(&pdf_commands.m_strKeywords)->default_value(""), "Sets the PDF Keywords field")
            ("pdfproducer", po::value< std::string >(&pdf_commands.m_strProducer)->default_value(""), "Sets the PDF Producer field")
            ("pdfsubject", po::value< std::string >(&pdf_commands.m_strSubject)->default_value(""), "Sets the PDF Subject field")
            ("pdftitle", po::value< std::string >(&pdf_commands.m_strTitle)->default_value(""), "Sets the PDF Title field")
            ("pdfencrypt", po::bool_switch(&pdf_commands.m_bEncrypt)->default_value(false), "Turn on PDF encryption")
            ("pdfownerpass", po::value< std::string >(&pdf_commands.m_strOwnerPass)->default_value(""), "Sets the PDF owner password")
            ("pdfuserpass", po::value< std::string >(&pdf_commands.m_strUserPass)->default_value(""), "Sets the PDF user password")
            ("pdfrandowner", po::bool_switch(&pdf_commands.m_bRandomOwner)->default_value(false), "Use random PDF owner password.  Cannot be used with --pdfownerpass")
            ("pdfranduser", po::bool_switch(&pdf_commands.m_bRandomUser)->default_value(false), "Use random PDF Userr password.  Cannot be used with --pdfuserpass")
            ("pdfpermit", po::value< std::string >(&pdf_commands.m_strPermissions)->default_value(""), "PDF permissions allowed for encrypted files")
            ("pdfsize", po::value< std::string >(&pdf_commands.m_strPaperSize)->default_value("letter"), "PDF Paper size.  Default is \"letter\"")
            ("pdfquality", po::value< int >(&pdf_commands.m_quality)->default_value(60), "set the JPEG quality factor for PDF files")
            ("pdforient", po::value< std::string >(&pdf_commands.m_strOrient)->default_value("portrait"), "Sets orientation to portrait or landscape")
            ("pdfscale", po::value< std::string >(&pdf_commands.m_strScale)->default_value("noscale"), "PDF page scaling")
            ("resolution", po::value< double >(&s_options.m_dResolution)->default_value(0), "Image resolution in dots per unit (see --unit)")
            ("rotation", po::value< double >(&s_options.m_dRotation)->default_value(0.0), "Rotate page by the specified number of degrees (device must support rotation)")
            ("saveoncancel", po::bool_switch(&s_options.m_bSaveOnCancel)->default_value(false), "Save image file even if acquisition canceled by user")
            ("scale", po::value< std::string >(&s_options.m_scaling)->default_value(""), "set x/y scaling options")
            ("selectbydialog", po::bool_switch(&s_options.m_bSelectByDialog)->default_value(true), "When selecting device, show \"Select Source\" dialog (Default)")
            ("selectbyname", po::value< std::string >(&s_options.m_strSelectName)->default_value(""), "Select TWAIN device by specifying device product name")
            ("selectdefault", po::bool_switch(&s_options.m_bSelectDefault)->default_value(false), "Select the default TWAIN device automatically")
            ("shadow", po::value< double >(&s_options.m_dShadow)->default_value(0), "Shadow level (device must support shadow levels)")
            ("showindicator", po::bool_switch(&s_options.m_bShowIndicator)->default_value(false), "Show progress indicator when no user-interface is chosen (-noui)")
            ("tempdir", po::value< std::string >(&s_options.m_strTempDirectory)->default_value(""), "Temporary file directory")
            ("threshold", po::value< double >(&s_options.m_dThreshold)->default_value(0), "Threshold level (device must support threshold)")
            ("transfermode", po::value< int >(&s_options.m_nTransferMode)->default_value(0), "Transfer mode. 0=Native, 1=Buffered")
            ("transparency", po::bool_switch(&s_options.m_bUseTransparencyUnit)->default_value(false), "Use transparency unit")
            ("uionly", po::bool_switch(&s_options.m_bShowUIOnly)->default_value(false), "Allow user interface to be shown without acquiring images")
            ("uiperm", po::bool_switch(&s_options.m_bUIPerm)->default_value(false), "Leave UI open on successful acquisition")
            ("unitofmeasure", po::value< std::string >(&s_options.m_strUnitOfMeasure)->default_value("inch"), "Unit of measure")
            ("usedsm2", po::bool_switch(&s_options.m_bUseDSM2)->default_value(false), "Use TWAINDSM.DLL if found as the data source manager.")
            ("useinc", po::bool_switch(&s_options.m_bUseFileInc)->default_value(false), "Use file name increment")
            ("verbose", po::bool_switch(&s_options.m_bUseVerbose)->default_value(false), "Turn on verbose mode")
            ("version", po::bool_switch(&s_options.m_bShowVersion)->default_value(false), "Display program version")
            ("@", po::value< std::string >(&s_options.m_strConfigFile)->default_value(""), "Configuration file");
        po::variables_map vm2;
        po::store(po::parse_command_line(argc, argv, *desc2, style), vm2);
        po::notify(vm2);
        return{ true, vm2 };
    }
    catch (const boost::program_options::error_with_option_name& /*e*/)
    {
        s_options.set_return_code(RETURN_BAD_COMMAND_LINE);
    }
    return{ false, po::variables_map() };
}

template <typename SelectType>
twain_source select_the_source(twain_session& tsession, SelectType s)
{
    return tsession.select_source(s);
}

template <typename T>
static bool constexpr is_rangeable()
{
    return std::is_same<T, double>::value || std::is_same<T, int>::value;
}

template <typename T>
static bool constexpr is_stringtype()
{
    return std::is_same<T, std::string>::value;
}

template <typename T>
static bool constexpr is_booltype()
{
    return std::is_same<T, bool>::value;
}

template <typename T, bool isRange=false>
static void options_writer(twain_source& theSource, std::string entry, const std::vector<T>& testArray, T value, bool valuefound)
{

    if constexpr (is_stringtype<T>())
        std::cout << (valuefound ? "Success!  " : "Sorry : ") << "The TWAIN device \"" << theSource.get_source_info().get_product_name() << "\" does" << (valuefound ? " " : " not ")
        << " support the value " << std::quoted(value) << " that you are using for --" << entry << "\n";
    else
        std::cout << (valuefound ? "Success!  " : "Sorry : ") << "The TWAIN device \"" << theSource.get_source_info().get_product_name() << "\" does" << (valuefound ? " " : " not ")
            << " support the value " << value << " that you are using for --" << entry << "\n";
    if (!valuefound)
    {
        std::cout << "\nThe allowable values for --" << entry << " are as follows";
        if constexpr (isRange && is_rangeable<T>())
        {
            std::cout << " (as a range):\n";
            dynarithmic::twain::twain_range<double> testRangeX(testArray);
            std::cout << "Minimum Value: " << testRangeX.get_min() << "\n";
            std::cout << "Maximum Value: " << testRangeX.get_max() << "\n";
            std::cout << "Step Value: " << testRangeX.get_step() << "\n";
        }
        else
        {
            std::cout << ":\n";
            for (auto& v : testArray)
            {
                if constexpr (is_stringtype<T>())
                    std::cout << std::quoted(v) << "\n";
                else
                    std::cout << v << "\n";
            }
        }
    }
}

template <typename T>
struct RangeCharacteristicTester
{
    static void test(twain_source& theSource, std::string entry, T value, int capvalue = 0)
    {
        // now test if the device can actually use the value set
        std::vector<T> testArray;
        if constexpr (is_stringtype<T>())
            std::cout << "Testing if " << std::quoted(value) << " can be used...\n";
        else
            std::cout << "Testing if " << value << " can be used...\n";

        // Get all the values supported
        testArray = theSource.get_capability_interface().get_cap_values<decltype(testArray)>(capvalue, capability_interface::get());

        // Test if the returned array suggests that the values are in a range
        dynarithmic::twain::twain_range<T> testRangeX(testArray);
        if (testRangeX.is_valid())
            // Handle this as a range and test if value is in the range's domain
            options_writer<T, true>(theSource, entry, testArray, value, testRangeX.value_exists(value));
        else
            // Not a range, so the values in the array are discrete values that can be tested with value
            options_writer(theSource, entry, testArray, value, std::find(testArray.begin(), testArray.end(), value) != testArray.end());
    }
};

template <typename T>
struct GenericCharacteristicTester
{
    static void test(twain_source& theSource, std::string entry, T value, int capvalue = 0)
    {
        // now test if the device can actually use the value set
        std::vector<T> testArray;
        std::cout << "Testing if " << value << " can be used...\n";
        testArray = theSource.get_capability_interface().get_cap_values<decltype(testArray)>(capvalue, capability_interface::get());
        bool valuefound = std::find(testArray.begin(), testArray.end(), value) != testArray.end();
        std::cout << (valuefound ? "Success!  " : "Sorry : ") << "The TWAIN device \"" << theSource.get_source_info().get_product_name() << "\" does" << (valuefound ? " " : " not ")
            << " support the value when requested for --" << entry << "\n";
    }
};

struct BoolCharacteristicTester
{
    static bool test(twain_source& theSource, int capvalue)
    {
        // now test if the device can actually use the value set
        std::vector<TW_BOOL> testArray;
        testArray = theSource.get_capability_interface().get_cap_values<decltype(testArray)>(capvalue, capability_interface::get());
        return find(testArray.begin(), testArray.end(), 1) != testArray.end();
    }
};

template <typename T>
struct DummyCharacteristicTester
{
    static void test(twain_source&, std::string, T value, int) {}
};

template <typename T, typename ValueTester = DummyCharacteristicTester<T>>
std::pair<std::string, bool> test_twainsave_option(twain_source& theSource, // TWAIN source
                         const T& value, // value to test
                         const po::variables_map& varmap, 
                         const std::string& entry, 
                         bool bSkipEntryCheck,
                         int capvalue,
                         bool bTestForTrue = false)
{
    bool issupported = false;
    auto iter = varmap.find(entry);
    if (bSkipEntryCheck || iter != varmap.end())
    {
        if (bSkipEntryCheck || !iter->second.defaulted())
        {
            if (capvalue)
            {
                if ( !bSkipEntryCheck)
                    std::cout << "Checking if device supports --" << entry << " ...\n";
                // test if the source supports what we're supposed to be setting later
                issupported = theSource.get_capability_interface().is_cap_supported(capvalue);

                // do further testing 
                if (bTestForTrue)
                {
                    issupported = BoolCharacteristicTester::test(theSource, capvalue);
                    if ( !bSkipEntryCheck )
                        std::cout << (issupported ? "Success!  " : "Sorry: ") << "The TWAIN device \"" << theSource.get_source_info().get_product_name() << "\" does" << (issupported ? " " : " not ")
                            << "support the \"--" << entry << "\" option\n";
                }
                else
                if (!bSkipEntryCheck)
                {
                    std::cout << (issupported ? "Success!  " : "Sorry: ") << "The TWAIN device \"" << theSource.get_source_info().get_product_name() << "\" does" << (issupported ? " " : " not ")
                        << "support the \"--" << entry << "\" option\n";
                    if (issupported)
                        ValueTester::test(theSource, entry, value, capvalue);
                }
            }
            if ( !bSkipEntryCheck)
                std::cout << std::string(120, '-') << "\n";
        }
    }
    return { entry, issupported };
}

std::string resolve_extension(std::string filetype)
{
    if (boost::starts_with(filetype, "tif"))
        return "tif";
    else
    if (boost::starts_with(filetype, "ps"))
        return "ps";
    else
    if (boost::starts_with(filetype, "bmp"))
        return "bmp";
    else
    if (boost::starts_with(filetype, "tga"))
        return "tga";
    return filetype;
}

void set_blank_page_options(twain_source& mysource, const po::variables_map& varmap)
{
    if (!varmap["noblankpages"].defaulted())
    {
        auto& ac = mysource.get_acquire_characteristics();
        auto& blank_handler = ac.get_blank_page_options();
        blank_handler.
            enable(true).
            set_discard_option(blankpage_discard_option::discard_all);

        if (!varmap["blankpagethreshold"].defaulted())
        {
            double val = s_options.m_dBlankThreshold;
            val = (std::min)((std::max)(0.0, val), 100.0);
            blank_handler.set_threshold(val);
        }
    }
}

void set_scale_options(twain_source& mysource, const po::variables_map& varmap)
{
    if (!varmap["scale"].defaulted())
    {
        auto& ac = mysource.get_acquire_characteristics();
        auto& infoOptions = ac.get_imageparameter_options();

        // parse the scaling information
        std::istringstream strm(s_options.m_scaling);
        double xscale = 100, yscale = 100;
        strm >> xscale >> yscale;
        xscale /= 100.0;
        yscale /= 100.0;
        infoOptions.enable_forced_scaling(true).set_xscaling(xscale).set_yscaling(yscale);
    }
}

bool set_areaofinterest_options(twain_source& mysource, const po::variables_map& varmap)
{
    if (!varmap["area"].defaulted())
    {
        auto& ac = mysource.get_acquire_characteristics();
        // parse the area argument
        std::istringstream strm(s_options.m_area);
        std::vector<double> area_values;
        double value;
        int numItems = 0;
        while (strm >> value && numItems < 5)
        {
            area_values.push_back(value);
            ++numItems;
        }
        if (area_values.size() == 4)
        {
            dynarithmic::twain::twain_frame<double> tf(area_values[0], area_values[1], area_values[2], area_values[3]);
            ac.get_pages_options().set_frame(tf);
        }
        else
        {
            s_options.set_return_code(RETURN_BAD_COMMAND_LINE);
            return false;
        }
    }
    return true;
}

void set_pdf_options(twain_source& mysource, const po::variables_map& varmap)
{
    if (boost::any_cast<std::string>(varmap["filetype"].value()) == "pdf")
    {
        auto& ac = mysource.get_acquire_characteristics();
        auto& pdfopts = ac.get_pdf_options();
        pdfopts.set_author(pdf_commands.m_strAuthor)
            .set_creator(pdf_commands.m_strCreator)
            .set_producer(pdf_commands.m_strProducer)
            .set_keywords(pdf_commands.m_strKeywords)
            .set_subject(pdf_commands.m_strSubject)
            .set_title(pdf_commands.m_strTitle)
            .set_use_ASCII(pdf_commands.m_bAscii)
            .set_jpeg_quality(pdf_commands.m_quality);

        auto& pagesizeopts = pdfopts.get_page_size_options();
        std::istringstream strm(pdf_commands.m_strPaperSize);
        std::string word;
        strm >> word;

        if (word == "custom")
        {
            uint32_t width, height;
            strm >> width >> height;
            pagesizeopts.set_custom_size(width, height).set_custom_option(dynarithmic::twain::pdf_options::pdf_paper_size_custom::custom);
        }
        else
            if (word == "variable")
                pagesizeopts.set_custom_option(dynarithmic::twain::pdf_options::pdf_paper_size_custom::variable);
            else
                pagesizeopts.set_page_size(s_options.m_PageSizeMap[pdf_commands.m_strPaperSize]);

        // get PDF scale options
        {
            auto& pagescaleopts = pdfopts.get_page_scale_options();
            std::istringstream strm(pdf_commands.m_strPaperSize);
            std::string word;
            strm >> word;

            if (word == "custom")
            {
                double xscale, yscale;
                strm >> xscale >> yscale;
                pagescaleopts.set_custom_scale(xscale, yscale);
                pagescaleopts.set_page_scale(dynarithmic::twain::pdf_options::pdf_page_scale::custom);
            }
            else
            if (word == "fitpage")
                pagescaleopts.set_page_scale(dynarithmic::twain::pdf_options::pdf_page_scale::fitpage);
            else
            if (word == "noscale")
                pagescaleopts.set_page_scale(dynarithmic::twain::pdf_options::pdf_page_scale::none);
        }
        // encryption
        bool encryption_on = boost::any_cast<bool>(varmap["pdfencrypt"].value());
        if (!encryption_on)
        {
            std::vector<std::string> encryptcommands = {
                "pdfownerpass", "pdfuserpass", "pdfrandowner",
                "pdfranduser", "pdfpermit", "pdf128" };
            encryption_on = std::find_if(encryptcommands.begin(), encryptcommands.end(), [&](const std::string& s)
                { return !varmap[s].defaulted(); }) != encryptcommands.end();
        }
        if (encryption_on)
        {
            auto& encrypt_opts = pdfopts.get_encryption_options();
            encrypt_opts.use_encryption(true).
                set_owner_password(pdf_commands.m_strOwnerPass).
                set_user_password(pdf_commands.m_strUserPass).
                use_autogen_password(pdf_commands.m_bRandomOwner || pdf_commands.m_bRandomUser).
                use_strong_encryption(pdf_commands.m_bStrong);

            // parse the permissions string
            std::vector<std::string> sAllPermissions;
            std::istringstream ss(pdf_commands.m_strPermissions);
            std::string buf;

            while (ss >> buf)
                sAllPermissions.push_back(buf);

            // Set the permissions here
            LONG Permissions = 0;
            std::vector<std::string>::size_type i;

            // set of our permissions
            std::set<dynarithmic::twain::pdf_options::pdf_permission> permissionContainer;
            for (i = 0; i < sAllPermissions.size(); ++i)
            {
                if (sAllPermissions[i] == "none")
                    permissionContainer.clear();
                if (s_options.m_PDFEncryptMap.find(sAllPermissions[i]) != s_options.m_PDFEncryptMap.end())
                    permissionContainer.insert(s_options.m_PDFEncryptMap[sAllPermissions[i]]);
                if (s_options.m_PDFEncryptMapOff.find(sAllPermissions[i]) != s_options.m_PDFEncryptMapOff.end())
                    permissionContainer.erase(s_options.m_PDFEncryptMapOff[sAllPermissions[i]]);
            }

            encrypt_opts.set_permissions(permissionContainer);
        }
    }
}

bool check_device_options(twain_source& mysource, const po::variables_map& varmap, bool doOptionCheck = true)
{
    // test the characteristics that have been set
    if (!varmap["verbose"].defaulted() || (!varmap["optioncheck"].defaulted() && doOptionCheck))
    {
        std::map<std::string, bool> mapOptions;
        doOptionCheck = doOptionCheck && !varmap["optioncheck"].defaulted();
        mapOptions.insert(test_twainsave_option(mysource, s_options.m_bAutobrightMode, varmap, "autofeed", doOptionCheck, CAP_AUTOFEED));
        mapOptions.insert(test_twainsave_option(mysource, s_options.m_bAutobrightMode, varmap, "autobright", doOptionCheck, ICAP_AUTOBRIGHT));
        mapOptions.insert(test_twainsave_option(mysource, s_options.m_bDeskew, varmap, "deskew", doOptionCheck, ICAP_AUTOMATICDESKEW));
        mapOptions.insert(test_twainsave_option(mysource, s_options.m_bAutoRotateMode, varmap, "autorotate", doOptionCheck, ICAP_AUTOMATICROTATE));
        mapOptions.insert(test_twainsave_option(mysource, s_options.m_bUseTransparencyUnit ? 1 : 0, varmap, "transparency", doOptionCheck, ICAP_LIGHTPATH));
        mapOptions.insert(test_twainsave_option(mysource, s_options.m_dRotation, varmap, "rotation", doOptionCheck, ICAP_ROTATION));
        mapOptions.insert(test_twainsave_option(mysource, s_options.m_bOverscanMode, varmap, "overscan", doOptionCheck, ICAP_OVERSCAN));
        mapOptions.insert(test_twainsave_option(mysource, s_options.m_bOverscanMode, varmap, "overscanmode", doOptionCheck, ICAP_OVERSCAN));
        mapOptions.insert(test_twainsave_option(mysource, s_options.m_bShowIndicator, varmap, "showindicator", doOptionCheck, CAP_INDICATORS));
        mapOptions.insert(test_twainsave_option(mysource, s_options.m_bUseDuplex, varmap, "duplex", doOptionCheck, CAP_DUPLEX));
        mapOptions.insert(test_twainsave_option(mysource, s_options.m_bNoUI, varmap, "noui", doOptionCheck, CAP_UICONTROLLABLE, true));
        mapOptions.insert(test_twainsave_option(mysource, s_options.m_bNoUIWait, varmap, "nouiwait", doOptionCheck, CAP_PAPERDETECTABLE, true));
        mapOptions.insert(test_twainsave_option(mysource, s_options.m_NoUIWaitTime, varmap, "nouiwaittime", doOptionCheck, CAP_PAPERDETECTABLE, true));
        mapOptions.insert(test_twainsave_option(mysource, s_options.m_bUseADF, varmap, "autofeedorflatbed", doOptionCheck, CAP_PAPERDETECTABLE, true));
        mapOptions.insert(test_twainsave_option<std::string, GenericCharacteristicTester<std::string>>(mysource, s_options.m_strHalftone, varmap, "halftone", doOptionCheck, ICAP_HALFTONES));
        mapOptions.insert(test_twainsave_option<double, RangeCharacteristicTester<double>>(mysource, s_options.m_dHighlight, varmap, "highlight", doOptionCheck, ICAP_HIGHLIGHT));
        mapOptions.insert(test_twainsave_option<double, RangeCharacteristicTester<double>>(mysource, s_options.m_dThreshold, varmap, "threshold", doOptionCheck, ICAP_THRESHOLD));
        mapOptions.insert(test_twainsave_option<double, RangeCharacteristicTester<double>>(mysource, s_options.m_dResolution, varmap, "resolution", doOptionCheck, ICAP_XRESOLUTION));
        mapOptions.insert(test_twainsave_option<double, RangeCharacteristicTester<double>>(mysource, s_options.m_dGamma, varmap, "gamma", doOptionCheck, ICAP_GAMMA));
        mapOptions.insert(test_twainsave_option<double, RangeCharacteristicTester<double>>(mysource, s_options.m_brightness, varmap, "brightness", doOptionCheck, ICAP_BRIGHTNESS));
        mapOptions.insert(test_twainsave_option<double, RangeCharacteristicTester<double>>(mysource, s_options.m_dContrast, varmap, "contrast", doOptionCheck, ICAP_CONTRAST));
        mapOptions.insert(test_twainsave_option<double, RangeCharacteristicTester<double>>(mysource, s_options.m_dShadow, varmap, "shadow", doOptionCheck, ICAP_SHADOW));
        mapOptions.insert(test_twainsave_option<TW_UINT16, GenericCharacteristicTester<TW_UINT16>>(mysource, static_cast<TW_UINT16>(s_options.m_ColorTypeMap[s_options.m_color]), varmap, "color", doOptionCheck, ICAP_PIXELTYPE));
        mapOptions.insert(test_twainsave_option<TW_UINT16, GenericCharacteristicTester<TW_UINT16>>(mysource, static_cast<TW_UINT16>(s_options.m_MeasureUnitMap[s_options.m_strUnitOfMeasure]), varmap, "unitofmeasure", doOptionCheck, ICAP_UNITS));
        mapOptions.insert(test_twainsave_option<TW_UINT16, GenericCharacteristicTester<TW_UINT16>>(mysource, static_cast<TW_UINT16>(s_options.m_PageSizeMap[s_options.m_strPaperSize]), varmap, "papersize", doOptionCheck, ICAP_SUPPORTEDSIZES));
        mapOptions.insert(test_twainsave_option<TW_UINT16, GenericCharacteristicTester<TW_UINT16>>(mysource, static_cast<TW_UINT16>(s_options.m_OrientationTypeMap[s_options.m_Orientation]), varmap, "orientation", doOptionCheck, ICAP_ORIENTATION));
        mapOptions.insert(test_twainsave_option(mysource, s_options.m_strImprinter.empty() ? false : true, varmap, "imprinterstring", doOptionCheck, CAP_PRINTER));
        mapOptions.insert(test_twainsave_option<TW_UINT16, GenericCharacteristicTester<TW_UINT16>>(mysource, s_options.m_nPrinter, varmap, "imprinter", doOptionCheck, CAP_PRINTER));
        mapOptions.insert(test_twainsave_option<TW_UINT16, GenericCharacteristicTester<TW_UINT16>>(mysource, static_cast<TW_UINT16>(s_options.m_JobControlMap[s_options.m_nJobControl]), varmap, "jobcontrol", doOptionCheck, CAP_JOBCONTROL));

        if (doOptionCheck)
        {
            for (auto& pr : varmap)
                mapOptions.insert({ pr.first, true });

            // Sort entries based on name
            std::vector<std::pair<std::string, bool>> vOptionCheck;
            for (auto& pr : mapOptions)
                vOptionCheck.push_back(pr);

            auto iter = std::stable_partition(vOptionCheck.begin(), vOptionCheck.end(), [&](auto& pr) { return pr.second; });
            std::cout << "For device: \"" << mysource.get_source_info().get_product_name() << "\", the following device-dependent option list was generated : \n\n";
            std::cout << "Supported option(s):\n";
            std::for_each(vOptionCheck.begin(), iter, [&](auto& pr) { std::cout << "--" << pr.first << "\n"; });
            std::cout << "\nUnsupported option(s):\n";
            std::for_each(iter, vOptionCheck.end(), [&](auto& pr) { std::cout << "--" << pr.first << "\n"; });
            s_options.set_return_code(RETURN_OK);
            return false;
        }
    }
    return true;
}

bool set_device_options(twain_source& mysource, const po::variables_map& varmap)
{
    // Give a rundown of what is supported if --verbose or --optioncheck is specified
    bool checkReturn = check_device_options(mysource, varmap, true);
    if (!checkReturn)
        return false; // get out if --optioncheck was done

    // get the general acquire characteristics and set them
    auto& ac = mysource.get_acquire_characteristics();

    auto iter = s_options.m_FileTypeMap.find(s_options.m_filetype);
    auto iterMode2 = s_options.m_MapMode2Map.find(s_options.m_filetype);

    // set the file type, name
    bool type1 = false;
    bool type2 = false;
    if ((type1 = (iter != s_options.m_FileTypeMap.end())) || 
        (type2 = (iterMode2 != s_options.m_MapMode2Map.end())))
    {
        if (varmap["filename"].defaulted())
            s_options.m_filename = default_name + "." + resolve_extension(s_options.m_filetype);

        // must set these
        auto& fOptions = ac.get_file_transfer_options();
        if (type1)
        {
            auto multipage_type = file_type_info::get_multipage_type(iter->second);
            if (s_options.m_bMultiPage)
                iter->second = multipage_type;
            fOptions.set_type(iter->second);
            ac.get_general_options().set_transfer_type(s_options.m_nTransferMode == 0 ? transfer_type::file_using_native : transfer_type::file_using_buffered);
        }
        else
        {
            fOptions.set_type(iterMode2->second.first);
            ac.get_general_options().set_transfer_type(transfer_type::file_using_source);
        }

        // set the file save mode for multiple pages
        ac.get_file_transfer_options().
            get_multipage_save_options().
            set_save_mode(s_options.m_bMultiPage2?multipage_save_mode::save_uiclose : multipage_save_mode::save_default).
            set_save_incomplete(s_options.m_bSaveOnCancel);

        // set options, regardless if they appear on the command-line or not
        ac.get_file_transfer_options().
            enable_autocreate_directory(s_options.m_bCreateDir).
            set_multi_page(s_options.m_bMultiPage).
            set_name(s_options.m_filename);

        // set the max page count and the total number of acquisitions to attempt
        ac.get_general_options().
            set_max_page_count(s_options.m_NumPages).
            set_max_acquisitions(s_options.m_bUIPerm ? DTWAIN_MAXACQUIRE : 1);

        // Set the feeder and duplex options
        ac.get_paperhandling_options().
            enable_feeder(s_options.m_bUseADF).
            set_feedermode(s_options.m_bUseADFOrFlatbed?feedermode_value::feeder_flatbed:feedermode_value::feeder).
            set_feederwait(s_options.m_bNoUIWait?s_options.m_NoUIWaitTime:0).
            enable_duplex(s_options.m_bUseDuplex);

        // Turn on/of the user interface
        ac.get_userinterface_options().
            show(!s_options.m_bNoUI).
            show_indicators(s_options.m_bShowIndicator).
            show_onlyui(s_options.m_bShowUIOnly);

        // turn on/off halftoning, negation, threshold, pixel type
        ac.get_imagetype_options().
            set_halftone(s_options.m_strHalftone).
            set_pixeltype(s_options.m_ColorTypeMap[s_options.m_color]).
            enable_negate(s_options.m_bNegateImage).
            set_bitdepth(s_options.m_bitsPerPixel).
            set_jpegquality(s_options.m_nJpegQuality).
            set_threshold(s_options.m_dThreshold);

        // brightness, contrast, rotation, etc.
        ac.get_imageparameter_options().
            enable_autobright(s_options.m_bAutobrightMode).
            set_brightness(s_options.m_brightness).
            set_contrast(s_options.m_dContrast).
            set_orientation(s_options.m_OrientationTypeMap[s_options.m_Orientation]).
            set_rotation(s_options.m_dRotation).
            set_shadow(s_options.m_dShadow).
            set_highlight(s_options.m_dHighlight);

        // deskew and rotation
        ac.get_autoadjust_options().
            enable_deskew(s_options.m_bDeskew).
            enable_rotate(s_options.m_bAutoRotateMode);

        // film scanning, units
        ac.get_deviceparams_options().
            set_overscan(s_options.m_bOverscanMode).
            set_lightpath(s_options.m_bUseTransparencyUnit ? lightpath_value::transmissive : lightpath_value::reflective).
            set_units(s_options.m_MeasureUnitMap[s_options.m_strUnitOfMeasure]);

        ac.get_color_options().
            set_gamma(s_options.m_dGamma);

        ac.get_resolution_options().
            set_resolution(s_options.m_dResolution, s_options.m_dResolution);

        ac.get_pages_options().
            set_supportedsize(s_options.m_PageSizeMap[s_options.m_strPaperSize]);

        // Imprinter options
        auto& imprinter_options = ac.get_imprinter_options();
        if (!s_options.m_strImprinter.empty())
        {
            if (!varmap["imprinter"].defaulted())
                imprinter_options.set_printer({ static_cast<TW_UINT16>(s_options.m_nPrinter) });
            imprinter_options.set_string({ s_options.m_strImprinter });
            imprinter_options.enable(true);
        }

        ac.get_jobcontrol_options().
            set_option(s_options.m_JobControlMap[s_options.m_nJobControl]);

        s_options.m_nOverwriteWidth = NumDigits(s_options.m_nOverwriteMax);

        auto& file_rules = ac.get_file_transfer_options().get_filename_increment_options();
        file_rules.enable(s_options.m_bUseFileInc).
                set_increment(s_options.m_FileIncrement).
                use_reset_count(false);

        // blank page handling
        set_blank_page_options(mysource, varmap);

        // area of interest handling
        bool areaOk = set_areaofinterest_options(mysource, varmap);
        if (!areaOk)
            return false;

        // set scale options
        set_scale_options(mysource, varmap);

        // set pdf options
        set_pdf_options(mysource, varmap);
    }
    else
        return false;
    return true;
}

class STFCallback : public twain_callback
{
    scanner_options* m_pScannerOpts;

public:
    STFCallback(scanner_options *mSS) : twain_callback(), m_pScannerOpts(mSS)
    {}

    int uiopenfailure(twain_source& source) override
    {
        m_pScannerOpts->set_return_code(RETURN_TWAIN_UIOPEN_ERROR);
        return 1;
    }

    int acquiredone(twain_source& source) override
    {
        m_pScannerOpts->m_nOverwriteCount = 1;
        return 1;
    }
    
    int transferready(twain_source& source) override
    {
        if (m_pScannerOpts->m_nOverwriteMode == OVERWRITE_EXIT)
        {
            if (boost::filesystem::exists(m_pScannerOpts->m_filename))
            {
                m_pScannerOpts->set_return_code(RETURN_FILESAVE_FILEEXISTS);
                return 0;
            }
        }
        return 1;
    }

    int filenamechanging(twain_source& source) override
    {
        auto newName = m_pScannerOpts->m_filename;
        switch (m_pScannerOpts->m_nOverwriteMode)
        {
            case OVERWRITE_CREATENEW:
            {
                bool bFound = false;
                while (m_pScannerOpts->m_nOverwriteCount < m_pScannerOpts->m_nOverwriteMax)
                {
                    if (boost::filesystem::exists(newName))
                    {
                        newName = ::GetNewFileName(m_pScannerOpts->m_filename,
                            m_pScannerOpts->m_nOverwriteCount,
                            m_pScannerOpts->m_nOverwriteWidth);
                        ++m_pScannerOpts->m_nOverwriteCount;
                    }
                    else
                    {
                        bFound = true;
                        break;
                    }
                }
                if (bFound)
                    ::DTWAIN_SetSaveFileNameA(g_source->get_source(), newName.c_str());
            }
            break;
        }
        return 1;
    }
};

using namespace dynarithmic::twain;

struct twain_derived_logger : public twain_logger
{
    public:
        enum logger_destination
        {
            todebug,
            tofile,
            toconsole
        };
        
    private:
        logger_destination m_destination;
        std::string m_filename;
        std::unique_ptr<std::ofstream> m_file;

    public:
        twain_derived_logger() = default;
        twain_derived_logger& set_destination(logger_destination destination)
        {
            m_destination = destination;
            return *this;
        }

        twain_derived_logger& set_filename(std::string filename)
        {
            m_filename = filename;
            return *this;
        }

        bool enable()
        {
            if (m_destination == logger_destination::tofile)
                m_file = std::make_unique<std::ofstream>(m_filename);
            twain_logger::enable();
            return true;
        }

        virtual void log(const char* msg) override
        {
            switch (m_destination)
            {
                case logger_destination::todebug:
                {
                    std::string msgTotal = msg;
                    msgTotal.push_back('\n');
                    OutputDebugStringA(msgTotal.c_str());
                }
                break;
                case logger_destination::tofile:
                    *m_file << msg << "\n";
                break;
                case logger_destination::toconsole:
                    std::cout << msg << "\n";
                break;
            }
        }
};


int start_acquisitions(const po::variables_map& varmap) 
{
    if (s_options.m_bNoConsole)
        ShowWindow(GetConsoleWindow(), SW_HIDE);
    auto defaultIter = varmap.find("version");
    if (!defaultIter->second.defaulted())
    {
        std::cout << TWAINSAVE_VERINFO_ORIGINALFILENAME << " " << TWAINSAVE_FULL_VERSION << "\n";
        s_options.set_return_code(RETURN_OK);
        return RETURN_OK;
    }

    defaultIter = varmap.find("help");
    if (!defaultIter->second.defaulted())
    {
        std::cout << *desc2;
        s_options.set_return_code(RETURN_OK);
        return RETURN_OK;
    }
    
    defaultIter = varmap.find("details");
    if (!defaultIter->second.defaulted())
    {
        auto s = generate_details();
        if (s_options.m_bNoConsole)
        {
            DWORD d;
            WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), s.c_str(), static_cast<DWORD>(s.size()), &d, nullptr);
        }
        else
            std::cout << generate_details();
        s_options.set_return_code(RETURN_OK);
        return RETURN_OK;
    }

    // first start the TWAIN session
    twain_session ts(startup_mode::none);
    auto iter = varmap.find("tempdir");
    if (iter != varmap.end())
        ts.set_temporary_directory(boost::any_cast<std::string>(iter->second.value()));
    iter = varmap.find("dsmsearchorder");
    if (iter != varmap.end())
    {
        int so = boost::any_cast<int>(iter->second.value());
        ts.set_dsm_search_order(so);
    }
    iter = varmap.find("diagnose");
    if (!iter->second.defaulted())
    {
        bool logging_enabled = (iter != varmap.end());
        if (iter != varmap.end())
        {
            // create a logger and set the twain session to use the logger
            auto& logdetails = ts.register_logger<twain_derived_logger>();
            logdetails.set_verbosity(static_cast<logger_verbosity>(s_options.m_nDiagnose));
            logdetails.set_filename("stddiag.log");
            logdetails.set_destination(twain_derived_logger::logger_destination::tofile);
            if (varmap.find("diagnoselog") != varmap.end())
            {
                if (s_options.m_DiagnoseLog == "*")
                    logdetails.set_destination(twain_derived_logger::logger_destination::todebug);
                else
                    logdetails.set_filename(s_options.m_DiagnoseLog);
            }
            else
            {
                logdetails.set_destination(twain_derived_logger::logger_destination::tofile);
                logdetails.set_filename("stddiag.log");
            }
            logdetails.enable();
        }
    }

    // See if the user wants to use TWAIN DSM2 (32-bit version)
    if (s_options.m_bUseDSM2)
        ts.set_dsm(dsm_type::version2_dsm);

    // Set the application information for the session
    twain_session::twain_app_info appInfo;
    appInfo.set_product_name(TWAINSAVE_DEFAULT_TITLE).set_version_info(TWAINSAVE_FULL_VERSION);
    ts.set_app_info(appInfo);
        
    // Start the TWAIN session
    ts.start();

    if (ts)
    {
        // Load the language for the TWAIN dialog, diagnose logs, etc.
        if (!varmap["language"].defaulted())
        {
            if (s_options.m_DialogConfig.m_language == "default")
                ts.set_language_resource(s_options.m_strLanguage);
            else
                ts.set_language_resource(s_options.m_DialogConfig.m_language);
        }

        if (!s_options.m_strSelectName.empty())
            g_source = std::make_unique<twain_source>(ts.select_source(select_byname(s_options.m_strSelectName), false));
        else
        if (s_options.m_bSelectDefault)
            g_source = std::make_unique<twain_source>(ts.select_source(select_default(), false));
        else
        {
            // Get a twain dialog to customize
            twain_select_dialog twain_dialog;

            // Customize the dialog
            std::vector<int> vFlags;
            if (s_options.m_DialogConfig.m_position == std::make_pair((std::numeric_limits<int>::min)(), (std::numeric_limits<int>::min)()))
                vFlags.push_back(twain_select_dialog::showcenterscreen);
            else
                twain_dialog.set_position(s_options.m_DialogConfig.m_position);

            if (s_options.m_DialogConfig.m_horizscroll)
                vFlags.push_back(twain_select_dialog::horzscroll);

            if (s_options.m_DialogConfig.m_topmost)
                vFlags.push_back(twain_select_dialog::topmostwindow);

            if (s_options.m_DialogConfig.m_sortedNames)
                vFlags.push_back(twain_select_dialog::sortnames);

            twain_dialog.
                set_parent_window(nullptr).
                set_title(s_options.m_DialogConfig.m_strTwainTitle).
                set_flags(vFlags);

            // Show the TWAIN Select Source dialog
            g_source = std::make_unique<twain_source>(ts.select_source(select_usedialog(twain_dialog), false));
        }
        if (!g_source->is_selected())
        {
            s_options.set_return_code(RETURN_TWAIN_SOURCE_CANCEL);
            g_source.reset();
            ts.stop();
            return RETURN_TWAIN_SOURCE_CANCEL;
        }
        else
            g_source->open();
        if (!g_source->is_open())
        {
            s_options.set_return_code(RETURN_TWAIN_SOURCE_ERROR);
            g_source.reset();
            ts.stop();
            return RETURN_TWAIN_SOURCE_ERROR;
        }
    }
    else
    {
        s_options.set_return_code(RETURN_TWAIN_INIT_ERROR);
        return RETURN_TWAIN_INIT_ERROR;
    }

    if (g_source->is_open())
    {
        // check for pixel types
        auto vPixelTypes = g_source->get_capability_interface().get_pixeltype();
        std::array<ICAP_PIXELTYPE_::value_type, 6> supported_types = { DTWAIN_PT_BW, DTWAIN_PT_GRAY, DTWAIN_PT_RGB, DTWAIN_PT_PALETTE, DTWAIN_PT_CMY, DTWAIN_PT_CMYK };
        bool bfound = false;
        for (size_t i = 0; i < supported_types.size(); ++i)
        {
            if (std::find(vPixelTypes.begin(), vPixelTypes.end(), supported_types[i]) != vPixelTypes.end())
            {
                bfound = true;
                break;
            }
        }
        if (!bfound)
        {
            s_options.set_return_code(RETURN_COLORSPACE_NOT_SUPPORTED);
            g_source.reset();
            return RETURN_COLORSPACE_NOT_SUPPORTED;
        }

        // Set all of the options specified by the user
        if (set_device_options(*g_source, varmap))
        {
            ts.register_callback(*g_source, STFCallback(&s_options));

            // Start the acquisition
            auto acq_return = g_source->acquire();

            // Get the return status of the acquisition
            if (acq_return.first == dynarithmic::twain::twain_source::acquire_timeout)
                s_options.set_return_code(RETURN_TIMEOUT_REACHED);
            else
            if (acq_return.first == dynarithmic::twain::twain_source::acquire_canceled || acq_return.first == dynarithmic::twain::twain_source::acquire_ok)
                s_options.set_return_code(RETURN_OK);
            else
                s_options.set_return_code(RETURN_FILESAVE_ERROR);
            g_source.reset();
        }
        else
        {
            g_source.reset();
            // stop the twain session
            ts.stop();
        }
    }
    return 0;
}


std::string GetTwainSaveExecutionPath()
{
    const auto symlocation = boost::dll::symbol_location(TWAINSAVE_VERINFO_ORIGINALFILENAME);
    return symlocation.parent_path().string();
}

void LoadCustomResourcesFromIni()
{
    // Load the resources
    CSimpleIniA customProfile;
    auto s = GetTwainSaveExecutionPath();
    if (s.back() != '\\')
        s.push_back('\\');
    s += TWAINSAVE_INI_FILE;
    auto err = customProfile.LoadFile(s.c_str());
    if (err != SI_OK)
        return;
    s_options.m_DialogConfig.m_strTwainTitle = customProfile.GetValue("Twain Dialog", "title", TWAINSAVE_DEFAULT_TITLE);
    if (s_options.m_DialogConfig.m_strTwainTitle.empty())
        s_options.m_DialogConfig.m_strTwainTitle = TWAINSAVE_DEFAULT_TITLE;
    auto sSorted = customProfile.GetBoolValue("Twain Dialog", "sortednames", true);
    s_options.m_DialogConfig.m_sortedNames = sSorted;
    std::string position = customProfile.GetValue("Twain Dialog", "position", "center");
    if (position != "center")
    {
        int xpos = 0, ypos = 0;
        std::istringstream strm(position);
        strm >> xpos >> ypos;
        s_options.m_DialogConfig.m_position = { xpos,ypos };
    }

    auto sHorzScroll = customProfile.GetBoolValue("Twain Dialog", "horizscroll", true);
    s_options.m_DialogConfig.m_horizscroll = sHorzScroll;

    auto sTopmost = customProfile.GetBoolValue("Twain Dialog", "topmost", true);
    s_options.m_DialogConfig.m_topmost = sTopmost;

    auto sLanguage = customProfile.GetValue("Twain Dialog", "language", "default");
    s_options.m_DialogConfig.m_language = sLanguage;
}


class CommandLine
{
    std::vector<char *> argV;
    std::vector<std::string> argv_str;

    public:
        CommandLine(std::istream& in);
        
        std::vector<char *>& get_arguments()
        {
            argV.clear();
            for (size_t i = 0; i < argv_str.size(); ++i)
                argV.push_back(const_cast<char *>(argv_str[i].c_str()));
            return argV;
        }
};

CommandLine::CommandLine(std::istream& in)
{
    std::string cmd;
    argv_str.push_back(TWAINSAVE_VERINFO_ORIGINALFILENAME);
    while (std::getline(in, cmd))
    {
        std::string arg;
        std::istringstream iss(cmd);
        while (iss >> arg)
            argv_str.push_back(arg);
    }
}

parse_return_type parse_config_options(const std::string& filename)
{
    if (!boost::filesystem::exists(filename))
    {
        s_options.set_return_code(RETURN_COMMANDFILE_NOT_FOUND);
        return{ false, {} };
    }
    std::ifstream ifs(filename);
    if (!ifs)
    {
        s_options.set_return_code(RETURN_COMMANDFILE_OPEN_ERROR);
        return{ false,{} };
    }
    CommandLine cmdLine(ifs);
    auto args = cmdLine.get_arguments();
    return parse_options(static_cast<int>(args.size()), args.data());
}

int main(int argc, char *argv[])
{
    LoadCustomResourcesFromIni();
    auto retval = parse_options(argc, argv);

    if (retval.first)
    {
        if (!s_options.m_strConfigFile.empty())
            retval = parse_config_options(s_options.m_strConfigFile);
        if ( retval.first )
            start_acquisitions(retval.second);
    }
    if (s_options.m_bNoConsole && !s_options.m_bNoPause)
    {
        // display a pause message
        std::string s = "\nPress any key to continue...";
        DWORD d;
        WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), s.c_str(), static_cast<DWORD>(s.size()), &d, nullptr);
        char buffer[10];
        ReadConsoleA(GetStdHandle(STD_INPUT_HANDLE), buffer, 1, &d, NULL);
    }
    return s_options.get_return_code();
}

