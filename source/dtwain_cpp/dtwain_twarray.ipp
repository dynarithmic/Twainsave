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
    DTWAIN_ARRAY m_theArray;
    bool m_isRange = false;
    
    public:
        twain_array();
        twain_array(DTWAIN_ARRAY a);
        twain_array(const twain_array& rhs);
        twain_array(twain_array&& rhs) noexcept;
        twain_array& operator=(twain_array&& rhs) noexcept;
        twain_array& operator=(const twain_array& rhs);
        void swap(twain_array& t1, twain_array& t2) const;
        void set_array(DTWAIN_ARRAY a);
        void resize(size_t n) const;
        DTWAIN_ARRAY get_array() const;
        LPDTWAIN_ARRAY get_array_ptr();
        ~twain_array();
        long get_count() const;
        long get_expanded_count() const;
        bool is_range() const;
        bool expand_range_replace();
        bool empty() const;
        template <typename Container>
        bool expand_range(Container& ct) const;

        template <typename T>
        T* get_buffer()
        {
            if (m_theArray) return reinterpret_cast<T*>(DTWAIN_ArrayGetBuffer(m_theArray, 0));
            return nullptr;
        }
};

// copying traits for the twain_array
struct twain_array_copy_traits
{
    template <typename Container, typename std::enable_if<
                    std::is_floating_point<typename Container::value_type>::value ||
                    std::is_integral<typename Container::value_type>::value, bool>::type = 1>
    static void copy_from_twain_array(twain_array& ta, long sz, Container& C)
    {
        auto pBuffer = ta.get_buffer<typename Container::value_type>();
        std::copy(pBuffer, pBuffer + sz, std::inserter(C, std::end(C)));
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
        std::vector<char> v_char(DTWAIN_ArrayGetMaxStringLength(ta.get_array()) + 1);
        if (v_char.empty())
            return;
        for (long i = 0; i < sz; ++i)
        {
            DTWAIN_ArrayGetAtStringA(ta.get_array(), i, &v_char[0]);
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
                    std::is_same<typename Container::value_type, twain_frame>::value, bool>::type = 1>
    static void copy_from_twain_array(const twain_array& ta, long sz, Container& C)
    {
        std::vector<twain_frame> vFrm(1);
        for (long i = 0; i < sz; ++i)
        {
            DTWAIN_ArrayFrameGetAt(ta.get_array(), i, &(vFrm[0].left), &(vFrm[0].top), &(vFrm[0].right),
                                    &(vFrm[0].bottom));
            std::copy(vFrm.begin(), vFrm.end(), std::inserter(C, C.end()));
        }
    }

    template <typename Container, typename std::enable_if<
                    std::is_same<typename Container::value_type, twain_frame>::value, bool>::type = 1>
    static void copy_from_twain_array(const twain_array& ta, Container& C)
    {
        copy_from_twain_array(ta, ta.get_count(), C);
    }

    template <typename Container, typename std::enable_if<
                    std::is_floating_point<typename Container::value_type>::value ||
                    std::is_integral<typename Container::value_type>::value, bool>::type = 1>
    static void copy_to_twain_array(DTWAIN_SOURCE theSource, twain_array& ta, int cap_value, const Container& C)
    {
        ta.set_array(DTWAIN_ArrayCreateFromCap(theSource, cap_value, C.size()));
        auto pBuffer = reinterpret_cast<typename Container::value_type*>(DTWAIN_ArrayGetBuffer(
            ta.get_array(), 0));
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
        ta.set_array(DTWAIN_ArrayCreateFromCap(theSource, cap_value, C.size()));
        long i = 0;
        std::for_each(C.begin(), C.end(), [&](const std::string& s)
        {
            DTWAIN_ArraySetAtStringA(ta.get_array(), i, s.c_str());
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
                    std::is_same<typename Container::value_type, twain_frame>::value, bool>::type = 1>
    static void copy_to_twain_array(DTWAIN_SOURCE theSource, twain_array& ta, int cap_value, const Container& C)
    {
        ta.set_array(DTWAIN_ArrayCreateFromCap(theSource, cap_value, C.size()));
        for (size_t i = 0; i < C.size(); ++i)
            DTWAIN_ArrayFrameSetAt(ta.get_array(), i, C[i].left, C[i].top, C[i].right, C[i].bottom);
    }

    template <typename T, typename Container, typename std::enable_if<
                    std::is_same<typename Container::value_type, twain_frame>::value, bool>::type = 1>
    static void copy_to_twain_array(DTWAIN_SOURCE theSource, twain_array& ta, const Container& C)
    {
        return copy_to_twain_array(theSource, ta, T::cap_value, C);
    }
};

template <typename Container>
bool twain_array::expand_range(Container& ct) const
{
    if (m_isRange)
    {
        DTWAIN_ARRAY temp;
        if (DTWAIN_RangeExpand(m_theArray, &temp))
        {
            twain_array temp_arr(temp);
            twain_array_copy_traits::copy_from_twain_array(temp_arr, ct);
            return true;
        }
    }
    return false;
}
