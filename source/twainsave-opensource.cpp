/*
This file is part of the Twainsave-opensource version
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
#include "dtwain_cpp_interface.hpp"
#include <string>
#include <iostream>
#include <utility>
#include <iomanip>
#include <numeric>
#include <memory>
#include <sstream>
#include <type_traits>
#include <unordered_map>
#include "twainsave_verinfo.h"

template <typename E>
constexpr auto to_underlying(E e) noexcept
{
	return static_cast<std::underlying_type_t<E>>(e);
}

#define INIT_TYPE(x, thetype, y) {#x, dynarithmic::twain::##thetype::##y}
#define INIT_TYPE_2(x, thetype, y) {x, dynarithmic::twain::##thetype::##y}
#define OVERWRITE_ALWAYS	1
#define OVERWRITE_EXIT		2
#define OVERWRITE_CREATENEW 3
#define OVERWRITE_PROMPT	4
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
#define RETURN_FILESAVE_FILEEXISTS			12
#define RETURN_MODE2FILETYPE_NOT_SUPPORTED 13
#define RETURN_UIONLY_SUPPORT_ERROR		14	
#define RETURN_COMMANDFILE_NOT_FOUND    15
#define RETURN_COMMANDFILE_OPEN_ERROR   16
std::unique_ptr<dynarithmic::twain::twain_source> g_source;

struct scanner_options
{
	std::string m_filetype;
	std::string m_filename;
	std::string m_area;
	bool m_bUseADF;
	bool m_bUseADFOrFlatbed;
	bool m_bAutobrightMode;
	double m_brightness;
	double m_contrast;
	double m_gamma;
	int m_bitsPerPixel;
	int m_color;
	bool m_bUseDuplex;
	bool m_bDeskew;
	bool m_bAutoRotateMode;
	std::string m_strHalftone;
	int m_Orientation;
	bool m_bNoUI;
	double m_dHighlight;
	double m_threshold;
	double m_dRotation;
	bool m_bShowIndicator;
	bool m_bShowUIOnly;
	double m_dShadow;
	bool m_bNoBlankPages;
	double m_resolution;
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
	int m_NumPages;
	std::string m_strPaperSize;
	bool m_bNoUIWait;
	int m_NoUIWaitTime;
	bool m_bSaveOnCancel;
	bool m_bMultiPage;
	bool m_bMultiPage2;
	bool m_bUseDSM2;
	std::string m_strTempDirectory;
	int m_DSMSearchOrder;
	std::unordered_map<std::string, dynarithmic::twain::file_type> m_FileTypeMap;
	std::unordered_map<int, dynarithmic::twain::color_type> m_ColorTypeMap;
	std::unordered_map<int, dynarithmic::twain::image_orientation> m_OrientationTypeMap;
	std::unordered_map<std::string, dynarithmic::twain::paper_size> m_PageSizeMap;
	std::array<long, 4> m_errorLevels;
	bool m_bUseFileInc;
	int m_FileIncrement;
	int m_nTransferMode;
	int m_nDiagnose;
	std::string m_DiagnoseLog;
	bool m_bUseTransparencyUnit;
	std::string m_strUnitOfMeasure;
	int m_nJobControl;
	int m_nOverwriteCount;
	int m_nOverwriteWidth;
	std::unordered_map<std::string, dynarithmic::twain::measure_unit> m_MeasureUnitMap;
	std::unordered_map<int, dynarithmic::twain::jobcontrol_type> m_JobControlMap;
	std::unordered_map<std::string, dynarithmic::twain::pdf_permission> m_PDFEncryptMap;
	std::unordered_map<std::string, dynarithmic::twain::pdf_permission> m_PDFEncryptMapOff;
	std::unordered_map<std::string, std::pair<dynarithmic::twain::file_type, dynarithmic::twain::twain_compression_type>> m_MapMode2Map;
	std::unordered_map<std::string, TW_UINT16> m_OptionToCapMap;
	int twainsave_return_value;
	std::string m_strConfigFile;
	bool m_bUseVerbose;

	scanner_options() : twainsave_return_value(RETURN_OK),
							m_nOverwriteCount(1),
							m_nOverwriteWidth(1),
							m_FileTypeMap{
							INIT_TYPE(bmp, file_type, bmp),
							INIT_TYPE(gif, file_type,gif),
							INIT_TYPE(pcx, file_type,pcx),
							INIT_TYPE(dcx, file_type,dcx),
							INIT_TYPE(pdf, file_type,pdf),
							INIT_TYPE(ico, file_type,windowsicon),
							INIT_TYPE(png, file_type,png),
							INIT_TYPE(tga, file_type,targa),
							INIT_TYPE(psd, file_type,psd),
							INIT_TYPE(emf, file_type,enhancedmetafile),
							INIT_TYPE(wbmp, file_type,wirelessbmp),
							INIT_TYPE(wmf, file_type,windowsmetafile),
							INIT_TYPE(jpeg, file_type,jpeg),
							INIT_TYPE(jp2, file_type,jpeg2k),
							INIT_TYPE(tif1, file_type,tiffnocompress),
							INIT_TYPE(tif2, file_type,tiffpackbits),
							INIT_TYPE(tif3, file_type, tiffgroup3),
							INIT_TYPE(tif4, file_type, tiffgroup4),
							INIT_TYPE(tif5, file_type, tiffjpeg),
							INIT_TYPE(tif6, file_type, tiffdeflate),
							INIT_TYPE(tif7, file_type, tifflzw),
							INIT_TYPE(ps1, file_type, postscript1),
							INIT_TYPE(ps2, file_type, postscript2),
							INIT_TYPE(webp, file_type, googlewebp) },

						m_ColorTypeMap{
							INIT_TYPE_2(0, color_type, bw),
							INIT_TYPE_2(1, color_type, gray),
							INIT_TYPE_2(2, color_type, rgb) },

							m_PageSizeMap{
							INIT_TYPE(custom, paper_size, CUSTOM),
							INIT_TYPE(variable, paper_size, VARIABLE),
							INIT_TYPE(letter, paper_size, USLETTER),
							INIT_TYPE(none, paper_size, NONE),
							INIT_TYPE(legal, paper_size, USLEGAL),
							INIT_TYPE(ledger, paper_size, USLEDGER),
							INIT_TYPE(executive, paper_size, USEXECUTIVE),
							INIT_TYPE(statement, paper_size, USSTATEMENT),
							INIT_TYPE(businesscard, paper_size, BUSINESSCARD),
							INIT_TYPE(4A0, paper_size, FOURA0),
							INIT_TYPE(2A0, paper_size, TWOA0),
							INIT_TYPE(JISB0, paper_size, JISB0),
							INIT_TYPE(JISB1, paper_size, JISB1),
							INIT_TYPE(JISB2, paper_size, JISB2),
							INIT_TYPE(JISB3, paper_size, JISB3),
							INIT_TYPE(JISB4, paper_size, JISB4),
							INIT_TYPE(JISB5, paper_size, JISB5),
							INIT_TYPE(JISB6, paper_size, JISB6),
							INIT_TYPE(JISB7, paper_size, JISB7),
							INIT_TYPE(JISB8, paper_size, JISB8),
							INIT_TYPE(JISB9, paper_size, JISB9),
							INIT_TYPE(JISB10, paper_size, JISB10),
							INIT_TYPE(A0, paper_size, A0),
							INIT_TYPE(A1, paper_size, A1),
							INIT_TYPE(A2, paper_size, A2),
							INIT_TYPE(A3, paper_size, A3),
							INIT_TYPE(A4, paper_size, A4),
							INIT_TYPE(A5, paper_size, A5),
							INIT_TYPE(A6, paper_size, A6),
							INIT_TYPE(A7, paper_size, A7),
							INIT_TYPE(A8, paper_size, A8),
							INIT_TYPE(A9, paper_size, A9),
							INIT_TYPE(A10, paper_size, A10),
							INIT_TYPE(B0, paper_size, ISOB0),
							INIT_TYPE(B1, paper_size, ISOB1),
							INIT_TYPE(B2, paper_size, ISOB2),
							INIT_TYPE(B3, paper_size, ISOB3),
							INIT_TYPE(B4, paper_size, ISOB4),
							INIT_TYPE(B5, paper_size, ISOB5),
							INIT_TYPE(B6, paper_size, ISOB6),
							INIT_TYPE(B7, paper_size, ISOB7),
							INIT_TYPE(B8, paper_size, ISOB8),
							INIT_TYPE(B9, paper_size, ISOB9),
							INIT_TYPE(B10, paper_size, ISOB10),
							INIT_TYPE(C0, paper_size, C0),
							INIT_TYPE(C1, paper_size, C1),
							INIT_TYPE(C2, paper_size, C2),
							INIT_TYPE(C3, paper_size, C3),
							INIT_TYPE(C4, paper_size, C4),
							INIT_TYPE(C5, paper_size, C5),
							INIT_TYPE(C6, paper_size, C6),
							INIT_TYPE(C7, paper_size, C7),
							INIT_TYPE(C8, paper_size, C8),
							INIT_TYPE(C9, paper_size, C9),
							INIT_TYPE(C10, paper_size, C10) },

						m_MeasureUnitMap{
							INIT_TYPE(inch, measure_unit, inch),
							INIT_TYPE(cm, measure_unit, cm),
							INIT_TYPE(pica, measure_unit, pica),
							INIT_TYPE(point, measure_unit, point),
							INIT_TYPE(twip, measure_unit, twip),
							INIT_TYPE(pixel, measure_unit, pixel),
							INIT_TYPE(mm, measure_unit, mm) },

						m_OrientationTypeMap{
							INIT_TYPE_2(0, image_orientation, orient_0),
							INIT_TYPE_2(90, image_orientation, orient_90),
							INIT_TYPE_2(180, image_orientation, orient_180),
							INIT_TYPE_2(270, image_orientation, orient_270) },

						m_JobControlMap{
							INIT_TYPE_2(0, jobcontrol_type, none),
							INIT_TYPE_2(1, jobcontrol_type, include_separator),
							INIT_TYPE_2(2, jobcontrol_type, exclude_separator) },

						m_PDFEncryptMap{
							INIT_TYPE(modify, pdf_permission, modify),
							INIT_TYPE(copy, pdf_permission, copy),
							INIT_TYPE(modannot, pdf_permission, modifyannotations),
							INIT_TYPE(fillin, pdf_permission, fillin),
							INIT_TYPE(extract, pdf_permission, extract),
							INIT_TYPE(assembly, pdf_permission, assembly),
							INIT_TYPE(degradeprint, pdf_permission, degradedprint),
							INIT_TYPE(print, pdf_permission, print),
							INIT_TYPE(all, pdf_permission, all)
						},

						m_PDFEncryptMapOff{
							INIT_TYPE(nomodify, pdf_permission, modify),
							INIT_TYPE(nocopy, pdf_permission, copy),
							INIT_TYPE(nomodannot, pdf_permission, modifyannotations),
							INIT_TYPE(nofillin, pdf_permission, fillin),
							INIT_TYPE(noextract, pdf_permission, extract),
							INIT_TYPE(noassembly, pdf_permission, assembly),
							INIT_TYPE(nodegradeprint, pdf_permission, degradedprint),
							INIT_TYPE(noprint, pdf_permission, print),
							INIT_TYPE(none, pdf_permission, all)
						},

		m_MapMode2Map{ {"bmp1_mode2",{dynarithmic::twain::file_type::bmp_source_mode, dynarithmic::twain::twain_compression_type::none}},
				   {"bmp2_mode2",{ dynarithmic::twain::file_type::bmp_source_mode, dynarithmic::twain::twain_compression_type::rle4}},
				   {"bmp3_mode2",{ dynarithmic::twain::file_type::bmp_source_mode, dynarithmic::twain::twain_compression_type::rle8}},
				   {"bmp4_mode2",{ dynarithmic::twain::file_type::bmp_source_mode, dynarithmic::twain::twain_compression_type::bitfields}},
				   {"dejavu_mode2",{ dynarithmic::twain::file_type::dejavu_source_mode, dynarithmic::twain::twain_compression_type::none}},
				   { "exif_mode2",{ dynarithmic::twain::file_type::exif_source_mode, dynarithmic::twain::twain_compression_type::none }},
				   { "fpx_mode2",{ dynarithmic::twain::file_type::fpx_source_mode, dynarithmic::twain::twain_compression_type::none }},
				   { "jfif_mode2",{ dynarithmic::twain::file_type::jfif_source_mode, dynarithmic::twain::twain_compression_type::jpeg }},
				   { "jpeg_mode2",{ dynarithmic::twain::file_type::jpeg, dynarithmic::twain::twain_compression_type::none }},
				   { "jp2_mode2",{ dynarithmic::twain::file_type::jp2_source_mode, dynarithmic::twain::twain_compression_type::jpeg2k }},
				   { "jpx_mode2",{ dynarithmic::twain::file_type::jpx_source_mode, dynarithmic::twain::twain_compression_type::none }},
				   { "pdf_mode2",{ dynarithmic::twain::file_type::pdf_source_mode, dynarithmic::twain::twain_compression_type::none } },
				   { "pdfa1_mode2",{ dynarithmic::twain::file_type::pdfa_source_mode, dynarithmic::twain::twain_compression_type::none } },
				   { "pdfa2_mode2",{ dynarithmic::twain::file_type::pdfa2_source_mode, dynarithmic::twain::twain_compression_type::none } },
				   { "pict_mode2",{ dynarithmic::twain::file_type::pict_source_mode, dynarithmic::twain::twain_compression_type::none } },
				   { "png_mode2",{ dynarithmic::twain::file_type::png_source_mode, dynarithmic::twain::twain_compression_type::png } },
				   { "spiff1_mode2",{ dynarithmic::twain::file_type::spiff_source_mode, dynarithmic::twain::twain_compression_type::jpeg } },
				   { "spiff2_mode2",{ dynarithmic::twain::file_type::spiff_source_mode, dynarithmic::twain::twain_compression_type::jbig } },
				   { "tiff1_mode2",{ dynarithmic::twain::file_type::tiff_source_mode, dynarithmic::twain::twain_compression_type::none } },
				   { "tiff2_mode2",{ dynarithmic::twain::file_type::tiff_source_mode, dynarithmic::twain::twain_compression_type::group3_1d } },
				   { "tiff3_mode2",{ dynarithmic::twain::file_type::tiff_source_mode, dynarithmic::twain::twain_compression_type::group3_1deol } },
				   { "tiff4_mode2",{ dynarithmic::twain::file_type::tiff_source_mode, dynarithmic::twain::twain_compression_type::group3_2d } },
				   { "tiff5_mode2",{ dynarithmic::twain::file_type::tiff_source_mode, dynarithmic::twain::twain_compression_type::group4} },
				   { "tiff6_mode2",{ dynarithmic::twain::file_type::tiff_source_mode, dynarithmic::twain::twain_compression_type::jpeg } },
				   { "tiff7_mode2",{ dynarithmic::twain::file_type::tiff_source_mode, dynarithmic::twain::twain_compression_type::lzw } },
				   { "tiff8_mode2",{ dynarithmic::twain::file_type::tiff_source_mode, dynarithmic::twain::twain_compression_type::jbig } },
				   { "tiff9_mode2",{ dynarithmic::twain::file_type::tiff_source_mode, dynarithmic::twain::twain_compression_type::zip } },
				   { "xbm_mode2",{ dynarithmic::twain::file_type::xbm_source_mode, dynarithmic::twain::twain_compression_type::none} } },

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

po::options_description desc2;

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
	{
		auto name = p.filename().native();
		std::string s(name.begin(), name.end());
		elements.push_back(s);
	}
	return elements;
}

std::string GetNewFileName(const std::string& fullpath, int inc, int maxWidth)
{
	auto vString = SplitPath(boost::filesystem::path(fullpath));
	auto newName = vString.back();
	boost::filesystem::path theStem(newName);

	auto filename_only = theStem.stem().native();
	std::string file_part(filename_only.begin(), filename_only.end());

	auto ext_only = theStem.extension().native();
	std::string ext(ext_only.begin(), ext_only.end());
	std::ostringstream strm;
	strm << "/" << file_part << OVERWRITE_PREFIXSTRING << std::setw(maxWidth) << std::setfill('0') << inc;
	vString.back() = strm.str();
	auto retval = std::accumulate(vString.begin(), vString.end(), std::string());
	return retval += ext;
}

scanner_options s_options = {};
pdf_controls pdf_commands = {};
std::string default_name;
std::string descript_name;

using parse_return_type = std::pair<bool, po::variables_map>;

parse_return_type parse_options(int argc, char *argv[])
{
	boost::uuids::uuid uuid = boost::uuids::random_generator()();
	default_name = boost::uuids::to_string(uuid);
	descript_name = default_name;
	std::replace_if(descript_name.begin(), descript_name.end(), [&](char ch) { return ch != '-'; }, 'x');
	po::command_line_style::style_t style = po::command_line_style::style_t(po::command_line_style::unix_style);
	try
	{
		desc2.add_options()
			("area", po::value< std::string >(&s_options.m_area), "set acquisition area of image to acquire")
			("autobright", po::bool_switch(&s_options.m_bAutobrightMode)->default_value(false), "turn on autobright feature")
			("autofeed", po::bool_switch(&s_options.m_bUseADF)->default_value(false), "turn on automatic document feeder")
			("autofeedorflatbed", po::bool_switch(&s_options.m_bUseADFOrFlatbed)->default_value(false), "use feeder if not empty, else use flatbed")
			("autorotate", po::bool_switch(&s_options.m_bAutoRotateMode)->default_value(false), "Detect if document should be rotated.  Device must support autorotate")
			("bitsperpixel", po::value< int >(&s_options.m_bitsPerPixel), "Image bits-per-pixel.  Default is current device setting")
			("blankthreshold", po::value< double >(&s_options.m_dBlankThreshold)->default_value(98), "Percentage threshold to determine if page is blank")
			("brightness", po::value< double >(&s_options.m_brightness), "Brightness level (device must support brightness)")
			("color", po::value< int >(&s_options.m_color)->default_value(0), "Color. 0=B/W, 1=8-bit Grayscale, 2=24 bit RGB. Default is 0")
			("contrast", po::value< double >(&s_options.m_contrast), "Contrast level (device must support contrast)")
			("deskew", po::bool_switch(&s_options.m_bDeskew)->default_value(false), "Deskew image if skewed.  Device must support deskew")
			("diagnose", po::value< int >(&s_options.m_nDiagnose)->default_value(0), "Create diagnostic log.  Level values 1, 2, 3 or 4.")
			("diagnoselog", po::value< std::string >(&s_options.m_DiagnoseLog), "file name to store -diagnose messages")
			("dsmsearchorder", po::value< int >(&s_options.m_DSMSearchOrder)->default_value(0), "Directories TwainSave will search when locating TWAIN_32.DLL or TWAINDSM.DLL")
			("duplex", po::bool_switch(&s_options.m_bUseDuplex)->default_value(false), "turn on duplex unit")
			("filename", po::value< std::string >(&s_options.m_filename)->default_value(descript_name), "file name to save acquired image(s)")
			("filetype", po::value< std::string >(&s_options.m_filetype)->default_value("bmp"), "Image file type")
			("gamma", po::value< double >(&s_options.m_gamma), "Gamma level (device must support gamma levels)")
			("help", "Show help screen")
			("halftone", po::value< std::string >(&s_options.m_strHalftone)->default_value("none"), "Halftone effect to use when acquiring low resolution images")
			("highlight", po::value< double >(&s_options.m_dHighlight), "Highlight level (device must support highlight)")
			("imprinterstring", po::value< std::string >(&s_options.m_strImprinter), "Set imprinter string")
			("incvalue", po::value< int >(&s_options.m_FileIncrement)->default_value(1), "File name counter")
			("jobcontrol", po::value< int >(&s_options.m_nJobControl)->default_value(0), "0=none, 1=include job page, 2=exclude job page")
			("multipage", po::bool_switch(&s_options.m_bMultiPage)->default_value(false), "Save to multipage file")
			("multipage2", po::bool_switch(&s_options.m_bMultiPage2)->default_value(false), "Save to multipage file only after closing UI")
			("negate", po::bool_switch(&s_options.m_bNegateImage)->default_value(false), "Negates (reverses polarity) of acquired images")
			("noblankpages", po::bool_switch(&s_options.m_bNoBlankPages)->default_value(false), "Remove blank pages")
			("noui", po::bool_switch(&s_options.m_bNoUI)->default_value(false), "turn off device user interface")
			("nouiwait", po::bool_switch(&s_options.m_bNoUIWait)->default_value(false), "Do not display Source user interface and wait for feeder loaded before acquiring")
			("nouiwaittime", po::value< int >(&s_options.m_NoUIWaitTime)->default_value(120), "Time to wait (in seconds) for feeder loaded.")
			("numpages", po::value< int >(&s_options.m_NumPages)->default_value(0), "Number of pages to acquire.  Default is 0 (acquire all pages)")
			("orientation", po::value< int >(&s_options.m_Orientation)->default_value(0), "Clockwise orientation in degrees (0, 90, 180, 270)")
			("overscanmode", po::bool_switch(&s_options.m_bOverscanMode)->default_value(false), "Turn on overscan mode.  Device must support overscan")
			("overwritemax", po::value< int >(&s_options.m_nOverwriteMax)->default_value(9999), "Sets the maximum number of files created per acquisition for \"--overwritemode 3\"")
			("overwritemode", po::value< int >(&s_options.m_nOverwriteMode)->default_value(1), "Mode to use when file already exists.  Default is 1 (always overwrite existing file)")
			("papersize", po::value< std::string >(&s_options.m_strPaperSize)->default_value("letter"), "Paper size.  Default is \"letter\"")
			("pdfascii", po::bool_switch(&pdf_commands.m_bAscii)->default_value(false), "create ASCII compressed (text-based) PDF files")
			("pdf128", po::bool_switch(&pdf_commands.m_bStrong)->default_value(false), "use PDF 128-bit (strong) encryption")
			("pdf40", po::bool_switch(&pdf_commands.m_bWeak)->default_value(true), "use PDF 40-bit encryption")
			("pdfauthor", po::value< std::string >(&pdf_commands.m_strAuthor), "Sets the PDF Author field")
			("pdfcreator", po::value< std::string >(&pdf_commands.m_strCreator), "Sets the PDF Creator field")
			("pdfkeywords", po::value< std::string >(&pdf_commands.m_strKeywords), "Sets the PDF Keywords field")
			("pdfproducer", po::value< std::string >(&pdf_commands.m_strProducer), "Sets the PDF Producer field")
			("pdfsubject", po::value< std::string >(&pdf_commands.m_strSubject), "Sets the PDF Subject field")
			("pdftitle", po::value< std::string >(&pdf_commands.m_strTitle), "Sets the PDF Title field")
			("pdfencrypt", po::bool_switch(&pdf_commands.m_bEncrypt)->default_value(false), "Turn on PDF encryption")
			("pdfownerpass", po::value< std::string >(&pdf_commands.m_strOwnerPass), "Sets the PDF owner password")
			("pdfuserpass", po::value< std::string >(&pdf_commands.m_strUserPass), "Sets the PDF user password")
			("pdfrandowner", po::bool_switch(&pdf_commands.m_bRandomOwner)->default_value(false), "Use random PDF owner password.  Cannot be used with --pdfownerpass")
			("pdfranduser", po::bool_switch(&pdf_commands.m_bRandomUser)->default_value(false), "Use random PDF Userr password.  Cannot be used with --pdfuserpass")
			("pdfpermit", po::value< std::string >(&pdf_commands.m_strPermissions), "PDF permissions allowed for encrypted files")
			("pdfsize", po::value< std::string >(&pdf_commands.m_strPaperSize)->default_value("letter"), "PDF Paper size.  Default is \"letter\"")
			("pdfquality", po::value< int >(&pdf_commands.m_quality)->default_value(60), "set the JPEG quality factor for PDF files")
			("pdforient", po::value< std::string >(&pdf_commands.m_strOrient)->default_value("portrait"), "Sets orientation to portrait or landscape")
			("pdfscale", po::value< std::string >(&pdf_commands.m_strScale)->default_value("noscale"), "PDF page scaling")
			("resolution", po::value< double >(&s_options.m_resolution), "Image resolution in dots per unit (see --unit)")
			("rotation", po::value< double >(&s_options.m_dRotation), "Rotate page by the specified number of degrees (device must support rotation)")
			("saveoncancel", po::bool_switch(&s_options.m_bSaveOnCancel)->default_value(false), "Save image file even if acquisition canceled by user")
			("selectbydialog", po::bool_switch(&s_options.m_bSelectByDialog)->default_value(true), "When selecting device, show \"Select Source\" dialog (Default)")
			("selectbyname", po::value< std::string >(&s_options.m_strSelectName), "Select TWAIN device by specifying device product name")
			("selectdefault", po::bool_switch(&s_options.m_bSelectDefault)->default_value(false), "Select the default TWAIN device automatically")
			("shadow", po::value< double >(&s_options.m_dShadow), "Shadow level (device must support shadow levels)")
			("showindicator", po::bool_switch(&s_options.m_bShowIndicator)->default_value(false), "Show progress indicator when no user-interface is chosen (-noui)")
			("tempdir", po::value< std::string >(&s_options.m_strTempDirectory), "Temporary file directory")
			("threshold", po::value< double >(&s_options.m_threshold), "Threshold level (device must support threshold)")
			("transfermode", po::value< int >(&s_options.m_nTransferMode)->default_value(0), "Transfer mode. 0=Native, 1=Buffered")
			("transparency", po::bool_switch(&s_options.m_bUseTransparencyUnit)->default_value(false), "Use transparency unit")
			("uionly", po::bool_switch(&s_options.m_bShowUIOnly)->default_value(false), "Allow user interface to be shown without acquiring images")
			("uiperm", po::bool_switch(&s_options.m_bUIPerm)->default_value(false), "Leave UI open on successful acquisition")
			("unitofmeasure", po::value< std::string >(&s_options.m_strUnitOfMeasure)->default_value("inch"), "Unit of measure")
			("usedsm2", po::bool_switch(&s_options.m_bShowUIOnly)->default_value(false), "Use TWAINDSM.DLL if found as the data source manager.")
			("useinc", po::bool_switch(&s_options.m_bUseFileInc)->default_value(false), "Use file name increment")
			("verbose", po::bool_switch(&s_options.m_bUseVerbose)->default_value(false), "Turn on verbose mode")
			("version", "Display program version")
			("@", po::value< std::string >(&s_options.m_strConfigFile), "Configuration file");
		po::variables_map vm2;
		po::store(po::parse_command_line(argc, argv, desc2, style), vm2);
		po::notify(vm2);
		return{ true, vm2 };
	}
	catch (const boost::program_options::error_with_option_name& e)
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

template <typename Fn, typename T, typename CapType=T>
void set_characteristic(twain_source& theSource, const T& value, Fn fn, 
						const po::variables_map& varmap, const std::string& entry, 
						int capvalue = 0, CapType captype=T())
{
	auto iter = varmap.find(entry);
	if (iter != varmap.end())
	{
		acquire_characteristics& ac = theSource.get_acquire_characteristics();
		if (!iter->second.defaulted())
		{
			(ac.*fn)(value);
			if (capvalue && !varmap["verbose"].defaulted())
			{
				std::cout << "Checking if device supports " << entry << " ...\n";
				// test if the source supports what we're supposed to be setting later
				bool issupported = theSource.get_capability_interface().is_cap_supported(capvalue);
				std::cout << (issupported?"Success!  ":"Sorry :( ") << "The TWAIN device \"" << theSource.get_source_info().get_product_name() << "\" does" << (issupported ? " " : " not ")
					<< "support the \"--" << entry << "\" capability\n";
				if (issupported)
				{
					// now test if the device can actually use the value set
					std::vector<CapType> testArray;
					std::cout << "Testing if " << value << " can be used...\n";
					if (theSource.get_capability_interface().get_cap_values(testArray, capvalue, capability_interface::get()).first)
					{
						bool valuefound = std::find(testArray.begin(), testArray.end(), value) != testArray.end();
						std::cout << (valuefound ? "Success!  " : "Sorry :( ") << "The TWAIN device \"" << theSource.get_source_info().get_product_name() << "\" does" << (valuefound ? " " : " not ")
								<< " support the value " << value << " that you are using. capability\n";
					}
				}
			}
			std::cout << "\n";
		}
	}
}

bool set_caps(twain_source& mysource, const po::variables_map& varmap)
{
	// get the general acquire characteristics and set them
	auto& ac = mysource.get_acquire_characteristics();

	auto iter = s_options.m_FileTypeMap.find(s_options.m_filetype);
	auto iterMode2 = s_options.m_MapMode2Map.find(s_options.m_filetype);

	// set the file type
	bool type1 = false;
	bool type2 = false;
	if ((type1 = (iter != s_options.m_FileTypeMap.end())) || 
		(type2 = (iterMode2 != s_options.m_MapMode2Map.end())))
	{
		if (varmap["filename"].defaulted())
			s_options.m_filename = default_name + "." + s_options.m_filetype;

		// must set these
		if (type1)
			ac.set_file_type(iter->second);
		else
			ac.set_file_type(iterMode2->second.first).set_compression_type(iterMode2->second.second);

		// set the file save mode for multiple pages
		auto& save_mode = ac.get_multipage_save_info();
		save_mode.set_save_mode(s_options.m_bMultiPage2?multipage_save_mode::save_uiclose : multipage_save_mode::save_default);
		save_mode.set_save_incomplete(s_options.m_bSaveOnCancel);

		// set options, regardless if they appear on the command-line or not
		ac.set_multi_page(s_options.m_bMultiPage)
			.set_max_pages(s_options.m_NumPages)
			.set_show_ui(!s_options.m_bNoUI)
			.set_duplex_mode(s_options.m_bUseDuplex)
			.set_negate_images(s_options.m_bNegateImage)
			.set_uionly_mode(s_options.m_bShowUIOnly)
			.set_filename_pattern(s_options.m_filename)
			.set_max_acquisitions(s_options.m_bUIPerm ? DTWAIN_MAXACQUIRE : 1)
			.set_transfer_type(s_options.m_nTransferMode == 0 ? transfer_type::file_using_native : transfer_type::file_using_buffered);

		// only set these if specified on command line
		set_characteristic<decltype(&acquire_characteristics::set_autobright_mode), bool, long>(mysource, s_options.m_bAutobrightMode, &acquire_characteristics::set_autobright_mode, varmap, "autobright", ICAP_AUTOBRIGHT);
		set_characteristic<decltype(&acquire_characteristics::set_autodeskew_mode), bool, long>(mysource, s_options.m_bDeskew, &acquire_characteristics::set_autodeskew_mode, varmap, "deskew", ICAP_AUTOMATICDESKEW);
		set_characteristic<decltype(&acquire_characteristics::set_autorotate_mode), bool, long>(mysource, s_options.m_bAutoRotateMode, &acquire_characteristics::set_autorotate_mode, varmap, "autorotate", ICAP_AUTOMATICROTATE);
		set_characteristic(mysource, s_options.m_brightness, &acquire_characteristics::set_brightness, varmap, "brightness", ICAP_BRIGHTNESS);
		set_characteristic<decltype(&acquire_characteristics::set_use_filmscan_mode), bool, long>(mysource, s_options.m_bUseTransparencyUnit, &acquire_characteristics::set_use_filmscan_mode, varmap, "transparency", ICAP_LIGHTPATH);
		set_characteristic(mysource, s_options.m_contrast, &acquire_characteristics::set_contrast, varmap, "contrast", ICAP_CONTRAST);
		set_characteristic(mysource, s_options.m_dHighlight, &acquire_characteristics::set_highlight, varmap, "highlight", ICAP_HIGHLIGHT);
		set_characteristic(mysource, s_options.m_threshold, &acquire_characteristics::set_threshold, varmap, "threshold", ICAP_THRESHOLD);
		set_characteristic(mysource, s_options.m_gamma, &acquire_characteristics::set_gamma, varmap, "gamma", ICAP_GAMMA);
		set_characteristic(mysource, s_options.m_strHalftone, &acquire_characteristics::set_halftone, varmap, "halftone", ICAP_HALFTONES);
		set_characteristic(mysource, s_options.m_resolution, &acquire_characteristics::set_resolution, varmap, "resolution", ICAP_XRESOLUTION);
		set_characteristic(mysource, s_options.m_dRotation, &acquire_characteristics::set_rotation, varmap, "rotation", ICAP_ROTATION);
		set_characteristic(mysource, s_options.m_dShadow, &acquire_characteristics::set_shadow, varmap, "shadow", ICAP_SHADOW);
		set_characteristic<decltype(&acquire_characteristics::set_overscan_mode), bool, long>(mysource, s_options.m_bOverscanMode, &acquire_characteristics::set_overscan_mode, varmap, "overscan", ICAP_OVERSCAN);
		set_characteristic<decltype(&acquire_characteristics::set_use_indicators), bool, long>(mysource, s_options.m_bShowIndicator, &acquire_characteristics::set_use_indicators, varmap, "showindicator", CAP_INDICATORS);
		set_characteristic(mysource, static_cast<long>(s_options.m_ColorTypeMap[s_options.m_color]), &acquire_characteristics::set_color_type_i, varmap, "color", ICAP_PIXELTYPE);
		set_characteristic(mysource, static_cast<long>(s_options.m_MeasureUnitMap[s_options.m_strUnitOfMeasure]), &acquire_characteristics::set_measure_unit_i, varmap, "unitofmeasure", ICAP_UNITS);
		set_characteristic(mysource, static_cast<long>(s_options.m_PageSizeMap[s_options.m_strPaperSize]), &acquire_characteristics::set_paper_size_i, varmap, "papersize", ICAP_SUPPORTEDSIZES);
		set_characteristic(mysource, static_cast<long>(s_options.m_OrientationTypeMap[s_options.m_Orientation]), &acquire_characteristics::set_orientation_i, varmap, "orientation", ICAP_ORIENTATION);
		set_characteristic(mysource, s_options.m_strImprinter, &acquire_characteristics::set_imprinter_string, varmap, "imprinterstring", CAP_PRINTER);
		set_characteristic(mysource, static_cast<long>(s_options.m_JobControlMap[s_options.m_nJobControl]), &acquire_characteristics::set_jobcontrol_type_i, varmap, "jobcontrol", CAP_JOBCONTROL);
		set_characteristic(mysource, static_cast<long>(s_options.m_bNoUIWait), &acquire_characteristics::set_wait_for_feeder_loaded, varmap, "nouiwait", CAP_PAPERDETECTABLE);
		set_characteristic(mysource, static_cast<long>(s_options.m_NoUIWaitTime*1000), &acquire_characteristics::set_feeder_timeout, varmap, "nouiwaittime", CAP_PAPERDETECTABLE);

		if (!varmap["autofeedorflatbed"].defaulted())
			ac.set_wait_for_feeder_loaded(true).set_feeder_timeout(1).set_use_feeder_or_flatbed(true);

		s_options.m_nOverwriteWidth = NumDigits(s_options.m_nOverwriteMax);

		auto& file_rules = ac.get_filename_increment_rules();
		file_rules.set_enabled(s_options.m_bUseFileInc)
					.set_increment(s_options.m_FileIncrement)
					.set_reset_count(false);

		// ui wait time
		auto& blank_handler = ac.get_blank_image_handler();
		auto iter2 = varmap.find("noblankpages");
		if (iter2 != varmap.end())
		{
			blank_handler.enabled = true;
			blank_handler.discard_option = blank_image_discard_option::discard_all;
			auto iter3 = varmap.find("blankpagethreshold");
			if (iter3 != varmap.end())
			{
				double val = s_options.m_dBlankThreshold;
				val = (std::min)(std::max(0.0, val), 100.0);
				blank_handler.threshold = val;
			}
		}

		auto area_iter = varmap.find("area");
		if (area_iter != varmap.end())
		{
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
				twain_frame tf(area_values[0], area_values[1], area_values[2], area_values[3]);
				ac.set_use_area(true).set_acquire_area(tf);
			}
		}

		// set pdf options
		if (boost::any_cast<std::string>(varmap["filetype"].value()) == "pdf")
		{
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
				pagesizeopts.set_custom_size(width, height).set_custom_option(pdf_paper_size_custom::custom);
			}
			else
			if (word == "variable")
				pagesizeopts.set_custom_option(pdf_paper_size_custom::variable);
			else
				pagesizeopts.set_page_size(s_options.m_PageSizeMap[pdf_commands.m_strPaperSize]);

			// get scale options
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
					pagescaleopts.set_page_scale(pdf_page_scale::custom);
				}
				else
				if (word == "fitpage")
					pagescaleopts.set_page_scale(pdf_page_scale::fitpage);
				else
				if (word == "noscale")
					pagescaleopts.set_page_scale(pdf_page_scale::none);
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
			if ( encryption_on )
			{
				auto& encrypt_opts = pdfopts.get_encryption_options();
				encrypt_opts.set_use_encryption(true);
				encrypt_opts.set_owner_password(pdf_commands.m_strOwnerPass);
				encrypt_opts.set_user_password(pdf_commands.m_strUserPass);
				encrypt_opts.set_use_random_owner(pdf_commands.m_bRandomOwner);
				encrypt_opts.set_use_random_user(pdf_commands.m_bRandomUser);
				encrypt_opts.set_use_strong_encryption(pdf_commands.m_bStrong);

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
				std::set<pdf_permission> permissionContainer;
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
	else
		return false;
	return true;
}

class STFCallback : public twain_listener
{
	scanner_options* m_pScannerOpts;

public:
	STFCallback(scanner_options *mSS) : twain_listener(), m_pScannerOpts(mSS)
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

int start_acquisitions(const po::variables_map& varmap) 
{
	if (varmap.count("version"))
	{
		std::cout << "twainsave-opensource " << TWAINSAVE_FULL_VERSION << "\n";
		s_options.set_return_code(RETURN_OK);
		return RETURN_OK;
	}

	if (varmap.count("help"))
	{
		std::cout << desc2;
		s_options.set_return_code(RETURN_OK);
		return RETURN_OK;
	}
	
	// first start the TWAIN session
	twain_characteristics tc;
	auto iter = varmap.find("tempdir");
	if (iter != varmap.end())
		tc.set_temporary_directory(boost::any_cast<std::string>(iter->second.value()));
	iter = varmap.find("dsmsearchorder");
	if (iter != varmap.end())
	{
		int so = boost::any_cast<int>(iter->second.value());
		tc.set_dsm_search_order(static_cast<dsm_search_order>(so));
	}
	iter = varmap.find("diagnose");
	if (!iter->second.defaulted())
	{
		bool logging_enabled = (iter != varmap.end());
		if (iter != varmap.end())
		{
			auto& logdetails = tc.get_logger_details();
			logdetails.set_verbosity(static_cast<logger_verbosity>(s_options.m_nDiagnose));
			if (varmap.find("diagnoselog") != varmap.end())
			{
				if (s_options.m_DiagnoseLog == "*")
					logdetails.set_destination(logger_destination::todebug);
				else
					if (s_options.m_DiagnoseLog == "+")
						logdetails.set_destination(logger_destination::toconsole);
					else
					{
						logdetails.set_destination(logger_destination::tofile);
						logdetails.set_filename(s_options.m_DiagnoseLog);
					}
			}
			else
			{
				logdetails.set_destination(logger_destination::tofile);
				logdetails.set_filename("stddiag.log");
			}
			logdetails.enable(logging_enabled);
		}
	}

	twain_session ts(tc, true);
	if (ts)
	{
		if (!s_options.m_strSelectName.empty())
			g_source = std::make_unique<twain_source>(ts.select_source(select_byname(s_options.m_strSelectName), false));
		else
		if (s_options.m_bSelectDefault)
			g_source = std::make_unique<twain_source>(ts.select_source(select_default(), false));
		else
			g_source = std::make_unique<twain_source>(ts.select_source(select_prompt(), false));
		if (!g_source->is_selected())
		{
			s_options.set_return_code(RETURN_TWAIN_SOURCE_CANCEL);
			return RETURN_TWAIN_SOURCE_CANCEL;
		}
		else
			g_source->open();
		if (!g_source->is_open())
		{
			s_options.set_return_code(RETURN_TWAIN_SOURCE_ERROR);
			return RETURN_TWAIN_SOURCE_ERROR;
		}
	}
	else
	{
		s_options.set_return_code(RETURN_TWAIN_INIT_ERROR);
		return RETURN_TWAIN_INIT_ERROR;
	}
	STFCallback *pCallback = new STFCallback(&s_options);

	if (g_source->is_open())
	{
		// check for pixel types
		std::unordered_set<capability_interface::pixeltype_type> vPixelTypes;
		g_source->get_pixel_types(vPixelTypes);
		std::array<capability_interface::pixeltype_type, 3> supported_types = { DTWAIN_PT_BW, DTWAIN_PT_GRAY, DTWAIN_PT_RGB };
		bool bfound = false;
		for (size_t i = 0; i < supported_types.size(); ++i)
		{
			if (vPixelTypes.find(supported_types[i]) != vPixelTypes.end())
			{
				bfound = true;
				break;
			}
		}
		if (!bfound)
		{
			s_options.set_return_code(RETURN_COLORSPACE_NOT_SUPPORTED);
			g_source.reset();
			delete pCallback;
			return RETURN_COLORSPACE_NOT_SUPPORTED;
		}

		if (set_caps(*g_source, varmap))
		{
			ts.register_listener(*g_source, *pCallback);
			auto acq_return = g_source->acquire();
			if (acq_return.first == acquire_return_code::acquire_timeout)
				s_options.set_return_code(RETURN_TIMEOUT_REACHED);
			else
				s_options.set_return_code(RETURN_OK);
			g_source.reset();
		}
	}
	return 0;
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
	argv_str.push_back("twainsave-opensource.exe");
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
	return parse_options(args.size(), args.data());
}

int main(int argc, char *argv[])
{
	auto retval = parse_options(argc, argv);

	if (retval.first)
	{
		if (!s_options.m_strConfigFile.empty())
			retval = parse_config_options(s_options.m_strConfigFile);
		if ( retval.first )
			start_acquisitions(retval.second);
	}
	return s_options.get_return_code();
}