/*
This file is part of the Twainsave-opensource version
Copyright (c) 2002-2026 Dynarithmic Software.

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
#ifndef TWAINSAVE_APP_H
#define TWAINSAVE_APP_H

#include <string>
#include <array>
#include <algorithm>
#include <memory>

#include <boost/program_options/cmdline.hpp>
#include <boost/program_options/options_description.hpp> 
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/uuid/uuid_generators.hpp> 
#include <boost/uuid/uuid_io.hpp>         
#include <boost/dll/runtime_symbol_info.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <dynarithmic/twain/twain_session.hpp>
#include <dynarithmic/twain/twain_source.hpp>

#include <iostream>
#include <utility>
#include <iomanip>
#include <numeric>
#include <sstream>
#include <type_traits>
#include <unordered_map>
#include <algorithm>
#include <fstream>
#include <iostream>
#include "..\simpleini\SimpleIni.h"
#include "twainsave_verinfo.h"
#include "twainsave.h"
#include "twainsave_broker.h"
#include "twainsave_opensource.h"

std::string generate_details(dynarithmic::twain::twain_session* pSession = nullptr);
std::string generate_dtwainversion_info(dynarithmic::twain::twain_session* pSession = nullptr);
std::string generate_productnames(dynarithmic::twain::twain_session* pSession = nullptr);

struct scanner_options
{
    std::string m_filetype;
    std::string m_filename;
    std::string m_area;
    bool m_bUseADF{};
    bool m_bUseADFOrFlatbed;
    bool m_bCreateDir;
    bool m_bAutobrightMode;
    double m_brightness;
    double m_dContrast;
    double m_dGamma;
    int m_bitsPerPixel;
    bool m_bStartBroker;
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
    bool m_bShowProductNames;
    bool m_bUseDSM2;
    std::string m_strTempDirectory;
    int m_DSMSearchOrder;
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
    std::unordered_map<stringview, std::pair<dynarithmic::twain::filetype_value::value_type, dynarithmic::twain::compression_value::value_type>> m_MapMode2Map;
    std::map<int, std::string> m_ReturnCodesMap;
    std::map<int, std::string> m_StandardReturnCodesMap;
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
    scanner_options();
    void set_return_code(int returncode) { twainsave_return_value = returncode; }
    int get_return_code() const { return twainsave_return_value; }
};

struct pdf_controls
{
    bool m_bAscii;
    bool m_bStrong;
    bool m_bAES128Encrypt;
    bool m_bAES256Encrypt;
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

namespace po = boost::program_options;

class twainsave_app
{
    scanner_options s_options = {};
    pdf_controls pdf_commands = {};
    std::string default_name;
    std::string descript_name;
    std::string details_name;
    std::unique_ptr<po::options_description> desc2;
    std::unique_ptr<dynarithmic::twain::twain_source> g_source;

    public:
        using parse_return_type = std::pair<bool, po::variables_map>;
    private:
    	parse_return_type m_parse_return_type;

        struct sAdditionalTest
        {
            bool testOnlyTrueFalse = false;
            bool testMapValue = false;
            bool testCapOnly = false;
        };

	    class STFCallback : public dynarithmic::twain::twain_callback
	    {
            scanner_options* m_pScannerOpts = nullptr;

	        public:
                STFCallback();
                void setscanneropts(scanner_options* mSS);
		        int uiopenfailure(dynarithmic::twain::twain_source& source) override;
		        int acquiredone(dynarithmic::twain::twain_source& source) override;
		        int transferready(dynarithmic::twain::twain_source& source) override;
		        int filenamechanging(dynarithmic::twain::twain_source& source) override;
	    };

		class DerivedLogger : public dynarithmic::twain::twain_logger
		{
		    public:
			    enum logger_destination
			    {
				    todebug,
				    tofile,
				    toconsole
			    };

		    private:
			    logger_destination m_destination = toconsole;
			    std::string m_filename;
			    std::unique_ptr<std::ofstream> m_file;

		    public:
			    DerivedLogger() = default;
			    DerivedLogger& set_destination(logger_destination destination);
			    DerivedLogger& set_filename(std::string filename);
			    bool enable();
			    virtual void log(const char* msg) override;
		};


        STFCallback m_tsCallback;
        DerivedLogger m_logger;
        void set_blank_page_options();
        void set_scale_options();
        bool set_areaofinterest_options();
        void set_pdf_options();
        bool check_device_options(bool doOptionCheck = true);
        bool set_device_options();

	    template <typename mapType, typename ValueType>
	    bool check_option_value(mapType& m, ValueType& v, int retValue)
	    {
		    auto iter = m.find(v);
		    if (iter == m.end())
		    {
			    s_options.set_return_code(retValue);
			    return false;
		    }
		    return true;
	    }

    public:
        twainsave_app();
        int start_acquisitions(dynarithmic::twain::twain_session* pSession = nullptr);
        void load_custom_resources_from_ini();
        void load_resources_from_rc();
        void reload_custom_resources();
        scanner_options& get_scanner_options() { return s_options; }
	    parse_return_type parse_options(int argc, char* argv[]);
        parse_return_type parse_config_options(const std::string& filename);
};
#endif