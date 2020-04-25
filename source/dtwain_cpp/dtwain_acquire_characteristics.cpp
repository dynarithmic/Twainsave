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
#pragma warning( disable:4996 )  // Stores the current warning state for every warning.
namespace dynarithmic
{
    namespace twain
    {
        acquire_characteristics::acquire_characteristics() : m_file_type(file_type::bmp),
            m_transfer_type(transfer_type::file_using_native),
            m_transfer_color_type(color_type::rgb),
            m_image_orientation(image_orientation::portrait),
            m_bMultiPage(false),
            m_bShowUI(true),
            m_bUseDuplex(false),
            m_dResolution(default_resolution),
            m_dThreshold(default_resolution),
            m_nPageCount(DTWAIN_MAXACQUIRE),
            m_nMaxAcquisitions(DTWAIN_MAXACQUIRE),
            m_bUseFeeder(false),
            m_bUseFeederOrFlatbed(false),
            m_bCloseSource(false),
            m_bUIOnly(false),
            m_transferFlags(file_transfer_flags::use_name),
            m_filepattern("test.bmp"),
            m_bUseHighSpeed(false),
            m_bUseArea(false),
            m_dRotation(default_rotation),
            m_dBrightness(default_brightness),
            m_dContrast(default_contrast),
            m_dGamma(default_gamma),
            m_measure_unit(measure_unit::inch),
            m_paper_size(paper_size::NONE),
            m_bWaitForFeederLoaded(false),
            m_jobcontrol_type(jobcontrol_type::none),
            m_bOverscanMode(false),
            m_bAutoDeskewMode(false),
            m_bAutoBrightMode(false),
            m_dHighlight(0),
            m_bNegateImages(false),
            m_feeder_timeout(10000),
            m_compression_type(twain_compression_type::none),
			m_bUseFilmScanMode(false),
			m_bUseIndicators(false),
			m_bAutoRotateMode(false),
			m_bSaveIncomplete(false),
			m_dShadow(0)
        {}

        acquire_characteristics& acquire_characteristics::set_acquire_area(twain_frame frame, bool b)
        {
            m_area_of_interest = frame;
            m_bUseArea = b;
            return *this;
        }

        acquire_characteristics& acquire_characteristics::set_auto_close_source(bool bSet)
        { m_bCloseSource = bSet; return *this; }

        acquire_characteristics& acquire_characteristics::set_brightness(double brightness)
        { m_dBrightness = brightness; return *this; }

        acquire_characteristics& acquire_characteristics::set_color_type(color_type ct)
        { m_transfer_color_type = ct; return *this; }

        acquire_characteristics& acquire_characteristics::set_color_type_i(int ct)
        { m_transfer_color_type = static_cast<color_type>(ct); return *this; }

        acquire_characteristics& acquire_characteristics::set_compression_type(twain_compression_type ct)
        { m_compression_type = ct; return *this; }

        acquire_characteristics& acquire_characteristics::set_contrast(double contrast)
        { m_dContrast = contrast; return *this;}

        acquire_characteristics & acquire_characteristics::set_duplex_mode(bool bSet)
        { m_bUseDuplex = bSet; return *this; }

        acquire_characteristics& acquire_characteristics::set_file_transfer_flags(file_transfer_flags ftf)
        { m_transferFlags = ftf; return *this; }

        acquire_characteristics& acquire_characteristics::set_file_type(file_type ft)
        { m_file_type = ft; return *this; }

        acquire_characteristics& acquire_characteristics::set_filename_pattern(const std::string& name)
        { m_filepattern = name; return *this; }

        acquire_characteristics& acquire_characteristics::set_gamma(double gamma)
        { m_dGamma = gamma; return *this; }

        acquire_characteristics& acquire_characteristics::set_halftone(const std::string& s)
        { m_sHalftone = s; return *this; }

        acquire_characteristics& acquire_characteristics::set_max_pages(int n)
        { m_nPageCount = n; return *this; }

        acquire_characteristics& acquire_characteristics::set_max_acquisitions(int n)
        { m_nMaxAcquisitions = n; return *this; }

        acquire_characteristics& acquire_characteristics::set_measure_unit(measure_unit mu)
        { m_measure_unit = mu; return *this; }

        acquire_characteristics& acquire_characteristics::set_measure_unit_i(int mu)
        { m_measure_unit = static_cast<measure_unit>(mu); return *this; }

        acquire_characteristics& acquire_characteristics::set_multi_page(bool bSet)
        { m_bMultiPage = bSet; return *this; }

        acquire_characteristics& acquire_characteristics::set_negate_images(bool bSet)
        { m_bNegateImages = bSet; return *this; }

        acquire_characteristics& acquire_characteristics::set_orientation(image_orientation orient)
        { m_image_orientation = orient; return *this; }

        acquire_characteristics& acquire_characteristics::set_orientation_i(int orient)
        { m_image_orientation = static_cast<image_orientation>(orient); return *this; }

        acquire_characteristics& acquire_characteristics::set_paper_size(paper_size ps)
        { m_paper_size = ps; return *this; }

        acquire_characteristics& acquire_characteristics::set_paper_size_i(int ps)
        { m_paper_size = static_cast<paper_size>(ps); return *this; }

        acquire_characteristics& acquire_characteristics::set_resolution(double res)
        { m_dResolution = res; return *this; }

        acquire_characteristics& acquire_characteristics::set_rotation(double rotation)
        { m_dRotation = rotation; return *this; }

        acquire_characteristics& acquire_characteristics::set_threshold(double threshold)
        { m_dThreshold = threshold; return *this; }

        acquire_characteristics& acquire_characteristics::set_show_ui(bool bSet)
        { m_bShowUI = bSet; return *this; }

        acquire_characteristics& acquire_characteristics::set_transfer_type(transfer_type tt)
        { m_transfer_type = tt; return *this; }

        acquire_characteristics& acquire_characteristics::set_uionly_mode(bool bSet)
        { m_bUIOnly = bSet; return *this; }

        acquire_characteristics& acquire_characteristics::set_use_feeder(bool bSet)
        { m_bUseFeeder = bSet; return *this; }

        acquire_characteristics& acquire_characteristics::set_use_feeder_or_flatbed(bool bSet)
        { m_bUseFeederOrFlatbed = bSet; return *this; }

        acquire_characteristics& acquire_characteristics::set_use_high_speed(bool bSet)
        { m_bUseHighSpeed = bSet; return *this; }

        acquire_characteristics & acquire_characteristics::set_use_filmscan_mode(bool bSet)
        { m_bUseFilmScanMode = bSet; return *this; }

        acquire_characteristics & acquire_characteristics::set_use_indicators(bool bSet)
        { m_bUseIndicators = bSet; return *this; }

        acquire_characteristics & acquire_characteristics::set_wait_for_feeder_loaded(bool bSet)
        { m_bWaitForFeederLoaded = bSet; return *this; }

        acquire_characteristics & acquire_characteristics::set_feeder_timeout(int seconds)
        { m_feeder_timeout = seconds; return *this; }

        acquire_characteristics & acquire_characteristics::set_highlight(double h)
        { m_dHighlight= h; return *this; }

        acquire_characteristics & acquire_characteristics::set_jobcontrol_type(jobcontrol_type jc)
        { m_jobcontrol_type = jc; return *this; }

        acquire_characteristics & acquire_characteristics::set_jobcontrol_type_i(int jc)
        { m_jobcontrol_type = static_cast<jobcontrol_type>(jc); return *this; }

        acquire_characteristics & acquire_characteristics::set_overscan_mode(bool bSet)
        { m_bOverscanMode = bSet; return *this; }

        acquire_characteristics & acquire_characteristics::set_shadow(double shadow)
        { m_dShadow = shadow; return *this; }

        acquire_characteristics& acquire_characteristics::set_autodeskew_mode(bool bSet)
        { m_bAutoDeskewMode = bSet; return *this; }

        acquire_characteristics& acquire_characteristics::set_autobright_mode(bool bSet)
        { m_bAutoBrightMode = bSet; return *this; }

        acquire_characteristics& acquire_characteristics::set_autorotate_mode(bool bSet)
        { m_bAutoRotateMode = bSet; return *this; }

        acquire_characteristics& acquire_characteristics::set_imprinter_string(const std::string& s)
        { m_sImprinter = s; return *this; }

        acquire_characteristics& acquire_characteristics::set_use_area( bool bUseArea)
        { m_bUseArea = bUseArea; return *this; }

        bool acquire_characteristics::get_wait_for_feeder_loaded() const { return m_bWaitForFeederLoaded; }
        int acquire_characteristics::get_feeder_timeout() const { return m_feeder_timeout; }
        bool acquire_characteristics::get_use_feeder() const { return m_bUseFeeder; }
        bool acquire_characteristics::get_filmscan_mode() const { return m_bUseFilmScanMode; }
        bool acquire_characteristics::get_indicators_mode() const { return m_bUseIndicators; }
        bool acquire_characteristics::get_overscan_mode() const { return m_bOverscanMode; }
        bool acquire_characteristics::is_auto_close_source() const { return m_bCloseSource; }
        bool acquire_characteristics::is_duplex_mode() const { return m_bUseDuplex; }
        bool acquire_characteristics::is_uionly_mode() const { return m_bUIOnly; }
        bool acquire_characteristics::is_use_area() const { return m_bUseArea; }
        bool acquire_characteristics::is_use_high_speed() const { return m_bUseHighSpeed; }
        bool acquire_characteristics::is_negate_images() const { return m_bNegateImages; }
        double acquire_characteristics::get_brightness() const { return m_dBrightness; }
        double acquire_characteristics::get_contrast() const { return m_dContrast; }
        double acquire_characteristics::get_gamma() const { return m_dGamma; }
        double acquire_characteristics::get_highlight() const { return m_dHighlight; }
        double acquire_characteristics::get_resolution() const { return m_dResolution; }
        double acquire_characteristics::get_rotation() const { return m_dRotation; }
        double acquire_characteristics::get_threshold() const { return m_dThreshold; }
        double acquire_characteristics::get_shadow() const { return m_dShadow; }
        bool acquire_characteristics::get_autodeskew_mode() const { return m_bAutoDeskewMode; }
        bool acquire_characteristics::get_autobright_mode() const { return m_bAutoBrightMode; }
        bool acquire_characteristics::get_autorotate_mode() const { return m_bAutoRotateMode; }
        std::string acquire_characteristics::get_imprinter_string() const { return m_sImprinter; }
        twain_compression_type acquire_characteristics::get_compression_type() const { return m_compression_type;}
        multipage_save_info& acquire_characteristics::get_multipage_save_info() { return m_multipage_save_info; }
        bool acquire_characteristics::get_use_feeder_or_flatbed() const { return m_bUseFeederOrFlatbed; }

        blank_image_handler& acquire_characteristics::get_blank_image_handler() { return m_blankImageHandler; }
        acquire_characteristics::lightsource_container & acquire_characteristics::get_lightsources() { return m_lightsources; }
        file_transfer_flags acquire_characteristics::get_file_transfer_flags() const { return m_transferFlags; }
        filename_increment_rules& acquire_characteristics::get_filename_increment_rules() { return m_filename_increment_rules; }
        image_orientation acquire_characteristics::get_orientation() const { return m_image_orientation; }
        int acquire_characteristics::get_max_pages() const { return m_nPageCount; }
        measure_unit acquire_characteristics::get_measure_unit() const { return m_measure_unit; }
        paper_size acquire_characteristics::get_paper_size() const { return m_paper_size; }
        std::string acquire_characteristics::get_halftone() const { return m_sHalftone; }
        twain_frame& acquire_characteristics::get_acquire_area() { return m_area_of_interest; }
        bool acquire_characteristics::can_multi_page() const { return m_bMultiPage && file_type_info::get_multipage_type(m_file_type); }
		long acquire_characteristics::get_multi_page_type() const { return file_type_info::get_multipage_type(m_file_type); }
        bool acquire_characteristics::is_multi_page() const { return m_bMultiPage; }
        bool acquire_characteristics::is_show_ui() const { return m_bShowUI; }
        color_type acquire_characteristics::get_color_type() const { return m_transfer_color_type; }
        file_type acquire_characteristics::get_file_type() const { return m_file_type; }
        std::string acquire_characteristics::get_filename_pattern() const { return m_filepattern; }
        transfer_type acquire_characteristics::get_transfer_type() const { return m_transfer_type; }
        jobcontrol_type acquire_characteristics::get_jobcontrol_type() const { return m_jobcontrol_type; }
        pdf_options& acquire_characteristics::get_pdf_options() { return m_pdf_options; }
        int acquire_characteristics::get_max_acquisitions() const { return m_nMaxAcquisitions; }
    }
}
