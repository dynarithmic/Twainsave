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

namespace dynarithmic
{
	namespace twain
	{
		duplex_info::duplex_info() : m_Source(nullptr) {}

		void duplex_info::attach(twain_source& mSource)
		{
			m_Source = mSource.get_source();
			capability_interface& ci = mSource.get_capability_interface();
			std::vector<capability_interface::duplex_type> vect;
			ci.get_duplex(vect);
			if (!vect.empty())
				m_type = static_cast<duplex_type>(vect.front());
			else
				m_type = duplex_type::none;
			m_bIsSupported = (m_type != duplex_type::none);
		}

		bool duplex_info::is_enabled() const { return m_Source ? DTWAIN_IsDuplexEnabled(m_Source) : false; }
		bool duplex_info::is_supported() const { return m_bIsSupported; }
		duplex_type duplex_info::get_type() const { return m_type; }
	}
}