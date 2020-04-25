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
enum class duplex_type
{
    none = TWDX_NONE,
	onepass = TWDX_1PASSDUPLEX,
	twopass = TWDX_2PASSDUPLEX
};

class duplex_info
{
	DTWAIN_SOURCE m_Source;
	duplex_type m_type;
	bool m_bIsSupported;

	public:
		duplex_info();
		duplex_type get_type() const;
		bool is_enabled() const;
		bool is_supported() const;
		void attach(dynarithmic::twain::twain_source& mSource);
};