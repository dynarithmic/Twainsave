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
#ifndef DTWAIN_TWAIN_SOURCE_HPP
#define DTWAIN_TWAIN_SOURCE_HPP

#include <utility>
#include <unordered_map>
#include <functional>
#include <algorithm>
#include <chrono>
#include <thread>
#include <tuple>
#include <numeric>

#include <dynarithmic/twain/acquire_characteristics.hpp>
#include <dynarithmic/twain/capability_interface.hpp>
#include <dynarithmic/twain/imagehandler/image_handler.hpp>
#include <dynarithmic/twain/info/buffered_transfer_info.hpp>
#include <dynarithmic/twain/info/file_transfer_info.hpp>
#include <dynarithmic/twain/info/info_base.hpp>
#include <dynarithmic/twain/info/paperhandling_info.hpp>
#include <dynarithmic/twain/options/buffered_transfer_options.hpp> 
#include <dynarithmic/twain/options/options_base.hpp>
#include <dynarithmic/twain/options/ui_options.hpp>
#include <dynarithmic/twain/session/twain_session.hpp>
#include <dynarithmic/twain/source/twain_source_base.hpp>
#include <dynarithmic/twain/twain_values.hpp>
#include <dynarithmic/twain/types/twain_listener.hpp>
#include <dynarithmic/twain/types/twain_timer.hpp>

namespace dynarithmic {
namespace twain {
    class twain_session;

    /**
        The twain_source class is the main class that represents a TWAIN device (the DTWAIN_SOURCE when referring to the underlying DTWAIN API).  The device has to be selected first 
        before any image acquisitions are done by the device.<br><br>
        There are 3 ways to select a device :

        <ul>
            <li>1. Display a TWAIN "Select Source" dialog box, which allows the user to select the device.</li>
            <li>2. Select the default device automatically.The default device is usually the last device that was selected in a previous TWAIN session.</li>
            <li>3. Select the device by using the product name of the device.</li>
        </ul>
            Items 2 and 3 bypass the "Select Source" dialog box.

            To select a source, a twain_session must be instantiated, with a call to twain_session::select_source being invoked.  The return value of the select_source() function can be used to instantiate a twain_source object.<p>
            The twain_source also contains a list of capabilities that the source supports. These capabilities include things such as setting or getting the resolution, color type, etc.The list of capabilities and the interface to these capabilities(where you can get / set these capabilities) is available using the capability_interface object that the twain_source keeps internally.<p>
            When acquiring images, the acquire_characteristics class is used to prepare the acquisition.Once the acquisition is prepared, the twain_source::acquire() is invoked to start the acquisition process.<p>
            The twain_source class uses RAII (Resource Acquisition Is Initialization) techniques, meaning that when a twain_source object goes out of scope, the destructor for the twain_source will automatically close the source.Note that if the TWAIN device is still active(acquiring images), the twain_source destructor will not return until all the TWAIN source is closed.<p><p>
            Below is the simplest example of starting a TWAIN session, choosing a device and acquiring to a BMP file:

     *   \code {.cpp}
     *   #include <dynarithmic\twain\twain_session.hpp>
     *   #include <dynarithmic\twain\twain_source.hpp>
     *
     *   using namespace dynarithmic::twain;
     *   int main()
         {
     *       twain_session session;
     *       if ( session.start() )
     *       {
     *          twain_source source = twain_session::select();
     *          if ( source.is_open() )
     *              source.acquire();
     *       }
     *   } // session will close automatically
     *   \endcode
     *
     
         Copy Semantics for **twain_source**:
         <ul>
             <li>Move Constructible</li>
             <li>Move Assignable</li>
         </ul>
      */

    class twain_source : public twain_source_base
    {
    public:
        using acquire_return_type = std::pair<int32_t, twain_array>;
        using byte_array = std::vector<BYTE>;
        static constexpr int ACQUIRE_RETURN = 1;
        static constexpr int IMAGE_HANDLER = 2;
        static constexpr int TWAIN_ARRAY = 3;

        static constexpr int32_t acquire_ok = DTWAIN_NO_ERROR;
        static constexpr int32_t acquire_timeout = 1;
        static constexpr int32_t acquire_canceled = 2;

    private:
        const int IS_SUPPORTED = 1;
        const int IS_ENABLED = 2;
        typedef LONG(DLLENTRY_DEF* infofn)(DTWAIN_SOURCE, LPSTR, LONG);
        typedef twain_app_info& (twain_app_info::*appfn)(const std::string&);
        bool m_bIsSelected = false;
        twain_app_info m_sourceInfo;
        twain_session* m_pSession = nullptr;
        bool m_bCloseable = true;
        acquire_characteristics m_acquire_characteristics;
        buffered_transfer_info m_buffered_info;
        file_transfer_info m_filetransfer_info;

        std::unique_ptr<capability_listener> m_capability_listener;

        void get_source_info_internal()
        {
            const auto p_id = static_cast<TW_IDENTITY*>(API_INSTANCE DTWAIN_GetSourceID(m_theSource));
            m_sourceInfo = *p_id;
        }

        #define GET_SET_CAP_VALUE(x,y) {\
            auto val2 = ac.get_##x(); \
            if (val2 != static_cast<decltype(val2)>(acquire_characteristics::default_##x)) {\
                m_capability_info.set_##y({val2}); } }

        #define GET_SET_CAP_VALUE2(x,y,z) {\
            auto val2 = static_cast<z>(ac.get_##x()); \
            if (val2 != static_cast<decltype(val2)>(acquire_characteristics::default_##x)) {\
                m_capability_info.set_##y({val2}); } }

        void attach(DTWAIN_SOURCE source)
        {
            m_theSource = source;
            if (source)
            {
                get_source_info_internal();
                m_capability_info.attach(source);
                m_buffered_info.attach(*this);
                m_bIsSelected = true;
            }
            else
                m_bIsSelected = false;
        }

        void start_apply()
        {
            auto& ci = get_capability_interface();
            auto& ac = get_acquire_characteristics();
            options_base::apply(*this, ac.get_language_options());
            options_base::apply(*this, ac.get_deviceparams_options());
            options_base::apply(*this, ac.get_powermonitor_options());
            options_base::apply(*this, ac.get_doublefeed_options());
            options_base::apply(*this, ac.get_autoadjust_options());
            options_base::apply(*this, ac.get_autoscanning_options());
            options_base::apply(*this, ac.get_barcodedetection_options());
            options_base::apply(*this, ac.get_patchcode_options());
            options_base::apply(*this, ac.get_autocapture_options());
            options_base::apply(*this, ac.get_imagetype_options());
            options_base::apply(*this, ac.get_imageinformation_options());
            options_base::apply(*this, ac.get_userinterface_options());
            options_base::apply(*this, ac.get_imageparamter_options());
            options_base::apply(*this, ac.get_audiblealarms_options());
            options_base::apply(*this, ac.get_deviceevent_options());
            options_base::apply(*this, ac.get_resolution_options());
            options_base::apply(*this, ac.get_paperhandling_options());
            options_base::apply(*this, ac.get_color_options());
            options_base::apply(*this, ac.get_capnegotiation_options());
            options_base::apply(*this, ac.get_micr_options());
            options_base::apply(*this, ac.get_pages_options());
            options_base::apply(*this, ac.get_imprinter_options());
        }

        void prepare_acquisition()
        {
            acquire_characteristics& ac = m_acquire_characteristics;
            start_apply();

            // set the acquisition area
            auto twframe = ac.get_pages_options().get_frame();

            // if user has overridden the default...
            if (twframe != twain_frame<>())
            {
                DTWAIN_ARRAY area = API_INSTANCE DTWAIN_ArrayCreate(DTWAIN_ARRAYFLOAT, 4);
                twain_array arr(area);
                double* buffer = arr.get_buffer<double>();
                buffer[0] = twframe.left;
                buffer[1] = twframe.top;
                buffer[2] = twframe.right;
                buffer[3] = twframe.bottom;
                API_INSTANCE DTWAIN_SetAcquireArea(m_theSource, DTWAIN_AREASET, area, NULL);
            }
            else
                API_INSTANCE DTWAIN_SetAcquireArea(m_theSource, DTWAIN_AREARESET, NULL, NULL);

            // Set the job control option
            API_INSTANCE DTWAIN_SetJobControl(m_theSource, static_cast<LONG>(ac.get_jobcontrol_options().get_option()), TRUE);

            // Disable the manual duplex mode
            API_INSTANCE DTWAIN_SetManualDuplexMode(m_theSource, 0, FALSE);

            // Get the duplex mode
            auto dupmode = ac.get_paperhandling_options().get_manualduplexmode();

            switch (dupmode)
            {
                // Duplex on or off was chosen
                case manualduplexmode_value::none:
                {}
                break;

                // manual duplex mode chosen, so turn this on
                default:

                    // turn off device's duplex mode, if available
                    get_capability_interface().set_cap_values< CAP_DUPLEXENABLED_>({ false });

                    // turn on manual duplex mode
                    API_INSTANCE DTWAIN_SetManualDuplexMode(m_theSource, static_cast<LONG>(dupmode), TRUE);
                break;
            }

            API_INSTANCE DTWAIN_SetAcquireImageNegative(m_theSource, ac.get_imagetype_options().get_negate() ? TRUE : FALSE);
            auto& blank_handler = ac.get_blank_page_options();
            API_INSTANCE DTWAIN_SetBlankPageDetection(m_theSource, blank_handler.get_threshold(),
                static_cast<LONG>(blank_handler.get_discard_option()),
                static_cast<LONG>(blank_handler.is_enabled()));
            auto& multisave_info = ac.get_file_transfer_options().get_multipage_save_options();
            API_INSTANCE DTWAIN_SetMultipageScanMode(m_theSource,
                static_cast<LONG>(multisave_info.get_save_mode())
                |
                (multisave_info.is_save_incomplete() ? static_cast<LONG>(multipage_save_mode::save_incomplete) : 0));

            // Get the general options
            general_options& gOpts = ac.get_general_options();
            API_INSTANCE DTWAIN_SetMaxAcquisitions(m_theSource, gOpts.get_max_acquisitions());

            set_pdf_options();
        }

        void set_pdf_options()
        {
            auto source = get_source();

            // set the PDF file properties
            pdf_options& po = m_acquire_characteristics.get_pdf_options();
            API_INSTANCE DTWAIN_SetPDFCreatorA(source, po.get_creator().c_str());
            API_INSTANCE DTWAIN_SetPDFTitleA(source, po.get_title().c_str());
            API_INSTANCE DTWAIN_SetPDFProducerA(source, po.get_creator().c_str());
            API_INSTANCE DTWAIN_SetPDFAuthorA(source, po.get_author().c_str());
            API_INSTANCE DTWAIN_SetPDFSubjectA(source, po.get_subject().c_str());
            API_INSTANCE DTWAIN_SetPDFKeywordsA(source, po.get_keywords().c_str());
            API_INSTANCE DTWAIN_SetPDFASCIICompression(source, po.is_use_ASCII());
            API_INSTANCE DTWAIN_SetPDFOrientation(source, static_cast<LONG>(po.get_orientation()));

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
            API_INSTANCE DTWAIN_SetPDFPageSize(source, static_cast<LONG>(po.get_page_size_options().get_page_size()), width, height);

            // Set PDF page scale
            auto& pagescaleopts = po.get_page_scale_options();
            custom_used = pagescaleopts.is_custom_scale_used();
            double xscale = 0.0;
            double yscale = 0.0;
            if (custom_used)
            {
                auto pr = pagescaleopts.get_custom_scale();
                xscale = pr.first;
                yscale = pr.second;
            }
            API_INSTANCE DTWAIN_SetPDFPageScale(source, static_cast<LONG>(po.get_page_scale_options().get_page_scale()), xscale, yscale);

            // Set encryption options
            auto& encrypt_opts = po.get_encryption_options();
            if (encrypt_opts.is_use_encryption())
            {
                API_INSTANCE DTWAIN_SetPDFEncryptionA(source, 1, encrypt_opts.get_user_password().c_str(),
                    encrypt_opts.get_owner_password().c_str(),
                    encrypt_opts.get_permissions_int(),
                    encrypt_opts.is_use_strong_encryption());
            }
        }

        void swap(twain_source& left, twain_source& right) noexcept
        {
            std::swap(left.m_theSource, right.m_theSource);
            std::swap(left.m_bIsSelected, right.m_bCloseable);
            std::swap(left.m_sourceInfo, right.m_sourceInfo);
            std::swap(left.m_bCloseable, right.m_bCloseable);
            std::swap(left.m_acquire_characteristics, right.m_acquire_characteristics);
            std::swap(left.m_capability_info, right.m_capability_info);
            std::swap(left.m_pSession, right.m_pSession);
            std::swap(left.m_bUIOnlyOn, right.m_bUIOnlyOn);
        }

        acquire_return_type acquire_to_file(transfer_type transtype)
        {
            acquire_characteristics& ac = m_acquire_characteristics;
            file_transfer_options& ftOptions = ac.get_file_transfer_options();

            LONG dtwain_transfer_type = DTWAIN_USENATIVE;
            if (transtype == transfer_type::file_using_buffered)
                dtwain_transfer_type = DTWAIN_USEBUFFERED;
            dtwain_transfer_type |= static_cast<LONG>(ftOptions.get_file_transfer_flags());

            const auto ft = ac.get_file_transfer_options().get_file_type();
            if (!file_type_info::is_universal_support(ft))
            {
                dtwain_transfer_type |= DTWAIN_USESOURCEMODE;
                // Set the compression type
                API_INSTANCE DTWAIN_SetCompressionType(m_theSource, static_cast<LONG>(ft), 1);
            }
            // check for auto increment
            filename_increment_rules& inc = ftOptions.get_filename_increment_rules();
            file_transfer_info& fTransfer = info_base::get_file_transfer_info(*this);
            API_INSTANCE DTWAIN_SetFileAutoIncrement(m_theSource, inc.get_increment(), inc.is_reset_count_used() ? TRUE : FALSE,
                                        inc.is_enabled() ? TRUE : FALSE);
            API_INSTANCE DTWAIN_EnableMsgNotify(1);
            LONG file_type = static_cast<LONG>(ftOptions.get_file_type());
            if (ftOptions.can_multi_page())
                file_type = ftOptions.get_multi_page_type();
            general_options& gOpts = ac.get_general_options();

            color_value::value_type ct = m_capability_info.get_cap_values< ICAP_PIXELTYPE_>(capability_interface::get_current()).front();

            bool isModeless = m_pSession->get_twain_characteristics().is_custom_twain_loop();
            API_INSTANCE DTWAIN_SetTwainMode(isModeless ? DTWAIN_MODELESS : DTWAIN_MODAL);
            auto retval = API_INSTANCE DTWAIN_AcquireFileA(m_theSource, ftOptions.get_filename_pattern().c_str(),
                                              file_type,
                                              dtwain_transfer_type,
                                              static_cast<LONG>(ct),
                                              static_cast<LONG>(gOpts.get_max_pages()), 
                                              ac.get_userinterface_options().is_shown(),
                                              gOpts.get_source_action() == sourceaction_type::closeafteracquire,
                                              nullptr) != 0;

            // if app is modeless, this needs to go back to the caller to loop
            if (retval)
                return { acquire_ok, {} };
            else
                return { acquire_canceled, {} };
        }

        acquire_return_type acquire_to_image_handles(transfer_type transtype)
        {
            acquire_characteristics& ac = m_acquire_characteristics;
            general_options& gOpts = ac.get_general_options();
            color_value::value_type ct = m_capability_info.get_cap_values< ICAP_PIXELTYPE_>(capability_interface::get_current()).front();

            bool isModeless = m_pSession->get_twain_characteristics().is_custom_twain_loop();
            API_INSTANCE DTWAIN_SetTwainMode(isModeless ? DTWAIN_MODELESS : DTWAIN_MODAL);

            if (transtype == transfer_type::image_native || transtype == transfer_type::image_buffered)
            {
                twain_array images(API_INSTANCE DTWAIN_CreateAcquisitionArray());
                bool retval = false;
                if (transtype == transfer_type::image_native)
                    retval = API_INSTANCE DTWAIN_AcquireNativeEx(m_theSource,
                                                    static_cast<LONG>(ct),
                                                    static_cast<LONG>(gOpts.get_max_pages()), 
                                                    ac.get_userinterface_options().is_shown(),
                                                    ac.get_general_options().get_source_action() == sourceaction_type::closeafteracquire, 
                                                    images.get_array(), nullptr) != 0;
                else
                {
                    // Set the compression type first, if it needs to be set
                    options_base::apply(*this, ac.get_compression_options());
                    buffered_transfer_info& bt = get_buffered_transfer_info();
                    bt.init_transfer(
                        static_cast<compression_value::value_type>(m_capability_info.get_cap_values< ICAP_COMPRESSION_>(capability_interface::get_current()).front()));
                    retval = API_INSTANCE DTWAIN_AcquireBufferedEx( m_theSource,
                                                        static_cast<LONG>(ct),
                                                        static_cast<LONG>(gOpts.get_max_pages()), 
                                                        ac.get_userinterface_options().is_shown(),
                                                        gOpts.get_source_action() == sourceaction_type::closeafteracquire, 
                                                        images.get_array(), 
                                                        nullptr) != 0;
                }
                int32_t last_error = twain_session::get_last_error();
                if (retval || last_error == DTWAIN_NO_ERROR )
                   return { acquire_ok, std::move(images) };
                else
                    return { last_error, {} };
            }
            return { acquire_canceled, {} };
        }

        void wait_for_feeder(bool& status)
        {
            using namespace std::chrono_literals;
            // check for feeder stuff here
            paperhandling_info paperinfo;
            paperinfo.get_info(*this);

            bool isfeedersupported = paperinfo.is_feedersupported();
            if (!isfeedersupported)
            {
                status = true;
                return;
            }
            twain_std_array<capability_type::feederenabled_type, 1> arr;
            arr[0] = 1;
            m_capability_info.set_cap_values< CAP_FEEDERENABLED_>(arr);
            auto& vEnabled = m_capability_info.get_cap_values< CAP_FEEDERENABLED_>(capability_interface::get_current());
            if ( vEnabled.empty() || !vEnabled.front())
            {
                // feeder not enabled
                status = true;
                return;
            }

            bool ispaperdetectable = m_capability_info.is_cap_supported(CAP_PAPERDETECTABLE);
            if (!ispaperdetectable)
            {
                // Cannot detect if paper is in feeder
                status = true;
                return;
            }

            bool isfeederloaded = m_capability_info.is_cap_supported(CAP_FEEDERLOADED);
            if (!isfeederloaded)
            {
                // Cannot detect if feeder is loaded
                status = true;
                return;
            }

            auto timeoutval = get_acquire_characteristics().get_paperhandling_options().get_feederwait();

            twain_timer theTimer;

            // loop until feeder is loaded
            while (!m_capability_info.get_cap_values<CAP_FEEDERLOADED_>(capability_interface::get_current()).front())
            {
                if (timeoutval != -1)
                {
                    if (theTimer.elapsed() > timeoutval)
                    {
                        status = false;
                        return;
                    }
                }
                std::this_thread::sleep_for(1ms);
            }
            status = true;
            return;
        }

    public:
        typedef double resolution_type;
        twain_source(const twain_source&) = delete;
        twain_source& operator=(const twain_source&) = delete;

        //! Move constructor.
            /*!
              Allows movability (std::move) of *this
            */        
        twain_source(twain_source&& rhs) noexcept
        {
            swap(*this, rhs);
            rhs.m_theSource = nullptr;
        }

        //! Move assignment.
            /*!
              Allows movability (std::move) of *this
            */
        twain_source& operator=(twain_source&& rhs) noexcept
        {
            if (this == &rhs)
                return *this;
            swap(*this, rhs);
            rhs.m_theSource = nullptr;
            if (rhs.m_capability_listener)
                m_capability_listener = std::move(rhs.m_capability_listener);
            else
                m_capability_listener.reset();
            rhs.m_capability_listener = nullptr;
            return *this;
        }

        /// Creates an instance of a twain_source from a low-level API TWAIN Source handle (DTWAIN_SOURCE).
        /// 
        /// The DTWAIN_SOURCE is a member in the select_return_type structure.  This structure also contains a pointer to the twain_session that was used to select the device.
        /// @param[in] select_return The selected source
        /// @see attach(const source_select_info&)
        twain_source(const source_select_info& select_info = source_select_info()) : 
                                    m_bIsSelected(false), m_bCloseable(false), m_pSession(nullptr)
        {
            attach(select_info);
        }

        /// Destructor.  If a DTWAIN_SOURCE is attached, the destructor automatically closes the device.
        /// If a DTWAIN_SOURCE is attached, the destructor automatically closes the device.Note that the destructor will wait until all acquisition processing
        /// is finished by the device.
        /// @see close().
        ~twain_source() noexcept
        {
            try
            {
               close();
            }
            catch(...) {}
        }

        /// Attaches an existing DTWAIN_SOURCE to the twain_source object
        /// 
        /// Attach an existing DTWAIN_SOURCE.  If **this** already has a DTWAIN_SOURCE attached, then the passed-in DTWAIN_SOURCE replaces the current DTWAIN_SOURCE.
        /// @param[in] select_return The DTWAIN_SOURCE.  The DTWAIN_SOURCE is a member in the select_return_type structure.
        void attach(const source_select_info& select_return )
        {
            m_theSource = select_return.source_handle;
            if (m_theSource)
            {
                attach(m_theSource);
                m_pSession = select_return.session_handle;
            }
        }

        /// Detaches an attached DTWAIN_SOURCE.
        /// 
        /// Detaches a DTWAIN source from the twain_source object.  
        /// @note Once a twain_source is detached, the DTWAIN_SOURCE that was attached is still valid (not closed). 
        void detach()
        {
            m_theSource = nullptr;
            m_bIsSelected = false;
            m_capability_info.detach();
        }

        template <typename T>
        void set_cap_listener(T& val)
        { m_capability_listener = std::make_unique<T>(T()); }

        twain_app_info get_source_info() const noexcept { return m_sourceInfo; }


        /// Returns a HANDLE to the current image acquired by the twain_source during the acquisition process.
        /// 
        /// If running under Windows OS, the HANDLE is a memory buffer that requires GlobalLock() to access the memory.The HANDLE describes
        /// a Device Independent Bitmap, where the LPBITINFOHEADER and other Windows API structures can be used to get further information.
        /// Note that get_current_image() is designed to be used while the acquisition process is occurring, thus the twain_listenter::acquiredone() and
        /// other twain_listener functions can be used.
        /// @returns A HANDLE to the current image that was acquired by the device.  The image is in the form of a Device Independent Bitmap.
        /// @note get_current_image() is designed to be used while the acquisition process is occurring, thus the twain_listenter::acquiredone() and other twain_listener functions can be used.
        HANDLE get_current_image()
        {
            return API_INSTANCE DTWAIN_GetCurrentAcquiredImage(get_source());
        }

        /// Returns a reference to the twain_source's acquire_characteristics.
        /// The acquire_characteristics describe the options to apply to the TWAIN device before and during the image acquisition process.  
        /// For example, transfer type, page size, color type, etc.
        /// @returns A reference to the current twain_source's acquire_characteristics used when acquiring images.
        /// @note The acquire_characteristics can be replaced with another set of characteristics using set_acquire_characteristics
        /// @see set_acquire_characteristics()
        acquire_characteristics& get_acquire_characteristics() noexcept { return m_acquire_characteristics; }

        /// Applies the acquire_characteristics to the twain_source.
        /// 
        /// This function is used to apply an entire set of acquire_characteristics to a twain_source, thus
        /// makes it simple to swap in and out various profile settings to use when acquiring images.
        /// @param[in] acq_characteristics acquire_characteristics to apply
        /// @returns A reference to the current twain_source.
        /// @see get_acquire_characteristics()
        twain_source& set_acquire_characteristics(const acquire_characteristics& acq_characteristics) noexcept
        { 
            m_acquire_characteristics = acq_characteristics; return *this;
        }

        buffered_transfer_info& get_buffered_transfer_info() noexcept { return m_buffered_info; }

        /// Returns a const reference to the capability_interface of the twain_source.
        /// 
        /// The capability_interface allows an application to get, set, and query the capabilities that the attached DTWAIN_SOURCE has available.
        /// If no DTWAIN_SOURCE is attached, a const reference is still returned, but will only be operable once a DTWAIN_SOURCE is attached.
        /// 
        /// @returns A const reference to the current capability_interface.  
        /// @note If no DTWAIN_SOURCE is attached, a const reference is still returned, but will only be operable once a DTWAIN_SOURCE is attached.
        const capability_interface& get_capability_interface() const noexcept { return m_capability_info; }

        /// Starts the acquisition process of the twain_source.
        /// 
        /// @returns std::pair<int32_t, twain_array> where **first** is the return code, **second** is the twain_array of images
        acquire_return_type acquire()
        {
            bool fstatus = true;
            prepare_acquisition();
            if (!m_acquire_characteristics.get_paperhandling_options().get_feederenabled())
                API_INSTANCE DTWAIN_EnableFeeder(m_theSource, FALSE);
            else
            {
                auto& feedOptions = m_acquire_characteristics.get_paperhandling_options();
                auto fmode = feedOptions.get_feedermode();
                bool use_feeder_or_flatbed = (fmode == feedermode_value::feeder_flatbed);
                bool use_wait = (feedOptions.get_feederwait() != 0);
                if (use_wait || use_feeder_or_flatbed)
                {
                    fstatus = false;
                    wait_for_feeder(fstatus);

                    // timed out waiting for the feeder to be loaded, or device doesn't support feeder
                    if (!fstatus && !use_feeder_or_flatbed)
                        return acquire_return_type{ acquire_timeout, {} };
                }

                // if we got a timeout on the feeder, but use flatbed as backup, disable the feeder and use the flatbed
                if (!fstatus && use_feeder_or_flatbed)
                {
                    API_INSTANCE DTWAIN_EnableFeeder(m_theSource, FALSE);
                    fstatus = true;
                }
            }

            if (fstatus)
            {
                if (callback_proc(twain_listener_values::DTWAIN_PREACQUIRE_START, 0, reinterpret_cast<LONG64>(m_pSession)))
                {
                    const auto transtype = m_acquire_characteristics.get_general_options().get_transfer_type();
                    if (transtype == transfer_type::file_using_native ||
                        transtype == transfer_type::file_using_buffered ||
                        transtype == transfer_type::file_using_source)
                        return acquire_to_file(transtype);
                    return acquire_to_image_handles(transtype);
                }
                else
                    callback_proc(twain_listener_values::DTWAIN_PREACQUIRE_TERMINATE, 0, reinterpret_cast<LONG64>(m_pSession));
            }
            return acquire_return_type{ acquire_canceled, {} };
        }

        /// Displays the twain_source's user interface, but does not allow acquiring images.
        /// 
        /// This mode is used to set up the device by using the device's user interface, but does not allow the acquisition of images.  
        /// @returns **true** if the user interface was displayed successfully, **false** otherwise.
        /// @note There are TWAIN devices that do not support this operation, thus this function will return **false** for those devices.
        /// 
        ///   \code {.cpp}
        ///   #include <dynarithmic\twain\twain_session.hpp>
        ///   #include <dynarithmic\twain\twain_source.hpp>
        ///   using namespace dynarithmic::twain;
        ///   int main()
        ///   {
        ///       twain_session session;
        ///       if ( session.start() )
        ///       {
        ///          twain_source source = twain_session::select();
        ///          if ( source.is_open() )
        ///          {
        ///              if ( source.is_uionlysupported() )
        ///                  source.showui_only();
        ///          }
        ///       }
        ///   } // session will close automatically
        ///   \endcode
        bool showui_only()
        {
            if (m_capability_info.is_cap_supported(CAP_ENABLEDSUIONLY))
            {
                struct val_restore
                {
                    bool* ui_on;
                    bool ismodeless;
                    val_restore(bool* p, bool modeless) : ui_on(p), ismodeless(modeless) {}
                    ~val_restore() { if ( !ismodeless) *ui_on = false; }
                };

                bool isModeless = m_pSession->get_twain_characteristics().is_custom_twain_loop();
                API_INSTANCE DTWAIN_SetTwainMode(isModeless ? DTWAIN_MODELESS : DTWAIN_MODAL);
                m_bUIOnlyOn = true;
                val_restore vr(&m_bUIOnlyOn, isModeless);
                return API_INSTANCE DTWAIN_ShowUIOnly(m_theSource) ? true : false;
            }
            return false;
        }

        /// Returns the TW_IDENTITY* that represents this TWAIN source.
        /// 
        /// @returns a pointer to the **TW_IDENTITY** that represents this TWAIN source.  
        /// @note The return value can be used in a call to call_dsm() (usually the second parameter).
        /// @see dynarithmic::twain::twain_session::call_dsm()
        TW_IDENTITY* get_twain_id()
        {
            if (m_theSource)
                return &m_sourceInfo.get_identity();
            return nullptr;
        }

        //! Returns an image_handler object which will allow easy access to the array of images that is returned by the twain_source::acquire() function
            /*!
        *   Returns an image_handler object which will allow easy access to the array of images that is returned by the twain_source::acquire() function.
        *   <p>Note that images are  returned for native and buffered transfers (non-image file) only.
        *   \returns  An image_handler object that is a high-level "two-dimensional" image wrapper that has all of the HANDLE objects that **true** if the user interface was displayed successfully, **false** otherwise.
        *
        *   \code {.cpp}
         *   #include <dynarithmic\twain\twain_session.hpp>
         *   #include <dynarithmic\twain\twain_source.hpp>
         *   #include <iostream>
         * 
         *   using namespace dynarithmic::twain;
         *   int main()
             {
         *       twain_session session;
         *       if ( session.start() )
         *       {
         *          twain_source source = twain_session::select();
         *          if ( source.is_open() )
         *          {
         *              source.get_acquire_characteristics().get_general_options().set_transfer_type(transfer_type::image_native);
         *              auto retval = source.acquire();
         *              if (retval.first == acquire_return_code::acquire_ok)
                        {
                            image_handler images = twain_source::get_images(retval.second);  // this sets up the image_handler with the acquired images
                            std::cout << "There were " << images.get_num_acquisitions() << " attempts of acquiring sets of images\n\n";
                            std::cout << "The number of images per acquisition are as follows:\n";

                            for (size_t i = 0; i < images.get_num_acquisitions(); ++i)
                            {
                                std::cout << "Acquisition " << i + 1 << " contains " << images.get_num_pages(i) << " image(s)\n";
                                auto& imgHandles = images.get_acquisition_images(i);
                                for (HANDLE wndHandle : imgHandles)
                                {
                                    // the HANDLE is a Windows DIB handle, stored as an unlocked memory handle.  We will want to do 
                                    // something with it now, like display it in a Window maybe?
                                    char* dib = (char*)::GlobalLock(wndHandle);
                                    // do something with dib.
                                    //
                                    ::GlobalUnlock(wndHandle);
                                }
                            }
                        }
         *          }
         *       }
         *   } 
         *   \endcode
         *
        */
        static image_handler get_images(const twain_array& images)
        {
            image_handler ih;
            const auto acq_count = images.get_count();
            for (int i = 0; i < acq_count; ++i)
            {
                twain_array img_array(API_INSTANCE DTWAIN_GetAcquiredImageArray(images.get_array(), i));
                ih.add_new_page();
                const auto image_count = img_array.get_count();
                const HANDLE* handleBuffer = reinterpret_cast<HANDLE*>(img_array.get_buffer<HANDLE>());
                for (auto j = 0; j < image_count; ++j)
                    ih.push_back_image(handleBuffer[j]);
            }
            return ih;
        }

        /// Opens the attached DTWAIN_SOURCE if it has been selected
        /// 
        /// Opens the attached DTWAIN_SOURCE if it has been selected.  This function is called automatically by twain_session::select_source() if the **open** parameter
        /// (the second parameter to twain_session::select_source()) is **true< / b>, otherwise the application is responsible for calling the twain_source::open() function
        /// @returns **true** if the source is opened, **false** otherwise.
        /// @see close()
        bool open()
        {
            if (m_theSource)
            {
                if (API_INSTANCE DTWAIN_OpenSource(m_theSource))
                {
                    get_source_info_internal();
                    attach(m_theSource);
                    return true;
                }
            }
            return false;
        }

        /// Closes the attached DTWAIN_SOURCE if it has been selected. 
        /// 
        /// Once a source is closed, the source must be reopened using open()
        /// @returns **true** if the source is successfully closed, **false** otherwise.
        /// @see open() ~twain_source()
        bool close()
        {
            if (m_theSource)
            {
                bool retVal = API_INSTANCE DTWAIN_CloseSource(m_theSource) ? true : false;
                m_theSource = nullptr;
                return retVal;
            }
            return false;
        }

        /// Returns whether the twain_source is opened.
        /// 
        /// @returns **true** if the TWAIN source has been opened, **false** otherwise.
        /// @see open()
        bool is_open() const { return DTWAIN_IsSourceOpen(m_theSource)?true:false; }

        /**
        \returns **true** if the source has been selected by the user, **false** otherwise.
        */
        bool is_selected() const noexcept
        {
            return m_bIsSelected;
        }

        bool is_closeable() const noexcept
        {
            return m_bCloseable;
        }

        /// Returns whether the twain_source is in the acquisition process.
        /// 
        /// @returns **true** if the twain_source is still in the acquisition state, **false** otherwise.
        /// @see acquire()
        bool is_acquiring() const { return API_INSTANCE DTWAIN_IsSourceAcquiring(m_theSource) ? true : false; }

        /// Returns whether the twain_source has the user interface displayed.
        /// 
        /// @returns **true** if the twain_source has the user interface displayed, **false** otherwise.
        bool is_uienabled() const { return API_INSTANCE DTWAIN_IsUIEnabled(get_source()) ? true : false; }


        /**
        \returns **true** if the source supports the twain_source user interface to be displayed without acquiring images.
        */
        bool is_uionlysupported() const
        {
            return m_capability_info.is_cap_supported(CAP_ENABLEDSUIONLY);
        }

        //! Returns the image information that is about to be acquired from the TWAIN device.
            /*!
        * Returns the image information that is about to be acquired from the TWAIN device.  This call can be issued while processing messages using the twain_listener.a container that represents the TWAIN device's customized data string(s).  
        \returns An image_information structure populated with the image information
        */
        image_information get_current_image_information() const
        {
            DTWAIN_FLOAT xRes, yRes;
            LONG width, length, numsamples;
            DTWAIN_ARRAY bitspersample;
            LONG bitsperpixel;
            LONG planar;
            LONG pixeltype, compression;
            image_information iinfo;
            if (API_INSTANCE DTWAIN_GetImageInfo(m_theSource, &xRes, &yRes, &width, &length, &numsamples,
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
                iinfo.bitsPerSample.resize(numsamples);
            }
            return iinfo;
        }

        // We need to have this part of the twain_source, since camera 
        // side could be switched to allow capability setting for each side.
        bool set_current_camera(const cameraside_value::value_type& camera)
        {
            auto vect = m_capability_info.get_cap_values<CAP_CAMERASIDE_>();
            if (std::find(vect.begin(), vect.end(), static_cast<capability_type::cameraside_type>(camera)) == vect.end())
                return false;
            m_capability_info.set_cap_values<CAP_CAMERASIDE_>({ static_cast<capability_type::cameraside_type>(camera) });
            return m_capability_info.get_last_error().return_value;
        }

        //! Returns a container that represents the TWAIN device's customized data string(s).
            /*!
        * Returns a container that represents the TWAIN device's customized data string(s).  The returned data is manufacturer and device dependent.
        \returns By default a std::vector<unsigned char> that represents the data.  
        */
        template <typename Container = std::vector<unsigned char>>
        Container get_custom_data() const
        {
            Container ct;
            if (!m_capability_info.is_customdsdata_supported())
                 return ct;
            LONG actual_size;
            std::vector<unsigned char> str;
            auto retval = API_INSTANCE DTWAIN_GetCustomDSData(m_theSource, nullptr, 0, &actual_size, DTWAINGCD_RETURNHANDLE);
            if (retval)
            {
                str.resize(actual_size + 1);
                retval = API_INSTANCE DTWAIN_GetCustomDSData(m_theSource, reinterpret_cast<LPBYTE>(&str[0]), actual_size,
                                                &actual_size, DTWAINGCD_COPYDATA);
            }
            std::copy(str.begin(), str.end(), std::inserter(ct, ct.begin()));
            return ct;
        }

        //! Sets the TWAIN device's customized data string(s).
        /*!
        * Sets the TWAIN device's customized data string(s).  The data string is manufacturer and device dependent.
        \returns Returns **true** if successful, **false** otherwise.
        */
        template <typename Container = std::vector<unsigned char>>
        bool set_custom_data(const Container& s) const
        {
            if (!m_capability_info.is_customdsdata_supported())
                return false;
            return API_INSTANCE DTWAIN_SetCustomDSData(m_theSource, NULL, reinterpret_cast<LPCBYTE>(&s[0]), s.size(),
                                            DTWAINSCD_USEDATA)
                        ? true
                        : false;
        }
    };

    inline std::ostream& operator <<(std::ostream& os, const image_information& ii)
    {
        std::vector<std::string> sList;
        std::transform(ii.bitsPerSample.begin(), 
                       ii.bitsPerSample.begin() + (std::min)(static_cast<size_t>(ii.numsamples), ii.bitsPerSample.size()),
                       std::back_inserter(sList), [](int n) { return std::to_string(n);  });
        std::string commaList = std::accumulate(sList.begin(), sList.end(), std::string(),
                                          [](std::string& total, std::string& n) { return total + "," + n; });
        std::string bps = "[" + commaList + "]";
        os << "\nx_resolution: " << ii.x_resolution << "\n"
            << "y_resolution: " << ii.y_resolution << "\n"
            << "width: " << ii.width << "\n"
            << "length: " << ii.length << "\n"
            << "numsamples: " << ii.numsamples << "\n"
            << "bitsPerSample: " << bps << "\n"
            << "bitsPerPixel: " << ii.bitsPerPixel << "\n"
            << "planar: " << (ii.planar ? "true" : "false") << "\n"
            << "pixeltype: " << ii.pixelType << "\n"
            << "compression: " << ii.compression;
        return os;
    }
}
}
#endif
