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
// This is the entire TWAIN capabilities interface.  
class capability_interface
{
public:
    typedef CAP_SUPPORTEDCAPS_::value_type twain_cap_type;
    typedef std::string twain_string_type;
    typedef std::map<twain_cap_type, twain_string_type> source_cap_info;
    typedef std::pair<bool, int> cap_return_type;
    enum {CAP_CALLBACK_GET_BEGIN, CAP_CALLBACK_GET_END, CAP_CALLBACK_SET_BEGIN, CAP_CALLBACK_SET_END};
        
    // This class tells us whether to use MSG_GET, MSG_GETDEFAULT, MSG_GETCURRENT when retrieving capability info
    struct cap_getter_info
    {
        getcap_type getter_type;
        twain_container_type container_type;
        twain_data_type data_type;
        bool expand_if_range;
        
        cap_getter_info() : getter_type(getcap_type::GET),
                            container_type(twain_container_type::CONTAINER_DEFAULT),
                            data_type(twain_data_type::DATATYPE_DEFAULT), expand_if_range(false)
        {}
        
        cap_getter_info& set_getter_type(getcap_type gt)
        { getter_type = gt; return *this; }
        
        cap_getter_info& set_expand_range(bool bSet = true)
        { expand_if_range = bSet; return *this; }
        
        cap_getter_info& set_container_type(twain_container_type ct)
        { container_type = ct; return *this; }
        
        cap_getter_info& set_data_type(twain_data_type dt)
        { data_type = dt; return *this; }
        
        getcap_type get_getter_type() const { return getter_type; }
        twain_container_type get_container_type() const { return container_type; }
        
        // tells if we want to expand the range values.  There could be thousands of values in the range, so be careful if this is set to true
        bool get_expand_range() const { return expand_if_range; }
        twain_data_type get_data_type() const { return data_type; }
    };
        
    // This class tells us whether to use MSG_SET, MSG_SETDEFAULT, MSG_SETCURRENT, MSG_SETCONSTRAINT, or MSG_RESET when setting capability info
    struct cap_setter_info
    {
        setcap_type setter_type;
        twain_container_type container_type;
        twain_data_type data_type;
        
        cap_setter_info() : setter_type(setcap_type::SET),
                            container_type(twain_container_type::CONTAINER_DEFAULT),
                            data_type(twain_data_type::DATATYPE_DEFAULT)
        {}
        
        cap_setter_info& set_setter_type(setcap_type st)
        { setter_type = st; return *this; }
        
        cap_setter_info& set_container_type(twain_container_type ct)
        { container_type = ct; return *this; }
        
        cap_setter_info& set_data_type(twain_data_type dt)
        { data_type = dt; return *this;}
        
        setcap_type get_setter_type() const { return setter_type; }
        twain_container_type get_container_type() const { return container_type; }
        twain_data_type get_data_type() const { return data_type; }
    };
        
private:
    DTWAIN_SOURCE m_Source;
    source_cap_info m_caps;
    using cache_vector_type = std::vector<boost::variant<long, std::string, double, twain_frame>>;
    using capability_cache = std::unordered_map<int, cache_vector_type>;
    using cache_set_type = std::unordered_set<int>;
    mutable capability_cache m_cap_cache;
    cache_set_type m_cacheable_set;
        
    template <typename Container>
    void copy_to_cache(const Container& ct, int capvalue) const
    {
        auto iter = m_cap_cache.insert({capvalue, cache_vector_type()}).first;
        auto& vect = iter->second;
        std::copy(ct.begin(), ct.end(), typename std::back_inserter(vect));
    }
        
    template <typename Container>
    bool copy_from_cache(Container& ct, int capvalue) const
    {
        auto iter = m_cap_cache.find(capvalue);
        if (iter == m_cap_cache.end())
            return false;
        auto& vect = iter->second;
        std::transform(std::begin(vect), std::end(vect), std::inserter(ct, ct.end()),
                        [&](cache_vector_type::value_type& vt)
                        {
                            return boost::get < Container::value_type > (vt);
                        });
        return true;
    }
        
    void initialize_cached_set();
    bool fill_caps();
        
    template <typename Container, typename Cap>
    cap_return_type get_caps_impl(Container& ct, const cap_getter_info& gcType) const
    {
        static_assert(std::is_same<typename Container::value_type, typename Cap::value_type>(),
            "Container has incompatible type");
        return get_cap_values<Cap>(ct, gcType);
    }
        
    template <typename Container, typename Cap>
    cap_return_type set_caps_impl(const Container& ct, const cap_setter_info& scType)
    {
        static_assert(std::is_same<typename Container::value_type, typename Cap::value_type>(),
            "Container has incompatible type");
        return set_cap_values<Cap>(ct, scType);
    }
        
public:
    typedef source_cap_info::value_type value_type;
    typedef std::string camera_name_type;
    DTWAIN_SOURCE get_source() const { return m_Source; }
        
    static cap_getter_info get()
    {
        return cap_getter_info().set_getter_type(getcap_type::GET);
    }
        
    static cap_getter_info get_current()
    {
        return cap_getter_info().set_getter_type(getcap_type::GET_CURRENT);
    }
        
    static cap_getter_info get_default()
    {
        return cap_getter_info().set_getter_type(getcap_type::GET_DEFAULT);
    }
        
    static cap_setter_info set()
    {
        return cap_setter_info().set_setter_type(setcap_type::SET);
    }
        
    static cap_setter_info reset()
    {
        return cap_setter_info().set_setter_type(setcap_type::RESET);
    }
        
    static cap_setter_info set_constraint()
    {
        return cap_setter_info().set_setter_type(setcap_type::SET_CONSTRAINT);
    }
        
    // These are aliases for all of the types supported by each capability
    using alarms_type = CAP_ALARMS_::value_type;
    using alarmvolume_type = CAP_ALARMVOLUME_::value_type;
    using audio_xfermech_type = ACAP_XFERMECH_::value_type;
    using author_type = CAP_AUTHOR_::value_type;
    using autobright_type = ICAP_AUTOBRIGHT_::value_type;
    using autodiscardblankpages_type = ICAP_AUTODISCARDBLANKPAGES_::value_type;
    using autofeed_type = CAP_AUTOFEED_::value_type;
    using automaticborderdetection_type = ICAP_AUTOMATICBORDERDETECTION_::value_type;
    using automaticcapture_type = CAP_AUTOMATICCAPTURE_::value_type;
    using automaticcolorenabled_type = ICAP_AUTOMATICCOLORENABLED_::value_type;
    using automaticcolornoncolorpixeltype_type = ICAP_AUTOMATICCOLORNONCOLORPIXELTYPE_::value_type;
    using automaticcropusesframe_type = ICAP_AUTOMATICCROPUSESFRAME_::value_type;
    using automaticdeskew_type = ICAP_AUTOMATICDESKEW_::value_type;
    using automaticlengthdetection_type = ICAP_AUTOMATICLENGTHDETECTION_::value_type;
    using automaticrotate_type = ICAP_AUTOMATICROTATE_::value_type;
    using automaticsensemedium_type = CAP_AUTOMATICSENSEMEDIUM_::value_type;
    using autoscan_type = CAP_AUTOSCAN_::value_type;
    using autosize_type = ICAP_AUTOSIZE_::value_type;
    using barcodedetectionenabled_type = ICAP_BARCODEDETECTIONENABLED_::value_type;
    using barcodemaxretries_type = ICAP_BARCODEMAXRETRIES_::value_type;
    using barcodemaxsearchpriorities_type = ICAP_BARCODEMAXSEARCHPRIORITIES_::value_type;
    using barcodesearchmode_type = ICAP_BARCODESEARCHMODE_::value_type;
    using barcodesearchpriorities_type = ICAP_BARCODESEARCHPRIORITIES_::value_type;
    using barcodetimeout_type = ICAP_BARCODETIMEOUT_::value_type;
    using batteryminutes_type = CAP_BATTERYMINUTES_::value_type;
    using batterypercentage_type = CAP_BATTERYPERCENTAGE_::value_type;
    using bitdepth_type = ICAP_BITDEPTH_::value_type;
    using bitdepthreduction_type = ICAP_BITDEPTHREDUCTION_::value_type;
    using bitorder_type = ICAP_BITORDER_::value_type;
    using bitordercodes_type = ICAP_BITORDERCODES_::value_type;
    using brightness_type = ICAP_BRIGHTNESS_::value_type;
    using cameraenabled_type = CAP_CAMERAENABLED_::value_type;
    using cameraorder_type = CAP_CAMERAORDER_::value_type;
    using camerapreviewui_type = CAP_CAMERAPREVIEWUI_::value_type;
    using cameraside_type = CAP_CAMERASIDE_::value_type;
    using caption_type = CAP_CAPTION_::value_type;
    using ccittkfactor_type = ICAP_CCITTKFACTOR_::value_type;
    using clearpage_type = CAP_CLEARPAGE_::value_type;
    using colormanagementenabled_type = ICAP_COLORMANAGEMENTENABLED_::value_type;
    using compression_type = ICAP_COMPRESSION_::value_type;
    using contrast_type = ICAP_CONTRAST_::value_type;
    using custhalftone_type = ICAP_CUSTHALFTONE_::value_type;
    using customdsdata_type = CAP_CUSTOMDSDATA_::value_type;
    using custominterfaceguid_type = CAP_CUSTOMINTERFACEGUID_::value_type;
    using deviceevent_type = CAP_DEVICEEVENT_::value_type;
    using deviceonline_type = CAP_DEVICEONLINE_::value_type;
    using devicetimedate_type = CAP_DEVICETIMEDATE_::value_type;
    using doublefeeddetection_type = CAP_DOUBLEFEEDDETECTION_::value_type;
    using doublefeeddetectionlength_type = CAP_DOUBLEFEEDDETECTIONLENGTH_::value_type;
    using doublefeeddetectionresponse_type = CAP_DOUBLEFEEDDETECTIONRESPONSE_::value_type;
    using doublefeeddetectionsensitivity_type = CAP_DOUBLEFEEDDETECTIONSENSITIVITY_::value_type;
    using duplex_type = CAP_DUPLEX_::value_type;
    using duplexenabled_type = CAP_DUPLEXENABLED_::value_type;
    using enabledsuionly_type = CAP_ENABLEDSUIONLY_::value_type;
    using endorser_type = CAP_ENDORSER_::value_type;
    using exposuretime_type = ICAP_EXPOSURETIME_::value_type;
    using extendedcaps_type = CAP_EXTENDEDCAPS_::value_type;
    using extendedcaps_type = CAP_EXTENDEDCAPS_::value_type;
    using extimageinfo_type = ICAP_EXTIMAGEINFO_::value_type;
    using feederalignment_type = CAP_FEEDERALIGNMENT_::value_type;
    using feederenabled_type = CAP_FEEDERENABLED_::value_type;
    using feederloaded_type = CAP_FEEDERLOADED_::value_type;
    using feederorder_type = CAP_FEEDERORDER_::value_type;
    using feederpocket_type = CAP_FEEDERPOCKET_::value_type;
    using feederprep_type = CAP_FEEDERPREP_::value_type;
    using feedertype_type = ICAP_FEEDERTYPE_::value_type;
    using feedpage_type = CAP_FEEDPAGE_::value_type;
    using filmtype_type = ICAP_FILMTYPE_::value_type;
    using filtertype_type = ICAP_FILTER_::value_type;
    using flashused2_type = ICAP_FLASHUSED2_::value_type;
    using fliprotation_type = ICAP_FLIPROTATION_::value_type;
    using frames_type = ICAP_FRAMES_::value_type;
    using gamma_type = ICAP_GAMMA_::value_type;
    using halftones_type = ICAP_HALFTONES_::value_type;
    using highlight_type = ICAP_HIGHLIGHT_::value_type;
    using iccprofile_type = ICAP_ICCPROFILE_::value_type;
    using image_xfermech_type = ICAP_XFERMECH_::value_type;
    using imagedataset_type = ICAP_IMAGEDATASET_::value_type;
    using imagefileformat_type = ICAP_IMAGEFILEFORMAT_::value_type;
    using imagefilter_type = ICAP_IMAGEFILTER_::value_type;
    using imagemerge_type = ICAP_IMAGEMERGE_::value_type;
    using imagemergeheightthreshold_type = ICAP_IMAGEMERGEHEIGHTTHRESHOLD_::value_type;
    using indicators_type = CAP_INDICATORS_::value_type;
    using indicatorsmode_type = CAP_INDICATORSMODE_::value_type;
    using jobcontrol_type = CAP_JOBCONTROL_::value_type;
    using jpegpixeltype_type = ICAP_JPEGPIXELTYPE_::value_type;
    using jpegquality_type = ICAP_JPEGQUALITY_::value_type;
    using jpegsubsampling_type = ICAP_JPEGSUBSAMPLING_::value_type;
    using lampstate_type = ICAP_LAMPSTATE_::value_type;
    using language_type = CAP_LANGUAGE_::value_type;
    using lightpath_type = ICAP_LIGHTPATH_::value_type;
    using lightsource_type = ICAP_LIGHTSOURCE_::value_type;
    using maxbatchbuffers_type = CAP_MAXBATCHBUFFERS_::value_type;
    using maxframes_type = ICAP_MAXFRAMES_::value_type;
    using micrenabled_type = CAP_MICRENABLED_::value_type;
    using minimumheight_type = ICAP_MINIMUMHEIGHT_::value_type;
    using minimumwidth_type = ICAP_MINIMUMWIDTH_::value_type;
    using mirror_type = ICAP_MIRROR_::value_type;
    using noisefilter_type = ICAP_NOISEFILTER_::value_type;
    using orientation_type = ICAP_ORIENTATION_::value_type;
    using overscan_type = ICAP_OVERSCAN_::value_type;
    using paperdetectable_type = CAP_PAPERDETECTABLE_::value_type;
    using paperhandling_type = CAP_PAPERHANDLING_::value_type;
    using patchcodedetectionenabled_type = ICAP_PATCHCODEDETECTIONENABLED_::value_type;
    using patchcodemaxretries_type = ICAP_PATCHCODEMAXRETRIES_::value_type;
    using patchcodemaxsearchpriorities_type = ICAP_PATCHCODEMAXSEARCHPRIORITIES_::value_type;
    using patchcodesearchmode_type = ICAP_PATCHCODESEARCHMODE_::value_type;
    using patchcodesearchpriorities_type = ICAP_PATCHCODESEARCHPRIORITIES_::value_type;
    using patchcodetimeout_type = ICAP_PATCHCODETIMEOUT_::value_type;
    using physicalheight_type = ICAP_PHYSICALHEIGHT_::value_type;
    using physicalwidth_type = ICAP_PHYSICALWIDTH_::value_type;
    using pixelflavor_type = ICAP_PIXELFLAVOR_::value_type;
    using pixelflavorcodes_type = ICAP_PIXELFLAVORCODES_::value_type;
    using pixeltype_type = ICAP_PIXELTYPE_::value_type;
    using planarchunky_type = ICAP_PLANARCHUNKY_::value_type;
    using powersavetime_type = CAP_POWERSAVETIME_::value_type;
    using powersupply_type = CAP_POWERSUPPLY_::value_type;
    using printer_type = CAP_PRINTER_::value_type;
    using printercharrotation_type = CAP_PRINTERCHARROTATION_::value_type;
    using printerenabled_type = CAP_PRINTERENABLED_::value_type;
    using printerfontstyle_type = CAP_PRINTERFONTSTYLE_::value_type;
    using printerindex_type = CAP_PRINTERINDEX_::value_type;
    using printerindexleadchar_type = CAP_PRINTERINDEXLEADCHAR_::value_type;
    using printerindexmaxvalue_type = CAP_PRINTERINDEXMAXVALUE_::value_type;
    using printerindexnumdigits_type = CAP_PRINTERINDEXNUMDIGITS_::value_type;
    using printerindexstep_type = CAP_PRINTERINDEXSTEP_::value_type;
    using printerindextrigger_type = CAP_PRINTERINDEXTRIGGER_::value_type;
    using printermode_type = CAP_PRINTERMODE_::value_type;
    using printerstring_type = CAP_PRINTERSTRING_::value_type;
    using printerstringpreview_type = CAP_PRINTERSTRINGPREVIEW_::value_type;
    using printersuffix_type = CAP_PRINTERSUFFIX_::value_type;
    using printerverticaloffset_type = CAP_PRINTERVERTICALOFFSET_::value_type;
    using reacquireallowed_type = CAP_REACQUIREALLOWED_::value_type;
    using rewindpage_type = CAP_REWINDPAGE_::value_type;
    using rotation_type = ICAP_ROTATION_::value_type;
    using segmented_type = CAP_SEGMENTED_::value_type;
    using serialnumber_type = CAP_SERIALNUMBER_::value_type;
    using shadow_type = ICAP_SHADOW_::value_type;
    using sheetcount_type = CAP_SHEETCOUNT_::value_type;
    using supportedbarcodetypes_type = ICAP_SUPPORTEDBARCODETYPES_::value_type;
    using supportedcaps_type = CAP_SUPPORTEDCAPS_::value_type;
    using supportedcapssegmentunique_type = CAP_SUPPORTEDCAPSSEGMENTUNIQUE_::value_type;
    using supporteddats_type = CAP_SUPPORTEDDATS_::value_type;
    using supportedextimageinfo_type = ICAP_SUPPORTEDEXTIMAGEINFO_::value_type;
    using supportedpatchcodetypes_type = ICAP_SUPPORTEDPATCHCODETYPES_::value_type;
    using supportedsizes_type = ICAP_SUPPORTEDSIZES_::value_type;
    using threshold_type = ICAP_THRESHOLD_::value_type;
    using thumbnailsenabled_type = CAP_THUMBNAILSENABLED_::value_type;
    using tiles_type = ICAP_TILES_::value_type;
    using timebeforefirstcapture_type = CAP_TIMEBEFOREFIRSTCAPTURE_::value_type;
    using timebetweencaptures_type = CAP_TIMEBETWEENCAPTURES_::value_type;
    using timedate_type = CAP_TIMEDATE_::value_type;
    using timefill_type = ICAP_TIMEFILL_::value_type;
    using uicontrollable_type = CAP_UICONTROLLABLE_::value_type;
    using undefinedimagesize_type = ICAP_UNDEFINEDIMAGESIZE_::value_type;
    using units_type = ICAP_UNITS_::value_type;
    using xfercount_type = CAP_XFERCOUNT_::value_type;
    using xnativeresolution_type = ICAP_XNATIVERESOLUTION_::value_type;
    using xresolution_type = ICAP_XRESOLUTION_::value_type;
    using resolution_type = xresolution_type;
    using xscaling_type = ICAP_XSCALING_::value_type;
    using ynativeresolution_type = ICAP_YNATIVERESOLUTION_::value_type;
    using yresolution_type = ICAP_YRESOLUTION_::value_type;
    using yscaling_type = ICAP_YSCALING_::value_type;
    using zoomfactor_type = ICAP_ZOOMFACTOR_::value_type;
                    
                    
    // Note.  These functions cover *all* of the getting and setting and capabilities that have
    // been available as of TWAIN specification 2.4.  An application can explicitly call these
    // functions, since the function name matches the capability that is attempted to be retrieved
    // or set (the name matches the capability without the CAP_ or ICAP_ prefix.
                    
    // This one is special, since there is an ACAP_XFERMECH and an ICAP_XFERMECH.  This function is
    // for ACAP_XFERMECH.
    template <typename Container>
    cap_return_type get_audio_xfermech(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ACAP_XFERMECH_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_alarms(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, CAP_ALARMS_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_alarmvolume(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, CAP_ALARMVOLUME_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_author(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, CAP_AUTHOR_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_autofeed(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, CAP_AUTOFEED_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_automaticcapture(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, CAP_AUTOMATICCAPTURE_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_automaticsensemedium(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, CAP_AUTOMATICSENSEMEDIUM_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_autoscan(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, CAP_AUTOSCAN_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_batteryminutes(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, CAP_BATTERYMINUTES_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_batterypercentage(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, CAP_BATTERYPERCENTAGE_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_cameraenabled(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, CAP_CAMERAENABLED_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_cameraorder(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, CAP_CAMERAORDER_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_camerapreviwewui(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, CAP_CAMERAPREVIEWUI_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_cameraside(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, CAP_CAMERASIDE_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_caption(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, CAP_CAPTION_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_clearpage(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, CAP_CLEARPAGE_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_customdsdata(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, CAP_CUSTOMDSDATA_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_custominterfaceguid(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, CAP_CUSTOMINTERFACEGUID_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_deviceevent(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, CAP_DEVICEEVENT_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_deviceonline(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, CAP_DEVICEONLINE_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_devicetimedate(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, CAP_DEVICETIMEDATE_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_doublefeeddetection(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, CAP_DOUBLEFEEDDETECTION_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_doublefeeddetectionlength(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, CAP_DOUBLEFEEDDETECTIONLENGTH_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_doublefeeddetectionresponse(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, CAP_DOUBLEFEEDDETECTIONRESPONSE_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_doublefeeddetectionsensitivity(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, CAP_DOUBLEFEEDDETECTIONSENSITIVITY_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_duplex(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, CAP_DUPLEX_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_duplexenabled(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, CAP_DUPLEXENABLED_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_enabledsuionly(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, CAP_ENABLEDSUIONLY_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_endorser(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, CAP_ENDORSER_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_extendedcaps(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const 
    { return get_caps_impl<Container, CAP_EXTENDEDCAPS_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_feederalignment(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, CAP_FEEDERALIGNMENT_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_feederenabled(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, CAP_FEEDERENABLED_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_feederloaded(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, CAP_FEEDERLOADED_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_feederorder(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, CAP_FEEDERORDER_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_feederpocket(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, CAP_FEEDERPOCKET_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_feederprep(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, CAP_FEEDERPREP_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_feedpage(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, CAP_FEEDPAGE_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_indicators(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, CAP_INDICATORS_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_indicatorsmode(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, CAP_INDICATORSMODE_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_jobcontrol(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, CAP_JOBCONTROL_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_language(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, CAP_LANGUAGE_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_maxbatchbuffers(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, CAP_MAXBATCHBUFFERS_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_micrenabled(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, CAP_MICRENABLED_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_paperdetectable(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, CAP_PAPERDETECTABLE_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_paperhandling(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, CAP_PAPERHANDLING_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_powersavetime(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, CAP_POWERSAVETIME_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_powersupply(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, CAP_POWERSUPPLY_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_printer(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, CAP_PRINTER_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_printercharrotation(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, CAP_PRINTERCHARROTATION_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_printerenabled(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, CAP_PRINTERENABLED_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_printerfontstyle(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, CAP_PRINTERFONTSTYLE_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_printerindex(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, CAP_PRINTERINDEX_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_printerindexleadchar(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, CAP_PRINTERINDEXLEADCHAR_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_printerindexmaxvalue(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, CAP_PRINTERINDEXMAXVALUE_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_printerindexnumdigits(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, CAP_PRINTERINDEXNUMDIGITS_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_printerindexstep(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, CAP_PRINTERINDEXSTEP_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_printerindextrigger(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, CAP_PRINTERINDEXTRIGGER_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_printermode(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, CAP_PRINTERMODE_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_printerstring(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, CAP_PRINTERSTRING_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_printerstringpreview(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, CAP_PRINTERSTRINGPREVIEW_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_printersuffix(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, CAP_PRINTERSUFFIX_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_printerverticaloffset(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, CAP_PRINTERVERTICALOFFSET_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_reacquireallowed(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, CAP_REACQUIREALLOWED_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_rewindpage(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, CAP_REWINDPAGE_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_segmented(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, CAP_SEGMENTED_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_serialnumber(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, CAP_SERIALNUMBER_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_sheetcount(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, CAP_SHEETCOUNT_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_supportedcaps(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, CAP_SUPPORTEDCAPS_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_supportedcapssegmentunique(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, CAP_SUPPORTEDCAPSSEGMENTUNIQUE_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_supporteddats(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, CAP_SUPPORTEDDATS_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_timebeforefirstcapture(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, CAP_TIMEBEFOREFIRSTCAPTURE_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_timebetweencaptures(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, CAP_TIMEBETWEENCAPTURES_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_timedate(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, CAP_TIMEDATE_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_thumbnailsenabled(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, CAP_THUMBNAILSENABLED_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_uicontrollable(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, CAP_UICONTROLLABLE_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_xfercount(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, CAP_XFERCOUNT_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_autobright(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_AUTOBRIGHT_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_autodiscardblankpages(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_AUTODISCARDBLANKPAGES_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_automaticborderdetection(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_AUTOMATICBORDERDETECTION_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_automaticcolorenabled(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_AUTOMATICCOLORENABLED_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_automaticcolornoncolorpixeltype(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_AUTOMATICCOLORNONCOLORPIXELTYPE_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_automaticcropusesframe(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_AUTOMATICCROPUSESFRAME_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_automaticdeskew(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_AUTOMATICDESKEW_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_automaticlengthdetection(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_AUTOMATICLENGTHDETECTION_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_automaticrotate(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_AUTOMATICROTATE_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_autosize(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_AUTOSIZE_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_barcodedetectionenabled(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_BARCODEDETECTIONENABLED_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_barcodemaxretries(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_BARCODEMAXRETRIES_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_barcodemaxsearchpriorities(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_BARCODEMAXSEARCHPRIORITIES_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_barcodesearchmode(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_BARCODESEARCHMODE_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_barcodesearchpriorities(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_BARCODESEARCHPRIORITIES_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_barcodetimeout(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_BARCODETIMEOUT_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_bitdepth(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_BITDEPTH_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_bitdepthreduction(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_BITDEPTHREDUCTION_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_bitorder(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_BITORDER_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_bitordercodes(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_BITORDERCODES_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_brightness(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_BRIGHTNESS_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_ccittkfactor(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_CCITTKFACTOR_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_colormanagementenabled(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_COLORMANAGEMENTENABLED_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_compression(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_COMPRESSION_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_contrast(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_CONTRAST_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_custhalftone(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_CUSTHALFTONE_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_exposuretime(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_EXPOSURETIME_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_extimageinfo(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_EXTIMAGEINFO_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_feedertype(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_FEEDERTYPE_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_filmtype(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_FILMTYPE_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_filter(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_FILTER_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_flashused2(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_FLASHUSED2_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_fliprotation(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_FLIPROTATION_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_frames(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_FRAMES_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_gamma(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_GAMMA_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_halftones(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_HALFTONES_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_highlight(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_HIGHLIGHT_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_iccprofile(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_ICCPROFILE_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_imagedataset(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_IMAGEDATASET_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_imagefileformat(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_IMAGEFILEFORMAT_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_imagefilter(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_IMAGEFILTER_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_imagemerge(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_IMAGEMERGE_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_imagemergeheightthreshold(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_IMAGEMERGEHEIGHTTHRESHOLD_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_jpegpixeltype(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_JPEGPIXELTYPE_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_jpegquality(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_JPEGQUALITY_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_jpegsubsampling(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_JPEGSUBSAMPLING_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_lampstate(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_LAMPSTATE_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_lightpath(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_LIGHTPATH_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_lightsource(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_LIGHTSOURCE_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_maxframes(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_MAXFRAMES_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_minimumheight(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_MINIMUMHEIGHT_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_minimumwidth(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_MINIMUMWIDTH_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_mirror(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_MIRROR_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_noisefilter(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_NOISEFILTER_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_orientation(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_ORIENTATION_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_overscan(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_OVERSCAN_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_patchcodedetectionenabled(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_PATCHCODEDETECTIONENABLED_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_patchcodemaxretries(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_PATCHCODEMAXRETRIES_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_patchcodemaxsearchpriorities(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_PATCHCODEMAXSEARCHPRIORITIES_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_patchcodesearchmode(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_PATCHCODESEARCHMODE_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_patchcodesearchpriorities(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_PATCHCODESEARCHPRIORITIES_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_patchcodetimeout(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_PATCHCODETIMEOUT_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_physicalheight(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_PHYSICALHEIGHT_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_physicalwidth(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_PHYSICALWIDTH_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_pixelflavor(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_PIXELFLAVOR_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_pixelflavorcodes(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_PIXELFLAVORCODES_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_pixeltype(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_PIXELTYPE_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_planarchunky(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_PLANARCHUNKY_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_rotation(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_ROTATION_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_shadow(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_SHADOW_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_supportedbarcodetypes(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_SUPPORTEDBARCODETYPES_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_supportedextimageinfo(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_SUPPORTEDEXTIMAGEINFO_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_supportedpatchcodetypes(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_SUPPORTEDPATCHCODETYPES_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_supportedsizes(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_SUPPORTEDSIZES_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_threshold(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_THRESHOLD_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_tiles(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_TILES_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_timefill(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_TIMEFILL_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_undefinedimagesize(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_UNDEFINEDIMAGESIZE_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_units(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_UNITS_>(ct, gcType); }
                    
    // This one is special, since there is an ACAP_XFERMECH and an ICAP_XFERMECH.  This function is
    // for ICAP_XFERMECH.
    template <typename Container>
    cap_return_type get_image_xfermech(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_XFERMECH_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_xnativeresolution(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_XNATIVERESOLUTION_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_ynativeresolution(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_YNATIVERESOLUTION_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_xresolution(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_XRESOLUTION_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_yresolution(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_YRESOLUTION_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_xscaling(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_XSCALING_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_yscaling(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_YSCALING_>(ct, gcType); }
                    
    template <typename Container>
    cap_return_type get_zoomfactor(Container& ct, const cap_getter_info& gcType = cap_getter_info()) const
    { return get_caps_impl<Container, ICAP_ZOOMFACTOR_>(ct, gcType); }
                    
    /////////////////////////////////////////////////////////////////////////////////////
    // This one is special, since there is an ACAP_XFERMECH and an ICAP_XFERMECH.  This function is
    // for ACAP_XFERMECH.
    template <typename Container>
    cap_return_type set_audio_xfermech(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, ACAP_XFERMECH_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_alarms(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, CAP_ALARMS_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_alarmvolume(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, CAP_ALARMVOLUME_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_author(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, CAP_AUTHOR_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_autofeed(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, CAP_AUTOFEED_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_automaticcapture(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, CAP_AUTOMATICCAPTURE_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_automaticsensemedium(const Container& ct, const cap_setter_info& scType = cap_setter_info()) 
    { return set_caps_impl<Container, CAP_AUTOMATICSENSEMEDIUM_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_autoscan(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, CAP_AUTOSCAN_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_cameraenabled(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, CAP_CAMERAENABLED_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_cameraorder(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, CAP_CAMERAORDER_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_cameraside(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, CAP_CAMERASIDE_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_caption(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, CAP_CAPTION_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_clearpage(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, CAP_CLEARPAGE_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_deviceevent(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, CAP_DEVICEEVENT_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_devicetimedate(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, CAP_DEVICETIMEDATE_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_doublefeeddetection(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, CAP_DOUBLEFEEDDETECTION_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_doublefeeddetectionlength(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, CAP_DOUBLEFEEDDETECTIONLENGTH_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_doublefeeddetectionresponse(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, CAP_DOUBLEFEEDDETECTIONRESPONSE_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_doublefeeddetectionsensitivity(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, CAP_DOUBLEFEEDDETECTIONSENSITIVITY_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_duplexenabled(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, CAP_DUPLEXENABLED_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_endorser(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, CAP_ENDORSER_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_extendedcaps(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, CAP_EXTENDEDCAPS_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_feederalignment(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, CAP_FEEDERALIGNMENT_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_feederenabled(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, CAP_FEEDERENABLED_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_feederorder(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, CAP_FEEDERORDER_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_feederpocket(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, CAP_FEEDERPOCKET_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_feederprep(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, CAP_FEEDERPREP_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_feedpage(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, CAP_FEEDPAGE_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_indicators(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, CAP_INDICATORS_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_indicatorsmode(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, CAP_INDICATORSMODE_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_jobcontrol(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, CAP_JOBCONTROL_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_language(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, CAP_LANGUAGE_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_maxbatchbuffers(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, CAP_MAXBATCHBUFFERS_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_micrenabled(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, CAP_MICRENABLED_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_paperhandling(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, CAP_PAPERHANDLING_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_powersavetime(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, CAP_POWERSAVETIME_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_printer(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, CAP_PRINTER_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_printercharrotation(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, CAP_PRINTERCHARROTATION_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_printerenabled(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, CAP_PRINTERENABLED_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_printerfontstyle(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, CAP_PRINTERFONTSTYLE_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_printerindex(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, CAP_PRINTERINDEX_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_printerindexleadchar(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, CAP_PRINTERINDEXLEADCHAR_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_printerindexmaxvalue(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, CAP_PRINTERINDEXMAXVALUE_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_printerindexnumdigits(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, CAP_PRINTERINDEXNUMDIGITS_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_printerindexstep(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, CAP_PRINTERINDEXSTEP_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_printerindextrigger(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, CAP_PRINTERINDEXTRIGGER_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_printermode(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, CAP_PRINTERMODE_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_printerstring(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, CAP_PRINTERSTRING_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_printersuffix(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, CAP_PRINTERSUFFIX_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_printerverticaloffset(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, CAP_PRINTERVERTICALOFFSET_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_rewindpage(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, CAP_REWINDPAGE_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_segmented(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, CAP_SEGMENTED_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_sheetcount(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, CAP_SHEETCOUNT_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_timebeforefirstcapture(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, CAP_TIMEBEFOREFIRSTCAPTURE_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_timebetweencaptures(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, CAP_TIMEBETWEENCAPTURES_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_thumbnailsenabled(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, CAP_THUMBNAILSENABLED_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_xfercount(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, CAP_XFERCOUNT_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_autobright(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, ICAP_AUTOBRIGHT_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_autodiscardblankpages(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, ICAP_AUTODISCARDBLANKPAGES_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_automaticborderdetection(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, ICAP_AUTOMATICBORDERDETECTION_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_automaticcolorenabled(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, ICAP_AUTOMATICCOLORENABLED_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_automaticcolornoncolorpixeltype(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, ICAP_AUTOMATICCOLORNONCOLORPIXELTYPE_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_automaticdeskew(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, ICAP_AUTOMATICDESKEW_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_automaticlengthdetection(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, ICAP_AUTOMATICLENGTHDETECTION_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_automaticrotate(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, ICAP_AUTOMATICROTATE_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_autosize(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, ICAP_AUTOSIZE_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_barcodedetectionenabled(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, ICAP_BARCODEDETECTIONENABLED_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_barcodemaxretries(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, ICAP_BARCODEMAXRETRIES_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_barcodemaxsearchpriorities(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, ICAP_BARCODEMAXSEARCHPRIORITIES_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_barcodesearchmode(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, ICAP_BARCODESEARCHMODE_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_barcodesearchpriorities(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, ICAP_BARCODESEARCHPRIORITIES_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_barcodetimeout(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, ICAP_BARCODETIMEOUT_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_bitdepth(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, ICAP_BITDEPTH_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_bitdepthreduction(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, ICAP_BITDEPTHREDUCTION_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_bitorder(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, ICAP_BITORDER_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_bitordercodes(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, ICAP_BITORDERCODES_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_brightness(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, ICAP_BRIGHTNESS_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_ccittkfactor(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, ICAP_CCITTKFACTOR_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_colormanagementenabled(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, ICAP_COLORMANAGEMENTENABLED_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_compression(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, ICAP_COMPRESSION_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_contrast(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, ICAP_CONTRAST_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_custhalftone(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, ICAP_CUSTHALFTONE_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_exposuretime(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, ICAP_EXPOSURETIME_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_extimageinfo(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, ICAP_EXTIMAGEINFO_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_feedertype(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, ICAP_FEEDERTYPE_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_filmtype(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, ICAP_FILMTYPE_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_filter(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, ICAP_FILTER_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_flashused2(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, ICAP_FLASHUSED2_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_fliprotation(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, ICAP_FLIPROTATION_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_frames(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, ICAP_FRAMES_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_gamma(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, ICAP_GAMMA_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_halftones(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, ICAP_HALFTONES_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_highlight(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, ICAP_HIGHLIGHT_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_iccprofile(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, ICAP_ICCPROFILE_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_imagedataset(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, ICAP_IMAGEDATASET_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_imagefileformat(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, ICAP_IMAGEFILEFORMAT_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_imagefilter(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, ICAP_IMAGEFILTER_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_imagemerge(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, ICAP_IMAGEMERGE_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_imagemergeheightthreshold(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, ICAP_IMAGEMERGEHEIGHTTHRESHOLD_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_jpegpixeltype(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, ICAP_JPEGPIXELTYPE_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_jpegquality(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, ICAP_JPEGQUALITY_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_jpegsubsampling(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, ICAP_JPEGSUBSAMPLING_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_lampstate(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, ICAP_LAMPSTATE_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_lightpath(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, ICAP_LIGHTPATH_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_lightsource(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, ICAP_LIGHTSOURCE_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_maxframes(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, ICAP_MAXFRAMES_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_mirror(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, ICAP_MIRROR_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_noisefilter(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, ICAP_NOISEFILTER_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_orientation(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, ICAP_ORIENTATION_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_overscan(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, ICAP_OVERSCAN_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_patchcodedetectionenabled(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, ICAP_PATCHCODEDETECTIONENABLED_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_patchcodemaxretries(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, ICAP_PATCHCODEMAXRETRIES_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_patchcodemaxsearchpriorities(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, ICAP_PATCHCODEMAXSEARCHPRIORITIES_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_patchcodesearchmode(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, ICAP_PATCHCODESEARCHMODE_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_patchcodesearchpriorities(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, ICAP_PATCHCODESEARCHPRIORITIES_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_patchcodetimeout(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, ICAP_PATCHCODETIMEOUT_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_pixelflavor(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, ICAP_PIXELFLAVOR_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_pixelflavorcodes(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, ICAP_PIXELFLAVORCODES_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_pixeltype(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, ICAP_PIXELTYPE_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_planarchunky(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, ICAP_PLANARCHUNKY_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_rotation(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, ICAP_ROTATION_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_shadow(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, ICAP_SHADOW_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_supportedsizes(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, ICAP_SUPPORTEDSIZES_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_threshold(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, ICAP_THRESHOLD_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_tiles(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, ICAP_TILES_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_timefill(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, ICAP_TIMEFILL_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_undefinedimagesize(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, ICAP_UNDEFINEDIMAGESIZE_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_units(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, ICAP_UNITS_>(ct, scType); }
                    
    // This one is special, since there is an ACAP_XFERMECH and an ICAP_XFERMECH.  This function is
    // for ICAP_XFERMECH.
    template <typename Container>
    cap_return_type set_image_xfermech(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, ICAP_XFERMECH_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_xresolution(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, ICAP_XRESOLUTION_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_yresolution(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, ICAP_YRESOLUTION_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_xscaling(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, ICAP_XSCALING_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_yscaling(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, ICAP_YSCALING_>(ct, scType); }
                    
    template <typename Container>
    cap_return_type set_zoomfactor(const Container& ct, const cap_setter_info& scType = cap_setter_info())
    { return set_caps_impl<Container, ICAP_ZOOMFACTOR_>(ct, scType); }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
    size_t get_num_caps() const { return m_caps.size(); }
        
    size_t get_num_custom_caps() const
    {
        return std::count_if(m_caps.begin(), m_caps.end(), [&](const source_cap_info::value_type& vt)
        {
            return vt.first >= CAP_CUSTOMBASE;
        });
    }

    size_t get_num_extended_caps() const
    {
        std::vector<long> v;
        get_extended_caps(v);
        return v.size();
    }
        
    template <typename Container>
    void get_caps(Container& C) const
    {
        std::copy(m_caps.begin(), m_caps.end(), std::inserter(C, C.end()));
    }
        
    template <typename Container>
    void get_caps(Container* C) const
    {
        std::copy(m_caps.begin(), m_caps.end(), C);
    }
        
    template <typename Container>
    void get_custom_caps(Container& C) const
    {
        std::copy_if(m_caps.begin(), m_caps.end(), std::inserter(C, C.end()),
                        [&](const source_cap_info::value_type& vt) { return vt.first >= CAP_CUSTOMBASE; });
    }
        
    template <typename Container>
    void get_extended_caps(Container& C) const
    {
        get_extendedcaps(C);
    }
        
    const source_cap_info& get_source_cap_info() const;
    void remove_from_cache(const std::vector<int>& removed);
    void add_to_cache(const std::vector<int>& added);
    void clear_cache();
    source_cap_info::iterator begin();
    source_cap_info::iterator end();
    source_cap_info::const_iterator cbegin();
    source_cap_info::const_iterator cend();
        
    template <typename Container>
    cap_return_type set_cap_values(const Container& C, int capvalue,
                                    const cap_setter_info& scType = cap_setter_info()) const
    {
        const auto theSource = m_Source;
        if (!theSource)
            return {false, DTWAIN_ERR_BAD_SOURCE};
        if (!m_caps.empty() && m_caps.find(capvalue) == m_caps.end())
            return {false, DTWAIN_ERR_CAP_NO_SUPPORT};
        
        twain_array ta;
        const auto array_type = DTWAIN_GetCapArrayType(theSource, capvalue);
        twain_array_copy_traits::copy_to_twain_array(theSource, ta, capvalue, C);
        auto retval = DTWAIN_SetCapValues(theSource, capvalue, static_cast<LONG>(scType.get_setter_type()),
                                            ta.get_array());
        LONG last_error = DTWAIN_NO_ERROR;
        if (!retval)
            last_error = DTWAIN_GetLastError();
        return {retval ? true : false, last_error};
    }
        
    template <typename T, typename Container>
    cap_return_type set_cap_values(const Container& C, const cap_setter_info& scType = cap_setter_info()) const
    {
        static_assert(std::is_same<typename T::value_type, typename Container::value_type>::value == 1,
            "Capability type does not match container value type");
        return set_cap_values(C, T::cap_value, scType);
    }
        
    template <typename T, typename Container>
    cap_return_type set_cap_values(const Container* C, long nItems,
                                    const cap_setter_info& scType = cap_setter_info())
    {
        static_assert(std::is_same<typename T::value_type, typename std::remove_pointer<Container>::type>::value
            == 1, "Capability type does not match pointer value type");
        std::vector<typename T::value_type> tempV(C, C + nItems);
        auto retval = set_cap_values(tempV, scType);
        int last_error = DTWAIN_NO_ERROR;
        if (!retval)
            last_error = DTWAIN_GetLastError();
        return {retval, last_error};
    }
        
        
    ///////////////////////////////////////////////////////////////////////////////////////////////
    template <typename Container>
    cap_return_type get_cap_values(Container& C, int capvalue,
                                    const cap_getter_info& gcType = cap_getter_info()) const
    {
        if (!m_Source)
            return {false, DTWAIN_ERR_BAD_SOURCE};
        if (!m_caps.empty() && m_caps.find(capvalue) == m_caps.end())
            return {false, DTWAIN_ERR_CAP_NO_SUPPORT};
        
        const bool is_cache =
            (gcType.get_getter_type() == getcap_type::GET) &&
            (m_cacheable_set.find(capvalue) != m_cacheable_set.end());
        
        if (is_cache)
        {
            C.clear();
            if (copy_from_cache(C, capvalue))
                return {true, DTWAIN_NO_ERROR};
        }

        twain_array ta;
        bool retVal = DTWAIN_GetCapValues(m_Source, capvalue,
                                            static_cast<LONG>(gcType.get_getter_type()), ta.get_array_ptr()) != 0;
        if (!retVal)
            return {false, DTWAIN_GetLastError()};
        C.clear();
        const auto array_type = DTWAIN_GetCapArrayType(m_Source, capvalue);
        twain_array_copy_traits::copy_from_twain_array(ta, ta.get_count(), C);
        if (is_cache)
            copy_to_cache(C, capvalue);
        return {retVal, DTWAIN_NO_ERROR};
    }
        
    template <typename T, typename Container>
    cap_return_type get_cap_values(Container& C, const cap_getter_info& gcType = cap_getter_info()) const
    {
        return get_cap_values(C, T::cap_value, gcType);
    }
        
    template <typename Container>
    twain_container_type get_cap_container_type(int capvalue,
                                                const cap_getter_info& gcType = cap_getter_info()) const
    {
        const auto theSource = m_Source;
        if (!m_caps.empty() && m_caps.find(capvalue) == m_caps.end())
            return twain_container_type::CONTAINER_INVALID;
        auto container_type = DTWAIN_GetCapContainer(theSource->get_source(), capvalue,
                                                        static_cast<LONG>(gcType.get_getter_type()));
        return static_cast<twain_container_type>(container_type);
    }
        
    template <typename T>
    twain_container_type get_cap_container_type(const cap_getter_info& gcType) const
    {
        return get_cap_values(T::cap_value, gcType);
    }
        
    template <typename Container>
    twain_container_type get_cap_container_type(int capvalue,
                                                const cap_setter_info& scType = cap_setter_info()) const
    {
        const auto theSource = m_Source;
        if (!m_caps.empty() && m_caps.find(capvalue) == m_caps.end())
            return twain_container_type::CONTAINER_INVALID;
        auto container_type = DTWAIN_GetCapContainer(theSource, capvalue,
                                                        static_cast<LONG>(scType.get_setter_type()));
        return static_cast<twain_container_type>(container_type);
    }
        
    template <typename T>
    twain_container_type get_cap_container_type(const cap_setter_info& scType) const
    {
        return get_cap_values(T::cap_value, scType);
    }
        
    capability_interface(twain_source* s);
    capability_interface(twain_source& s);
    capability_interface();
    bool attach(DTWAIN_SOURCE s);
    void detach();

    template <typename T>
    bool is_cap_supported() const
    {
        static_assert(T::cap_value != 0, "Invalid capability type used");
        static_assert(T::template cap_value<
            CAP_CUSTOMBASE,
            "Cannot use this function to test custom cap support.  Use is_cap_supported(int) overload instead");
        return is_cap_supported(T::cap_value);
    }
        
    bool is_cap_supported(int capValue) const
    {
        return m_caps.find(capValue) != m_caps.end();
    }
};

class capability_listener
{
    public:
        virtual void capvaluegetstart(twain_source&, int, const capability_interface::cap_getter_info&) {}
        virtual void capvaluegetend(twain_source&, int, const capability_interface::cap_getter_info&, bool) {}
        virtual void capvaluesetstart(twain_source&, int, const capability_interface::cap_setter_info&) {}
        virtual void capvaluesetend(twain_source&, int, const capability_interface::cap_setter_info&, bool) {}
        virtual ~capability_listener() {}
};    

