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
// Acquisition characteristics (optionally use feeder, duplex, hide the UI, etc.)
class acquire_characteristics
{
private:
    file_type m_file_type;
    transfer_type m_transfer_type;
    color_type m_transfer_color_type;
    filename_increment_rules m_filename_increment_rules;
    image_orientation m_image_orientation;
    pdf_options m_pdf_options;
    bool m_bMultiPage;
    bool m_bShowUI;
    bool m_bUseDuplex;
    bool m_bUseFilmScanMode;
    bool m_bUseIndicators;
    bool m_bAutoBrightMode;
    bool m_bUseFeeder;
    bool m_bCloseSource;
    bool m_bUIOnly;
    bool m_bWaitForFeederLoaded;
    bool m_bOverscanMode;
    bool m_bAutoDeskewMode;
    bool m_bAutoRotateMode;
    bool m_bNegateImages;
    bool m_bUseFeederOrFlatbed;
    bool m_bUseHighSpeed;
    bool m_bUseArea;
    bool m_bSaveIncomplete;
    double m_dResolution;
    int m_nPageCount;
    int m_feeder_timeout;
    int m_nMaxAcquisitions;
    blank_image_handler m_blankImageHandler;

    file_transfer_flags m_transferFlags;
    std::string m_filepattern;
    std::string m_halftone;
    twain_frame m_area_of_interest;
    double m_dRotation;
    double m_dBrightness;
    double m_dContrast;
    double m_dThreshold;
    double m_dGamma;
    double m_dShadow;
    double m_dHighlight;
    measure_unit m_measure_unit;
    std::string m_sHalftone;
    std::string m_sImprinter;
    paper_size m_paper_size;
    std::vector<lightsource_type> m_lightsources;
    jobcontrol_type m_jobcontrol_type;
    twain_compression_type m_compression_type;
    multipage_save_info m_multipage_save_info;

public:
    using lightsource_container = std::vector<lightsource_type>;
    using blankpage_detection_info = std::pair<bool, double>;
    static constexpr double default_resolution = DBL_MIN;
    static constexpr double default_rotation = DBL_MIN;
    static constexpr double default_brightness = DBL_MIN;
    static constexpr double default_contrast = DBL_MIN;
    static constexpr double default_gamma = DBL_MIN;
    static constexpr double default_threshold = DBL_MIN;
    static constexpr double default_shadow = DBL_MIN;
    static constexpr double default_highlight = DBL_MIN;
    static constexpr double default_blank_page_threshold = 98.0;
    static constexpr long default_compression = DTWAIN_CP_NONE;
    static constexpr long default_orientation = LONG_MAX;
    static constexpr long default_measure_unit = LONG_MIN;
    static constexpr long default_filmscan_mode = DTWAIN_LP_REFLECTIVE;
    static constexpr long default_indicators_mode = 1;
    static constexpr long default_autodeskew_mode = 0;
    static constexpr long default_autobright_mode = 0;
    static constexpr long default_autorotate_mode = 0;
    static constexpr long default_overscan_mode = 0;
    static constexpr paper_size default_paper_size = paper_size::NONE;
    static constexpr jobcontrol_type default_jobcontrol_type = jobcontrol_type::none;

    acquire_characteristics();
    acquire_characteristics& set_acquire_area(twain_frame frame, bool b = true);
    acquire_characteristics& set_auto_close_source(bool bSet = true);
    acquire_characteristics& set_brightness(double brightness);
    acquire_characteristics& set_color_type(color_type ct);
    acquire_characteristics& set_color_type_i(int ct);
    acquire_characteristics& set_compression_type(twain_compression_type ct);
    acquire_characteristics& set_contrast(double contrast);
    acquire_characteristics& set_duplex_mode(bool bSet = true);
    acquire_characteristics& set_file_transfer_flags(file_transfer_flags ftf);
    acquire_characteristics& set_file_type(file_type ft);
    acquire_characteristics& set_filename_pattern(const std::string& name);
    acquire_characteristics& set_gamma(double gamma);
    acquire_characteristics& set_halftone(const std::string& s);
    acquire_characteristics& set_highlight(double h);
    acquire_characteristics& set_max_acquisitions(int n);
    acquire_characteristics& set_max_pages(int n);
    acquire_characteristics& set_measure_unit(measure_unit mu);
    acquire_characteristics& set_measure_unit_i(int mu);
    acquire_characteristics& set_multi_page(bool bSet = true);
    acquire_characteristics& set_orientation(image_orientation orient);
    acquire_characteristics& set_orientation_i(int orient);
    acquire_characteristics& set_overscan_mode(bool bSet = true);
    acquire_characteristics& set_paper_size(paper_size ps);
    acquire_characteristics& set_paper_size_i(int ps);
    acquire_characteristics& set_resolution(double res);
    acquire_characteristics& set_rotation(double rotation);
    acquire_characteristics& set_shadow(double shadow);
    acquire_characteristics& set_threshold(double threshold);
    acquire_characteristics& set_show_ui(bool bSet = true);
    acquire_characteristics& set_transfer_type(transfer_type tt);
    acquire_characteristics& set_uionly_mode(bool bSet = true);
    acquire_characteristics& set_use_feeder(bool bSet = true);
    acquire_characteristics& set_use_high_speed(bool bSet = true);
    acquire_characteristics& set_use_area(bool bSet = true);
    acquire_characteristics& set_use_filmscan_mode(bool bSet = true);
    acquire_characteristics& set_use_indicators(bool bSet = true);
    acquire_characteristics& set_use_feeder_or_flatbed(bool bSet = true);
    acquire_characteristics& set_wait_for_feeder_loaded(bool bSet = true);
    acquire_characteristics& set_feeder_timeout(int milliseconds);
    acquire_characteristics& set_autodeskew_mode(bool bSet = true);
    acquire_characteristics& set_autobright_mode(bool bSet = true);
    acquire_characteristics& set_autorotate_mode(bool bSet = true);
    acquire_characteristics& set_negate_images(bool bSet = true);
    template <typename Container>
    acquire_characteristics& set_lightsources(const Container& ct);
    acquire_characteristics& set_jobcontrol_type(jobcontrol_type jc);
    acquire_characteristics& set_jobcontrol_type_i(int jc);
    acquire_characteristics& set_imprinter_string(const std::string& s);

    bool get_wait_for_feeder_loaded() const;
    int get_feeder_timeout() const;
    bool get_use_feeder() const;
    bool get_filmscan_mode() const;
    bool get_indicators_mode() const;
    bool get_overscan_mode() const;
    bool get_autodeskew_mode() const;
    bool get_autobright_mode() const;
    bool get_autorotate_mode() const;

    bool is_auto_close_source() const;
    bool is_duplex_mode() const;
    bool is_uionly_mode() const;
    bool is_use_area() const;
    bool is_use_high_speed() const;
    bool is_negate_images() const;
    blank_image_handler& get_blank_image_handler();
    multipage_save_info& get_multipage_save_info();

    double get_brightness() const;
    double get_contrast() const;
    double get_gamma() const;
    double get_highlight() const;
    double get_resolution() const;
    double get_rotation() const;
    double get_threshold() const;
    double get_shadow() const;
    bool get_use_feeder_or_flatbed() const;
    int get_max_acquisitions() const;
    twain_compression_type get_compression_type() const;

    lightsource_container& get_lightsources();
    file_transfer_flags get_file_transfer_flags() const;
    filename_increment_rules& get_filename_increment_rules();
    image_orientation get_orientation() const;
    int get_max_pages() const;
    measure_unit get_measure_unit() const;
    paper_size get_paper_size() const;
    std::string get_halftone() const;
    twain_frame& get_acquire_area();
    bool can_multi_page() const;
    bool is_multi_page() const;
	long get_multi_page_type() const;
    bool is_show_ui() const;
    color_type get_color_type() const;
    file_type get_file_type() const;
    std::string get_filename_pattern() const;
    transfer_type get_transfer_type() const;
    jobcontrol_type get_jobcontrol_type() const;
    pdf_options& get_pdf_options();
    std::string get_imprinter_string() const;
};

template <typename Container>
acquire_characteristics& acquire_characteristics::set_lightsources(const Container& ct)
{
    m_lightsources.clear();
    std::copy(ct.begin(), ct.end(), std::back_inserter(m_lightsources));
    return *this;
}