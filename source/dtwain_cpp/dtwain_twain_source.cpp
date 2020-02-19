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
#include <functional>
#include <unordered_map>

namespace dynarithmic
{
    namespace twain
    {
        twain_source::twain_source(DTWAIN_SOURCE theSource) : m_theSource(theSource), m_bIsSelected(false),
                                                              m_bCloseable(false),
                                                              m_capability_info(new capability_interface)
        {
            if (m_theSource)
                attach(m_theSource);
        }

        twain_source::twain_source(twain_source&& rhs) noexcept
        {
            swap(*this, rhs);
            rhs.m_theSource = nullptr;
        }

        twain_source& twain_source::operator=(twain_source&& rhs) noexcept
        {
			if (this == &rhs)
				return *this;
            swap(*this, rhs);
            m_capability_info = std::move(rhs.m_capability_info);
            rhs.m_theSource = nullptr;
            if (rhs.m_capability_info)
                m_capability_listener = std::move(rhs.m_capability_listener);
            else
                m_capability_listener.reset();
            rhs.m_capability_listener = nullptr;
            return *this;
        }

        twain_source::~twain_source()
        {
            if (m_theSource)
            {
                while (DTWAIN_IsSourceAcquiring(m_theSource))
                    ;
                DTWAIN_CloseSource(m_theSource);
            }
        }

        void twain_source::swap(twain_source& left, twain_source& right) noexcept
        {
            std::swap(left.m_theSource, right.m_theSource);
            std::swap(left.m_bIsSelected, right.m_bCloseable);
            std::swap(left.m_sourceInfo, right.m_sourceInfo);
            std::swap(left.m_bCloseable, right.m_bCloseable);
            std::swap(left.m_acquire_characteristics, right.m_acquire_characteristics);
            std::swap(left.m_buffered_info, right.m_buffered_info);
            std::swap(left.m_cap_status_info_map, right.m_cap_status_info_map);
            std::swap(left.m_caps_to_test, right.m_caps_to_test);
            std::swap(left.m_twain_printer_info, right.m_twain_printer_info);
        }

        void twain_source::attach(DTWAIN_SOURCE source)
        {
            m_theSource = source;
            if (source)
            {
                get_source_info_internal();
                m_capability_info->attach(*this);
                get_source_basic_cap_info(IS_SUPPORTED);
                m_twain_printer_info.attach(*this);
                m_bIsSelected = true;
            }
            else
                m_bIsSelected = false;
        }

        void twain_source::detach()
        {
            m_theSource = nullptr;
            m_bIsSelected = false;
            m_capability_info->detach();
        }

        void twain_source::invoke_callback(int which, int capValue, const capability_interface::cap_getter_info& gi, bool val)
        {
            if (m_capability_listener)
            {
                switch (which)
                {
                    case capability_interface::CAP_CALLBACK_GET_BEGIN:
                        m_capability_listener->capvaluegetstart(*this, capValue, gi);
                        break;
                    case capability_interface::CAP_CALLBACK_GET_END:
                        m_capability_listener->capvaluegetend(*this, capValue, gi, val);
                        break;
                }
            }
        }

        void twain_source::invoke_callback(int which, int capValue, const capability_interface::cap_setter_info& si, bool val)
        {
            if (m_capability_listener)
            {
                switch (which)
                {
                    case capability_interface::CAP_CALLBACK_SET_BEGIN:
                        m_capability_listener->capvaluesetstart(*this, capValue, si);
                        break;
                    case capability_interface::CAP_CALLBACK_SET_END:
                        m_capability_listener->capvaluesetend(*this, capValue, si, val);
                        break;
                }
            }
        }


        void twain_source::get_source_basic_cap_info(int get_info)
        {
            static auto fnIsJobControl = [](long val, const twain_source&)
            {
                static std::unordered_set<jobcontrol_type> test = { jobcontrol_type::include_separator,
                    jobcontrol_type::include_separator_stop,
                    jobcontrol_type::exclude_separator,
                    jobcontrol_type::exclude_separator_stop };
                if (test.count(static_cast<jobcontrol_type>(val)))
                    return val;
                return -1L;
            };
            static auto fnreturner = [](long, const twain_source&) {return 1; };
            static high_level_cap_map caps_to_test =
            { { high_level_cap::duplexenabled, std::make_tuple(CAP_DUPLEX, CAP_DUPLEXENABLED, fnreturner)},
              { high_level_cap::feederenabled, std::make_tuple(CAP_FEEDERENABLED, CAP_FEEDERENABLED, fnreturner)},
              { high_level_cap::feederloaded,  std::make_tuple(CAP_FEEDERLOADED, CAP_FEEDERLOADED, fnreturner)},
              { high_level_cap::paperdetectable, std::make_tuple(CAP_PAPERDETECTABLE, CAP_PAPERDETECTABLE, fnreturner)},
              { high_level_cap::filmscan, std::make_tuple(ICAP_LIGHTPATH, ICAP_LIGHTPATH, [](long, const twain_source&) { return DTWAIN_LP_TRANSMISSIVE; })},
              { high_level_cap::indicators, std::make_tuple(CAP_INDICATORS, boost::tribool::indeterminate_value, fnreturner)} ,
              { high_level_cap::uionly, std::make_tuple(CAP_ENABLEDSUIONLY, boost::tribool::indeterminate_value, fnreturner)},
              { high_level_cap::customdata, std::make_tuple(CAP_CUSTOMDSDATA, boost::tribool::indeterminate_value, fnreturner)},
              { high_level_cap::orientation, std::make_tuple(ICAP_ORIENTATION, boost::tribool::indeterminate_value, fnreturner)},
              { high_level_cap::rotation, std::make_tuple(ICAP_ROTATION, boost::tribool::indeterminate_value, fnreturner)},
              { high_level_cap::shadow, std::make_tuple(ICAP_SHADOW, boost::tribool::indeterminate_value, fnreturner) },
              { high_level_cap::lightsource, std::make_tuple(ICAP_LIGHTSOURCE, boost::tribool::indeterminate_value, fnreturner)},
              { high_level_cap::printer, std::make_tuple(CAP_PRINTER, CAP_PRINTERENABLED, fnreturner) },
              { high_level_cap::printercharrotation, std::make_tuple(CAP_PRINTERCHARROTATION, boost::tribool::indeterminate_value, fnreturner) },
              { high_level_cap::printerstring, std::make_tuple(CAP_PRINTERSTRING, boost::tribool::indeterminate_value, fnreturner) },
              { high_level_cap::printersuffix, std::make_tuple(CAP_PRINTERSUFFIX, boost::tribool::indeterminate_value, fnreturner) },
              { high_level_cap::printerindex, std::make_tuple(CAP_PRINTERINDEX, boost::tribool::indeterminate_value, fnreturner) },
              { high_level_cap::printerindexstep, std::make_tuple(CAP_PRINTERINDEXSTEP, boost::tribool::indeterminate_value, fnreturner) },
              { high_level_cap::printerindexmaxvalue, std::make_tuple(CAP_PRINTERINDEXMAXVALUE, boost::tribool::indeterminate_value, fnreturner) },
              { high_level_cap::printerindexnumdigits, std::make_tuple(CAP_PRINTERINDEXNUMDIGITS, boost::tribool::indeterminate_value, fnreturner) },
              { high_level_cap::printerindexleadchar, std::make_tuple(CAP_PRINTERINDEXLEADCHAR, boost::tribool::indeterminate_value, fnreturner) },
              { high_level_cap::printermode, std::make_tuple(CAP_PRINTERMODE, boost::tribool::indeterminate_value, fnreturner) },
              { high_level_cap::printerverticaloffset, std::make_tuple(CAP_PRINTERVERTICALOFFSET, boost::tribool::indeterminate_value, fnreturner) },
              { high_level_cap::printerindextrigger, std::make_tuple(CAP_PRINTERINDEXTRIGGER, boost::tribool::indeterminate_value, fnreturner) },
              { high_level_cap::printerstringpreview, std::make_tuple(CAP_PRINTERSTRINGPREVIEW, boost::tribool::indeterminate_value, fnreturner) },
              { high_level_cap::brightness, std::make_tuple(ICAP_BRIGHTNESS, boost::tribool::indeterminate_value, fnreturner)},
              { high_level_cap::contrast, std::make_tuple(ICAP_CONTRAST, boost::tribool::indeterminate_value, fnreturner)},
              { high_level_cap::threshold, std::make_tuple(ICAP_THRESHOLD, boost::tribool::indeterminate_value, fnreturner) },
              { high_level_cap::gamma, std::make_tuple(ICAP_GAMMA, boost::tribool::indeterminate_value, fnreturner)},
              { high_level_cap::highlight, std::make_tuple(ICAP_HIGHLIGHT, boost::tribool::indeterminate_value, fnreturner) },
              { high_level_cap::deviceonline, std::make_tuple(CAP_DEVICEONLINE, CAP_DEVICEONLINE, fnreturner)},
              { high_level_cap::automaticdeskew, std::make_tuple(ICAP_AUTOMATICDESKEW, ICAP_AUTOMATICDESKEW, fnreturner)},
              { high_level_cap::automaticrotate, std::make_tuple(ICAP_AUTOMATICROTATE, ICAP_AUTOMATICROTATE, fnreturner) },
              { high_level_cap::autobright, std::make_tuple(ICAP_AUTOBRIGHT, ICAP_AUTOBRIGHT, fnreturner) },
              { high_level_cap::overscan, std::make_tuple(ICAP_OVERSCAN, ICAP_OVERSCAN, fnreturner) },
              { high_level_cap::jobcontrol, std::make_tuple(CAP_JOBCONTROL, CAP_JOBCONTROL, fnIsJobControl)}
            };

            m_caps_to_test = caps_to_test;

            if (m_theSource && (get_info & IS_SUPPORTED))
            {
                std::for_each(caps_to_test.begin(), caps_to_test.end(), [&](const high_level_cap_map::value_type& ct)
                                    { get_one_source_basic_cap_info(IS_SUPPORTED, ct.first); });
            }

            if (m_theSource && (get_info & IS_ENABLED))
            {
                std::for_each(caps_to_test.begin(), caps_to_test.end(), [&](const high_level_cap_map::value_type& ct)
                { get_one_source_basic_cap_info(IS_ENABLED, ct.first); });
            }
        }

        void twain_source::get_one_source_basic_cap_info(int get_info, high_level_cap ct) const
        {
            bool is_supported = true;
            if (m_theSource && (get_info & IS_SUPPORTED))
            {
                auto iter = m_caps_to_test.find(ct);
                if (iter != m_caps_to_test.end())
                {
                    auto tupleval = iter->second;
                    auto iter2 = m_cap_status_info_map.insert({ ct, cap_status_info() }).first;
                    is_supported = m_capability_info->is_cap_supported(std::get<0>(tupleval));
                    iter2->second.set_supported(m_capability_info->is_cap_supported(is_supported));
                }
            }

            if (is_supported && m_theSource && (get_info & IS_ENABLED))
            {
                auto iter = m_caps_to_test.find(ct);
                if (iter != m_caps_to_test.end())
                {
                    auto tupleval = iter->second;
                    std::vector<long> vl;
                    auto gInfo = capability_interface::get_current();
                    if (std::get<1>(tupleval) != boost::tribool::indeterminate_value)
                    {
                        auto iter2 = m_cap_status_info_map.insert({ ct, cap_status_info() }).first;
                        auto retval = m_capability_info->get_cap_values(vl, std::get<1>(tupleval), gInfo);
                        if (retval.first)
                        {
                            auto fnToCall = std::get<2>(tupleval);
                            m_cap_status_info_map.insert({ ct, cap_status_info() }).first->second.set_enabled(!vl.empty() &&
                                vl.front() == fnToCall(vl.front(), *this));
                        }
                    }
                }
            }
        }

        void twain_source::get_source_info_internal()
        {
                const auto p_id = static_cast<TW_IDENTITY*>(DTWAIN_GetSourceID(m_theSource));
                m_sourceInfo = *p_id;
        }

#define GET_SET_CAP_VALUE(x,y) \
            val = static_cast<decltype(val)>(ac.get_##x()); \
            if (val != static_cast<decltype(val)>(acquire_characteristics::default_##x)) {\
                twain_std_array<decltype(val), 1> dValues; \
                dValues[0] = val; \
                m_capability_info->set_##y(dValues); }

        void twain_source::prepare_halftones()
        {
            auto& ac = m_acquire_characteristics;
            auto ht = ac.get_halftone();
            if (ht.empty())
                return;
            const auto bret = m_capability_info->set_bitdepthreduction(std::array<long, 1>{1});
            if (!bret.first)
                return;
            m_capability_info->set_halftones(std::array<std::string, 1>{ht});
        }

        void twain_source::prepare_acquisition()
        {
            acquire_characteristics& ac = m_acquire_characteristics;
            std::vector<long> vValues = {ac.is_duplex_mode()};
            m_capability_info->set_duplexenabled(vValues);

            {
                double val = 0;
                GET_SET_CAP_VALUE(rotation, rotation)
                GET_SET_CAP_VALUE(contrast, contrast)
                GET_SET_CAP_VALUE(brightness, brightness)
                GET_SET_CAP_VALUE(resolution, xresolution)
                GET_SET_CAP_VALUE(resolution, yresolution)
                GET_SET_CAP_VALUE(gamma, gamma)
                GET_SET_CAP_VALUE(shadow, shadow)
                GET_SET_CAP_VALUE(threshold, threshold)
                GET_SET_CAP_VALUE(highlight, highlight)
            }

            vValues[0] = ac.get_use_feeder() ? 1 : 0;
            m_capability_info->set_feederenabled(vValues);
            if (ac.is_use_high_speed())
                prepare_high_speed();
            {
                long val = 0;
                GET_SET_CAP_VALUE(orientation, orientation)
                GET_SET_CAP_VALUE(measure_unit, units)
                GET_SET_CAP_VALUE(paper_size, supportedsizes)
                GET_SET_CAP_VALUE(filmscan_mode, lightpath)
                GET_SET_CAP_VALUE(indicators_mode, indicators)
                GET_SET_CAP_VALUE(autodeskew_mode, automaticdeskew)
                GET_SET_CAP_VALUE(autobright_mode, autobright)
                GET_SET_CAP_VALUE(autorotate_mode, automaticrotate)
                GET_SET_CAP_VALUE(overscan_mode, overscan)
                if (get_cap_status(high_level_cap::jobcontrol).is_supported())
                    DTWAIN_SetJobControl(m_theSource, static_cast<LONG>(ac.get_jobcontrol_type()), TRUE);
            }

            DTWAIN_ARRAY area = DTWAIN_ArrayCreate(DTWAIN_ARRAYFLOAT, 4);
            twain_array arr(area);
            if (ac.is_use_area())
            {
                double* buffer = arr.get_buffer<double>();
                auto& aoi = ac.get_acquire_area();
                buffer[0] = aoi.left;
                buffer[1] = aoi.top;
                buffer[2] = aoi.right;
                buffer[3] = aoi.bottom;
                DTWAIN_SetAcquireArea(m_theSource, DTWAIN_AREASET, area, NULL);
            }
            else
                DTWAIN_SetAcquireArea(m_theSource, DTWAIN_AREARESET, area, NULL);
            DTWAIN_SetAcquireImageNegative(m_theSource, ac.is_negate_images() ? TRUE : FALSE);
            auto& blank_handler = ac.get_blank_image_handler();
            DTWAIN_SetBlankPageDetection(m_theSource, blank_handler.threshold, 
                                        static_cast<LONG>(blank_handler.discard_option), 
                                        static_cast<LONG>(blank_handler.enabled));
            auto& multisave_info = ac.get_multipage_save_info();
            DTWAIN_SetMultipageScanMode(m_theSource, 
                static_cast<LONG>(multisave_info.get_save_mode())
                | 
                (multisave_info.is_save_incomplete() ? static_cast<LONG>(multipage_save_mode::save_incomplete) : 0));

            DTWAIN_SetMaxAcquisitions(m_theSource, ac.get_max_acquisitions());
            // set the imprinter stuff
            if (get_cap_status(high_level_cap::printer).is_supported() &&
                get_cap_status(high_level_cap::printerstring).is_supported())
            { 
                std::string istring = ac.get_imprinter_string();
                if (!istring.empty())
                {
                    if (::DTWAIN_EnablePrinter(m_theSource, 1))
                    {
                        get_one_source_basic_cap_info(IS_ENABLED, high_level_cap::printer);
                        auto arrType = DTWAIN_ArrayCreate(DTWAIN_ARRAYSTRING, 1);
                        twain_array arr(arrType);
                        DTWAIN_ArraySetAtANSIString(arrType, 0, istring.c_str());
                        ::DTWAIN_SetPrinterStrings(m_theSource, arrType, NULL);
                    }
                }
            }
            prepare_halftones();
            get_source_basic_cap_info(IS_ENABLED);
            set_pdf_options();
        }

        void twain_source::set_pdf_options()
        {
            auto source = get_source();

            // set the PDF file properties
            pdf_options& po = this->get_acquire_characteristics().get_pdf_options();
            ::DTWAIN_SetPDFCreatorA(source, po.get_creator().c_str());
            ::DTWAIN_SetPDFTitleA(source, po.get_title().c_str());
            ::DTWAIN_SetPDFProducerA(source, po.get_creator().c_str());
            ::DTWAIN_SetPDFAuthorA(source, po.get_author().c_str());
            ::DTWAIN_SetPDFSubjectA(source, po.get_subject().c_str());
            ::DTWAIN_SetPDFKeywordsA(source, po.get_keywords().c_str());
            ::DTWAIN_SetPDFASCIICompression(source, po.is_use_ASCII());
            ::DTWAIN_SetPDFOrientation(source, static_cast<LONG>(po.get_orientation()));

            // Set PDF page size
            auto& pagesizeopts = po.get_page_size_options();
            bool custom_used = pagesizeopts.is_custom_size_used();
            double width = 0.0;
            double height = 0.0;
            if (custom_used)
            {
                auto pr = pagesizeopts.get_custom_size();
                width = pr.first;
                height = pr.second;
            }
            ::DTWAIN_SetPDFPageSize(source,static_cast<LONG>(po.get_page_size_options().get_page_size()), width, height);

            // Set PDF page scale
            auto& pagescaleopts = po.get_page_scale_options();
            custom_used = pagescaleopts.is_custom_scale_used();
            double xscale = 0.0;
            double yscale = 0.0;
            if ( custom_used)
            {
                auto pr = pagescaleopts.get_custom_scale();
                xscale = pr.first;
                yscale = pr.second;
            }
            ::DTWAIN_SetPDFPageScale(source, static_cast<LONG>(po.get_page_scale_options().get_page_scale()), xscale, yscale);

            // Set encryption options
            auto& encrypt_opts = po.get_encryption_options();
            if (encrypt_opts.get_use_encryption())
            {
                ::DTWAIN_SetPDFEncryptionA(source,1,encrypt_opts.get_user_password().c_str(),
                                                          encrypt_opts.get_owner_password().c_str(),
                                                          encrypt_opts.get_all_permissions(),
                                                          encrypt_opts.get_use_strong_encryption());
            }
        }

        twain_source::acquire_return_type twain_source::acquire_to_file(transfer_type transtype)
        {
            acquire_characteristics& ac = m_acquire_characteristics;
            LONG dtwain_transfer_type = DTWAIN_USENATIVE;
            if (transtype == transfer_type::file_using_buffered)
                dtwain_transfer_type = DTWAIN_USEBUFFERED;
            dtwain_transfer_type |= static_cast<LONG>(ac.get_file_transfer_flags());

            const auto ft = ac.get_file_type();
            if (!file_type_info::is_universal_support(ft))
            {
                dtwain_transfer_type |= DTWAIN_USESOURCEMODE;
                // Set the compression type
                DTWAIN_SetCompressionType(m_theSource, static_cast<LONG>(ac.get_compression_type()), 1);
            }
            // check for auto increment
            filename_increment_rules& inc = ac.get_filename_increment_rules();
            DTWAIN_SetFileAutoIncrement(m_theSource, inc.get_increment(), inc.is_reset_count() ? TRUE : FALSE,
                                        inc.is_enabled() ? TRUE : FALSE);
            DTWAIN_EnableMsgNotify(1);
            auto retval = DTWAIN_AcquireFileA(m_theSource, ac.get_filename_pattern().c_str(),
                                              static_cast<LONG>(ac.get_file_type()), dtwain_transfer_type,
                                              static_cast<LONG>(ac.get_color_type()),
                                              static_cast<LONG>(ac.get_max_pages()), ac.is_show_ui(),
                                              ac.is_auto_close_source(),
                                              nullptr) != 0;
            return {retval?acquire_return_code::acquire_ok:acquire_return_code::acquire_cancelled, image_handler(false)};
        }

        twain_source::acquire_return_type twain_source::acquire_to_image_handles(transfer_type transtype)
        {
            image_handler ih;
            acquire_characteristics& ac = m_acquire_characteristics;
            if (transtype == transfer_type::image_native || transtype == transfer_type::image_buffered)
            {
                twain_array images(DTWAIN_CreateAcquisitionArray());
                bool retval = false;
                if (transtype == transfer_type::image_native)
                    retval = DTWAIN_AcquireNativeEx(m_theSource, static_cast<LONG>(ac.get_color_type()),
                                                    static_cast<LONG>(ac.get_max_pages()), ac.is_show_ui(),
                                                    ac.is_auto_close_source(), images.get_array(), nullptr) != 0;
                else
                    retval = DTWAIN_AcquireBufferedEx(m_theSource, static_cast<LONG>(ac.get_color_type()),
                                                      static_cast<LONG>(ac.get_max_pages()), ac.is_show_ui(),
                                                      ac.is_auto_close_source(), images.get_array(), nullptr) != 0;
                if (retval)
                {
                    const auto acq_count = images.get_count();
                    for (int i = 0; i < acq_count; ++i)
                    {
                        twain_array img_array(DTWAIN_GetAcquiredImageArray(images.get_array(), i));
                        ih.add_new_page();
                        const auto image_count = img_array.get_count();
                        const HANDLE* handleBuffer = reinterpret_cast<HANDLE *>(img_array.get_buffer<HANDLE>());
                        for (auto j = 0; j < image_count; ++j)
                            ih.push_back_image(handleBuffer[j]);
                    }
                }
                return {retval?acquire_return_code::acquire_ok : acquire_return_code::acquire_cancelled, ih};
            }
            return {acquire_return_code::acquire_cancelled, image_handler(false)};
        }

        void twain_source::wait_for_feeder(bool& status) const
        {
            // check for feeder stuff here
            bool isfeedersupported = get_cap_status(high_level_cap::feederenabled).is_supported();
            if (!isfeedersupported)
            {
                status = true;
                return;
            }
            twain_std_array<capability_interface::feederenabled_type, 1> arr;
            arr[0] = 1;
            m_capability_info->set_feederenabled(arr);
            if ( !get_cap_status(high_level_cap::feederenabled).is_enabled())
            {
                status = true;
                return;
            }

            bool ispaperdetectable = get_cap_status(high_level_cap::paperdetectable).is_supported();
            if (!ispaperdetectable)
            {
                status = true;
                return;
            }

            bool isfeederloaded = get_cap_status(high_level_cap::feederloaded).is_supported();
            if (!isfeederloaded)
            {
                status = true;
                return;
            }

            auto timeoutval = m_acquire_characteristics.get_feeder_timeout();

            twain_timer theTimer;

            // loop until feeder is loaded
            while (!get_cap_status(high_level_cap::feederloaded).is_enabled())
            {
                if (timeoutval != -1)
                {
                    if (theTimer.elapsed() > timeoutval)
                    {
                        status = false;
                        return;
                    }
                }
            }
            status = true;
            return;
        }

        twain_source::acquire_return_type twain_source::acquire()
        {
            prepare_acquisition();
            bool use_flatbed = m_acquire_characteristics.get_use_feeder_or_flatbed();
            bool fstatus = true;
            if (m_acquire_characteristics.get_wait_for_feeder_loaded() || use_flatbed)
            {
                fstatus = false;
                wait_for_feeder(fstatus);

                // timed out waiting for the feeder to be loaded, or device doesn't support feeder
                if (!fstatus && !use_flatbed)
                    return acquire_return_type{ acquire_return_code::acquire_timeout, image_handler(false) };
            }

            // if we got a timeout on the feeder, but use flatbed as backup, disable the feeder and use the flatbed
            if (!fstatus && use_flatbed)
            {
                DTWAIN_EnableFeeder(m_theSource, FALSE);
                fstatus = true;
            }

            if (fstatus)
            {
                const auto transtype = m_acquire_characteristics.get_transfer_type();
                if (transtype == transfer_type::file_using_native ||
                    transtype == transfer_type::file_using_buffered ||
                    transtype == transfer_type::file_using_source)
                    return acquire_to_file(transtype);
                return acquire_to_image_handles(transtype);
            }
            return acquire_return_type{ acquire_return_code::acquire_cancelled, image_handler(false) };
        }

        twain_source::byte_array twain_source::get_custom_data() const
        {
            byte_array ba;
            if (!get_cap_status(high_level_cap::customdata).is_supported())
                return ba;
            LONG actual_size;
            byte_array str;
            auto retval = DTWAIN_GetCustomDSData(m_theSource, nullptr, 0, &actual_size, DTWAINGCD_RETURNHANDLE);
            if (retval)
            {
                str.resize(actual_size + 1);
                retval = DTWAIN_GetCustomDSData(m_theSource, reinterpret_cast<LPBYTE>(&str[0]), actual_size,
                                                &actual_size, DTWAINGCD_COPYDATA);
            }
            return str;
        }

        bool twain_source::set_custom_data(const byte_array& s) const
        {
            if (!get_cap_status(high_level_cap::customdata).is_supported())
                return false;
            return DTWAIN_SetCustomDSData(m_theSource, NULL, reinterpret_cast<LPCBYTE>(&s[0]), s.size(),
                                          DTWAINSCD_USEDATA)
                       ? true
                       : false;
        }

        bool twain_source::open()
        {
            if (m_theSource)
            {
                if (DTWAIN_OpenSource(m_theSource))
                {
                    get_source_info_internal();
                    attach(m_theSource);
                    return true;
                }
            }
            return false;
        }

        bool twain_source::is_open() const
        {
            if (m_theSource)
                return DTWAIN_IsSourceOpen(m_theSource) ? true : false;
            return false;
        }

        bool twain_source::is_selected() const
        {
            return m_bIsSelected;
        }

        bool twain_source::is_closeable() const
        {
            return m_bCloseable;
        }

        bool twain_source::close()
        {
            if (m_theSource)
            {
                bool retVal = DTWAIN_CloseSource(m_theSource) ? true : false;
                m_theSource = nullptr;
                return retVal;
            }
            return false;
        }

        image_information twain_source::get_image_information() const
        {
            DTWAIN_FLOAT xRes, yRes;
            LONG width, length, numsamples;
            DTWAIN_ARRAY bitspersample;
            LONG bitsperpixel;
            LONG planar;
            LONG pixeltype, compression;
            image_information iinfo;
            if (DTWAIN_GetImageInfo(m_theSource, &xRes, &yRes, &width, &length, &numsamples,
                                    &bitspersample, &bitsperpixel, &planar, &pixeltype, &compression))
            {
                iinfo.x_resolution = xRes;
                iinfo.y_resolution = yRes;
                iinfo.bitsPerPixel = bitsperpixel;
                iinfo.compression = compression;
                iinfo.length = length;
                iinfo.pixelType = pixeltype;
                iinfo.planar = planar ? true : false;
                iinfo.width = width;
                iinfo.numsamples = numsamples;
                twain_array arr(bitspersample);
                twain_array_copy_traits::copy_from_twain_array(arr, iinfo.bitsPerSample);
            }
            return iinfo;
        }

        bool twain_source::can_use_high_speed() const
        {
            std::vector<long> v, v2;
            m_capability_info->get_autoscan(v);
            m_capability_info->get_maxbatchbuffers(v2);
            return (!v.empty() && !v2.empty() && v.front() == 1);
        }

        const twain_source::cap_status_info & twain_source::get_cap_status(high_level_cap cap) const
        {
            if (m_theSource)
            {
                get_one_source_basic_cap_info(IS_ENABLED | IS_SUPPORTED, cap);
                return m_cap_status_info_map.find(cap)->second;
            }
            return no_status_info;
        }

        bool twain_source::prepare_high_speed()
        {
            if (!can_use_high_speed())
                return false;
            // check if autoscan is available, if so set it
            std::vector<capability_interface::autoscan_type> autoscan = {1};
            auto retval = m_capability_info->set_autoscan(autoscan);
            if (!retval.first)
                return false;
            std::vector<capability_interface::maxbatchbuffers_type> batchbuffers;
            retval = m_capability_info->get_maxbatchbuffers(batchbuffers);
            if (!retval.first || batchbuffers.empty())
                return false;
            auto topvalue = batchbuffers.back();
            auto ContainerType = DTWAIN_GetCapContainer(m_theSource, CAP_MAXBATCHBUFFERS, DTWAIN_CAPGET);
            if (ContainerType == DTWAIN_CONTRANGE)
                topvalue = batchbuffers[1];
            else
            {
                std::sort(batchbuffers.begin(), batchbuffers.end());
                topvalue = batchbuffers.back();
            }
            batchbuffers[0] = topvalue;
            batchbuffers.resize(1);
            return m_capability_info->set_maxbatchbuffers(batchbuffers).first;
        }

        HANDLE twain_source::get_current_image()
        {
            return ::DTWAIN_GetCurrentAcquiredImage(get_source());
        }



    }
}
