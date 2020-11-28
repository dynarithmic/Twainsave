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
#ifndef DTWAIN_TWAIN_ARRAY_HPP
#define DTWAIN_TWAIN_ARRAY_HPP

#include <array>
#include <algorithm>
#include <dtwain.h>
#include <dynarithmic/twain/types/twain_frame.hpp>
#include <dynarithmic/twain/dtwain_twain.hpp>

namespace dynarithmic
{
    namespace twain
    {
        template <typename T, size_t sz>
        class twain_std_array : public std::array<T, sz>
        {
            public:
                typename std::array<T, sz>::iterator insert(typename std::array<T, sz>::iterator iter, const T& val)
                {
                    if (iter == this->end())
                        iter = this->begin();
                    *iter = val;
                    return iter;
                }
            
                void clear() { std::fill(this->begin(), this->end(), T()); }
        };

        // Class that is used for capability setting / getting, and other tasks that require
        // the client (us) to communicate with various DTWAIN functions.
        class twain_array
        {
            DTWAIN_ARRAY m_theArray = nullptr;
            bool m_isRange = false;
            
            public:
                template <typename T>
                T* get_buffer()
                {
                    if (m_theArray) return reinterpret_cast<T*>(API_INSTANCE DTWAIN_ArrayGetBuffer(m_theArray, 0));
                    return nullptr;
                }

                twain_array() : m_theArray(nullptr)
                {}

                twain_array(DTWAIN_ARRAY a) : m_theArray(a)
                {
                    LONG nStatus = 0;
                    m_isRange = API_INSTANCE DTWAIN_RangeIsValid(a, &nStatus) ? true : false;
                }

                twain_array(const twain_array& rhs)
                {
                    if (rhs.m_theArray)
                        m_theArray = API_INSTANCE DTWAIN_ArrayCreateCopy(rhs.m_theArray);
                    m_isRange = rhs.m_isRange;
                }

                twain_array(twain_array&& rhs) noexcept
                {
                    m_theArray = rhs.m_theArray;
                    m_isRange = rhs.m_isRange;
                    rhs.m_theArray = nullptr;
                }

                twain_array& operator=(twain_array&& rhs) noexcept
                {
                    swap(*this, rhs);
                    rhs.m_theArray = nullptr;
                    return *this;
                }

                twain_array& operator=(const twain_array& rhs)
                {
                    if (&rhs != this)
                    {
                        auto temp(rhs);
                        swap(temp, *this);
                    }
                    return *this;
                }

                void swap(twain_array& t1, twain_array& t2) const
                {
                    std::swap(t1.m_theArray, t2.m_theArray);
                    std::swap(t1.m_isRange, t2.m_isRange);
                }

                void set_array(DTWAIN_ARRAY a) { m_theArray = a; m_isRange = API_INSTANCE DTWAIN_RangeIsValid(a, nullptr)?true:false; }
                void resize(size_t n) const { if (m_theArray) API_INSTANCE DTWAIN_ArrayResize(m_theArray, static_cast<LONG>(n)); }
                bool is_range() const { return m_isRange; }

                DTWAIN_ARRAY get_array() const { return m_theArray; }
                LPDTWAIN_ARRAY get_array_ptr() { return &m_theArray; }
                ~twain_array() { if (m_theArray) API_INSTANCE DTWAIN_ArrayDestroy(m_theArray); }

                long get_count() const
                {
                    if (m_theArray) return API_INSTANCE DTWAIN_ArrayGetCount(m_theArray);
                    return -1L;
                }

                bool expand_range_replace()
                {
                    if (m_isRange)
                    {
                        DTWAIN_ARRAY temp;
                        if (API_INSTANCE DTWAIN_RangeExpand(m_theArray, &temp))
                        {
                            const twain_array temp_arr(temp);
                            *this = temp_arr;
                            return true;
                        }
                    }
                    return false;
                }

                long get_expanded_count() const
                {
                    if (m_isRange)
                        return API_INSTANCE DTWAIN_RangeGetCount(m_theArray);
                    return 0;
                }

                template <typename Container>
                bool expand_range(Container& ct) const
                {
                    if (m_isRange)
                    {
                        DTWAIN_ARRAY temp;
                        if (API_INSTANCE DTWAIN_RangeExpand(m_theArray, &temp))
                        {
                            twain_array temp_arr(temp);
                            twain_array_copy_traits::copy_from_twain_array(temp_arr, ct);
                            return true;
                        }
                    }
                    return false;
                }
        };

        // copying traits for the twain_array
        struct twain_array_copy_traits
        {
            template <typename T>
            struct dtwain_underlying_type
            {
                typedef LONG value_type;
            };

            template <>
            struct dtwain_underlying_type<double>
            {
                typedef double value_type;
            };

            template <typename Container, typename std::enable_if<
                            std::is_floating_point<typename Container::value_type>::value ||
                            std::is_integral<typename Container::value_type>::value, bool>::type = 1>
            static void copy_from_twain_array(twain_array& ta, long sz, Container& C)
            {
                auto pBuffer = ta.get_buffer<dtwain_underlying_type<Container::value_type>::value_type>();
                std::transform(pBuffer, pBuffer + sz, std::inserter(C, std::end(C)), []
                                (dtwain_underlying_type<Container::value_type>::value_type val) 
                                { return static_cast<Container::value_type>(val); });
            }

            template <typename Container, typename std::enable_if<
                            std::is_floating_point<typename Container::value_type>::value ||
                            std::is_integral<typename Container::value_type>::value, bool>::type = 1>
            static void copy_from_twain_array(twain_array& ta, Container& C)
            {
                copy_from_twain_array(ta, ta.get_count(), C);
            }

            template <typename Container, typename std::enable_if<
                            std::is_same<typename Container::value_type, std::string>::value, bool>::type = 1>
            static void copy_from_twain_array(twain_array& ta, long sz, Container& C)
            {
                std::vector<std::string> vFrm(1);
                std::vector<char> v_char(API_INSTANCE DTWAIN_ArrayGetMaxStringLength(ta.get_array()) + 1);
                if (v_char.empty())
                    return;
                for (long i = 0; i < sz; ++i)
                {
                    API_INSTANCE DTWAIN_ArrayGetAtStringA(ta.get_array(), i, &v_char[0]);
                    vFrm[0] = &v_char[0];
                    std::copy(vFrm.begin(), vFrm.end(), std::inserter(C, C.end()));
                }
            }

            template <typename Container, typename std::enable_if<
                            std::is_same<typename Container::value_type, std::string>::value, bool>::type = 1>
            static void copy_from_twain_array(twain_array& ta, Container& C)
            {
                return copy_from_twain_array(ta, ta.get_count(), C);
            }

            template <typename Container, typename std::enable_if<
                            std::is_same<typename Container::value_type, twain_frame<>>::value, bool>::type = 1>
            static void copy_from_twain_array(const twain_array& ta, long sz, Container& C)
            {
                std::vector<twain_frame<>> vFrm(1);
                for (long i = 0; i < sz; ++i)
                {
                    API_INSTANCE DTWAIN_ArrayFrameGetAt(ta.get_array(), i, &(vFrm[0].left), &(vFrm[0].top), &(vFrm[0].right),
                                            &(vFrm[0].bottom));
                    std::copy(vFrm.begin(), vFrm.end(), std::inserter(C, C.end()));
                }
            }

            template <typename Container, typename std::enable_if<
                            std::is_same<typename Container::value_type, twain_frame<>>::value, bool>::type = 1>
            static void copy_from_twain_array(const twain_array& ta, Container& C)
            {
                copy_from_twain_array(ta, ta.get_count(), C);
            }

            template <typename Container, typename std::enable_if<
                            std::is_floating_point<typename Container::value_type>::value ||
                            std::is_integral<typename Container::value_type>::value, bool>::type = 1>
            static void copy_to_twain_array(DTWAIN_SOURCE theSource, twain_array& ta, int cap_value, const Container& C)
            {
                ta.set_array(API_INSTANCE DTWAIN_ArrayCreateFromCap(theSource, cap_value, static_cast<LONG>(C.size())));
                auto pBuffer = reinterpret_cast<dtwain_underlying_type<Container::value_type>::value_type*>(API_INSTANCE DTWAIN_ArrayGetBuffer(ta.get_array(), 0));
                std::copy(C.begin(), C.end(), pBuffer);
            }

            template <typename T, typename Container, typename std::enable_if<
                            std::is_floating_point<typename Container::value_type>::value ||
                            std::is_integral<typename Container::value_type>::value, bool>::type = 1>
            static void copy_to_twain_array(DTWAIN_SOURCE theSource, twain_array& ta, const Container& C)
            {
                copy_to_twain_array(theSource, ta, T::cap_value, C);
            }


            template <typename Container, typename std::enable_if<
                            std::is_same<typename Container::value_type, std::string>::value, bool>::type = 1>
            static void copy_to_twain_array(DTWAIN_SOURCE theSource, twain_array& ta, int cap_value, const Container& C)
            {
                ta.set_array(API_INSTANCE DTWAIN_ArrayCreateFromCap(theSource, cap_value, C.size()));
                long i = 0;
                std::for_each(C.begin(), C.end(), [&](const std::string& s)
                {
                    API_INSTANCE DTWAIN_ArraySetAtStringA(ta.get_array(), i, s.c_str());
                    ++i;
                });
            }

            template <typename T, typename Container,
                        typename std::enable_if<std::is_same<typename Container::value_type, std::string>::value, bool>::
                        type = 1>
            static void copy_to_twain_array(DTWAIN_SOURCE theSource, twain_array& ta, const Container& C)
            {
                copy_to_twain_array(theSource, ta, T::cap_value, C);
            }

            template <typename Container, typename std::enable_if<
                            std::is_same<typename Container::value_type, twain_frame<>>::value, bool>::type = 1>
            static void copy_to_twain_array(DTWAIN_SOURCE theSource, twain_array& ta, int cap_value, const Container& C)
            {
                ta.set_array(API_INSTANCE DTWAIN_ArrayCreateFromCap(theSource, cap_value, C.size()));
                for (size_t i = 0; i < C.size(); ++i)
                    API_INSTANCE DTWAIN_ArrayFrameSetAt(ta.get_array(), static_cast<LONG>(i), C[i].left, C[i].top, C[i].right, C[i].bottom);
            }

            template <typename T, typename Container, typename std::enable_if<
                            std::is_same<typename Container::value_type, twain_frame<>>::value, bool>::type = 1>
            static void copy_to_twain_array(DTWAIN_SOURCE theSource, twain_array& ta, const Container& C)
            {
                return copy_to_twain_array(theSource, ta, T::cap_value, C);
            }
        };

    }
}
#endif
