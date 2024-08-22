/*
This file is part of the Twainsave-opensource version
Copyright (c) 2002-2024 Dynarithmic Software.

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
#ifndef TWAINSAVE_H
#define TWAINSAVE_H

#if ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || __cplusplus >= 201703L)
	#include <filesystem>
	#include <string_view>
	namespace filesys = std::filesystem;
	using stringview = std::string_view;
#else
	#define USE_BOOST_FILESYSTEM
	#define USE_BOOST_STRINGVIEW
#endif

#ifdef USE_BOOST_FILESYSTEM
	#pragma warning (push)
	#pragma warning (disable : 4714)
	#include <boost/filesystem.hpp>
	namespace filesys = boost::filesystem;
	#pragma warning (pop)
#endif
#ifdef USE_BOOST_STRINGVIEW
	#pragma warning (push)
	#pragma warning (disable : 4714)
	#include <boost/utility/string_view.hpp>
	using stringview = boost::string_view;
	#pragma warning (pop)
#endif
#endif

