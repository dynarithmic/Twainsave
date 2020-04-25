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
		feeder_info::feeder_info() : m_Source(nullptr) {}
		bool feeder_info::is_loaded() const { return DTWAIN_IsFeederLoaded(m_Source) ? true : false; }
		bool feeder_info::is_enabled() const { return DTWAIN_IsFeederEnabled(m_Source) ? true : false; }
		bool feeder_info::is_supported() const { return m_bIsSupported; }
		void feeder_info::attach(twain_source& mSource) 
		{ 
			m_Source = mSource.get_source(); 
			m_bIsSupported = DTWAIN_IsFeederSupported(m_Source) ? true : false;
		}
	}
}
