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
#ifndef DTWAIN_TWAIN_SOURCE_BASE_HPP
#define DTWAIN_TWAIN_SOURCE_BASE_HPP

#include <dynarithmic/twain/acquire_characteristics.hpp>
#include <dynarithmic/twain/capability_interface.hpp>

namespace dynarithmic {
namespace twain {

    /**
    The twain_source_base class serves as the base class to twain_source.
    */
    class twain_source_base
    {
        protected:
            mutable capability_interface m_capability_info;
            DTWAIN_SOURCE m_theSource;
            bool m_bUIOnlyOn; 

        public:
            twain_source_base(const twain_source_base&) = delete;
            twain_source_base& operator=(const twain_source_base&) = delete;
            twain_source_base() : m_theSource(nullptr), m_bUIOnlyOn(false) {}
            virtual ~twain_source_base() {}

            /// Returns the attached DTWAIN_SOURCE
            /// 
            /// Returns the attached DTWAIN_SOURCE handle.  If no handle is attached, a **nullptr** is returned.
            /// @returns The attached DTWAIN_SOURCE, or **nullptr** is returned if no DTWAIN_SOURCE is attached.
            /// @see attach()
            /// @note The DTWAIN_SOURCE returned can be used for low-level DTWAIN API calls that require a DTWAIN_SOURCE.
            DTWAIN_SOURCE get_source() const noexcept { return m_theSource; }

            /// Returns a reference to the capability interface of the twain_source.
            /// 
            /// The capability_interface provides all of the functions to get and set the capabilities of the twain_source.
            /// @returns Reference to the capability_interface.
            capability_interface& get_capability_interface() noexcept { return m_capability_info; }

            //! Returns whether the twain_source has the user interface displayed but in a non-acquisition state.
                /*!
                  //! Returns whether the twain_source has the user interface displayed but in a non-acquisition state.<p>
                      The is_uionlyenabled() differs from is_uienabled() in that the is_uionlyenabled() only tests the mode
                      mode where the twain_source is displaying the user interface in a non-acquisition state.
                */
                /**
                \returns **true** if the twain_source has the user interface displayed, **false** otherwise.
                */
            bool is_uionlyenabled() const noexcept { return m_bUIOnlyOn; }
            void set_uionlyenabled(bool bSet) noexcept { m_bUIOnlyOn = bSet; }
    };
}
}
#endif
