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
enum class acquire_return_code
{
    acquire_ok = 0,
    acquire_timeout = 1,
    acquire_cancelled = 2
};

class twain_source
{
public:
    using acquire_return_type = std::pair<acquire_return_code, image_handler>;
    using byte_array = std::vector<BYTE>;

    struct cap_status_info
    {
        typedef std::pair<bool, boost::tribool> status_type;
        status_type m_status;
        cap_status_info(const status_type& ct) : m_status(ct) {}
        cap_status_info() : m_status{ false, boost::logic::indeterminate } {}
        cap_status_info& set_supported(bool b) { m_status.first = b; return *this; }
        cap_status_info& set_enabled(status_type::second_type b) { m_status.second = b; return *this; }
        bool is_supported() const { return m_status.first; }
        status_type::second_type is_enabled() const { return m_status.second; }
    };

private:
    using high_level_cap_map = std::unordered_map <high_level_cap, std::tuple<long, long, std::function<long(long, const twain_source&)>>>;
    cap_status_info no_status_info;
    const int IS_SUPPORTED = 1;
    const int IS_ENABLED = 2;
    typedef LONG(DLLENTRY_DEF* infofn)(DTWAIN_SOURCE, LPSTR, LONG);
    typedef twain_app_info& (twain_app_info::*appfn)(const std::string&);
    DTWAIN_SOURCE m_theSource = nullptr;
    bool m_bIsSelected = false;
    twain_app_info m_sourceInfo;
    bool m_bCloseable = true;
    acquire_characteristics m_acquire_characteristics;
    buffered_transfer_info m_buffered_info;
    twain_printer_info m_twain_printer_info;
    std::unique_ptr<capability_interface> m_capability_info;
    std::unique_ptr<capability_listener> m_capability_listener;

    mutable std::unordered_map<high_level_cap, cap_status_info> m_cap_status_info_map;
    high_level_cap_map m_caps_to_test;

    void get_source_info_internal();
    void get_source_basic_cap_info(int);
    void get_one_source_basic_cap_info(int get_info, high_level_cap ct) const;
    void prepare_acquisition();
    bool prepare_high_speed();
    void prepare_halftones();
    void swap(twain_source& left, twain_source& right) noexcept;

    acquire_return_type acquire_to_file(transfer_type transtype);
    acquire_return_type acquire_to_image_handles(transfer_type transtype);

    void wait_for_feeder(bool & status) const;

    template <typename Container,typename SourceType,typename Fn>
    bool get_values_impl(Container& ct, SourceType st, Fn fn) const;

    template<typename Container, typename Fn>
    bool get_values_impl(Container & ct, Fn fn) const;

public:
    typedef double resolution_type;
    twain_source(const twain_source&) = delete;
    twain_source& operator=(const twain_source&) = delete;
    twain_source(twain_source&& rhs) noexcept;
    twain_source& operator=(twain_source&& rhs) noexcept;
    twain_source(DTWAIN_SOURCE theSource = nullptr);
    ~twain_source();
    void attach(DTWAIN_SOURCE source);
    void detach();

    template <typename T>
    void set_cap_listener(T& val)
    { m_capability_listener = std::make_unique<T>(T()); }

/*	template <typename T, bool isCloned=true>
    void set_cap_listener(T& val)
    { m_capability_listener = std::make_unique(val.clone()); }
    */

    void invoke_callback(int which, int capvalue, const capability_interface::cap_getter_info& gi, bool val=true);
    void invoke_callback(int which, int capvalue, const capability_interface::cap_setter_info& si, bool val=true);
    DTWAIN_SOURCE get_source() const { return m_theSource; }
    twain_app_info get_source_info() const { return m_sourceInfo; }
    void set_pdf_options();
    HANDLE get_current_image();
    acquire_characteristics& get_acquire_characteristics() { return m_acquire_characteristics; }
    buffered_transfer_info& get_buffered_transfer_info() { return m_buffered_info; }
    capability_interface& get_capability_interface() const { return *(m_capability_info.get()); }
    bool can_use_high_speed() const;
    const cap_status_info& get_cap_status(high_level_cap cap) const;

    acquire_return_type acquire();
    byte_array get_custom_data() const;
    bool set_custom_data(const byte_array& s) const;
    bool open();
    bool is_open() const;
    bool is_selected() const;
    bool is_closeable() const;
    
    bool close();
    image_information get_image_information() const;

    template <typename Container>
    bool get_pixel_types(Container& ct) const;

    template <typename Container>
    bool get_paper_sizes(Container& ct) const;

    template <typename Container>
    bool get_measure_units(Container& ct) const;

    template <typename Container>
    bool get_resolution_values(Container& ct) const;

    template <typename Container>
    bool get_contrast_values(Container& ct) const;

    template <typename Container>
    bool get_brightness_values(Container& ct) const;

    template <typename Container>
    bool get_halftone_values(Container& ct) const;

    template <typename Container>
    bool get_lightsource_values(Container& ct) const;

    template <typename Container>
    bool get_compression_types(Container& ct) const;

    template <typename Container>
    bool get_filexfer_formats(Container& ct) const;

    template <typename Container>
    bool get_orientation_values(Container& ct) const;

    template <typename Container>
    bool get_rotation_values(Container& ct) const;
};
