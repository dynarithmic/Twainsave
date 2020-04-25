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
template <typename T=long, typename std::enable_if<
std::is_floating_point<typename T>::value ||
std::is_integral<typename T>::value, bool>::type = 1>
class twain_range
{
    template <typename T2, typename std::enable_if<std::is_floating_point<T2>::value, bool>::type = 1>
    static bool is_close_to(T2 value1, T2 value2)
    {
        return fabs(value1 - value2) <= 1.0e-8;
    }

    template <typename T2, typename std::enable_if<std::is_integral<T2>::value, bool>::type = 1>
    static bool is_close_to(T2 value1, T2 value2)
    {
        return value1 == value2;
    }

    std::array<T, 5> m_allValues;
    bool m_isValid;
    public:
        twain_range() : m_isValid(false) {}
        twain_range(T low, T high, T step, T current = T(), T defaultVal = T())
        {
            m_allValues[0] = low;
            m_allValues[1] = high;
            m_allValues[2] = step;
            m_allValues[3] = current;
            m_allValues[4] = defaultVal;
            m_isValid = is_valid_range(m_allValues);
        }

        template <typename Iter>
        twain_range(Iter it1, Iter it2) : m_allValues{}
        {
            auto min_dist = std::min(m_allValues.size(), std::distance(it1, it2));
            std::copy(it1, it1 + min_dist, m_allValues.begin());
            m_isValid = is_valid_range(m_allValues);
        }

        template <typename Container,
                typename std::enable_if<
                std::is_same<typename Container::value_type, T>::value, bool>::type = 1>
                twain_range(const Container& ct)
        {
            std::copy(ct.begin(), ct.begin() + std::min(m_allValues.size(), ct.size()), m_allValues.begin());
            m_isValid = is_valid_range(m_allValues);
        }

        template <typename Container>
        static bool is_valid_range(const Container& c)
        {
            auto iter = c.begin();
            auto low = *iter;
            iter = std::next(iter);
            auto high = *iter;
            iter = std::next(iter);
            auto step = *std::next(iter);
            if (low > high)
                    return false;
            if (step < 0)
                    return false;
            if (is_close_to(step, Container::value_type(0)) && low < high)
                    return false;
            return true;
        }

        template <typename Iter>
        static bool is_valid_range(Iter iter, Iter iter2)
        {
            if (iter == iter2)
                    return false;
            auto low = *iter;
            auto iterNext = std::next(iter);
            if (iterNext == iter2)
                    return false;
            auto high = *iterNext;
            iterNext = std::next(iterNext);
            if (iterNext == iter2)
                    return false;
            auto step = *iterNext;
            if (low > high)
                    return false;
            if (step < 0)
                    return false;
            if (is_close_to(step, Container::value_type(0)) && low < high)
                    return false;
            return true;
        }

        T get_min() const { return m_allValues[0]; }
        T get_max() const { return m_allValues[1]; }
        T get_step() const { return m_allValues[2]; }
        T get_current() const { return m_allValues[3]; }
        T get_default() const { return m_allValues[4]; }

        void set_min(const T& val) { m_allValues[0] = val; }
        void set_max(const T& val) { m_allValues[1] = val; }
        void set_step(const T& val) { m_allValues[2] = val; }
        void set_current(const T& val) { m_allValues[3] = val; }
        void set_default(const T& val) { m_allValues[4] = val; }

        size_t get_expand_count() const
        {
            if (!m_isValid)
                    return 0;
            return static_cast<size_t>(std::abs(m_allValues[1] - m_allValues[0]) / m_allValues[2] + 1);
        }

        template <typename Container, typename std::enable_if<
            std::is_floating_point<typename Container::value_type>::value ||
            std::is_integral<typename Container::value_type>::value, bool>::type = 1>
        bool expand_range(Container& ct) const
        {
            if (!m_isValid)
                    return false;
            ct.clear();
            size_t gc = get_expand_count();
            if (gc == 0)
                    return false;
            for (size_t start = 0; start < gc; ++start)
                    ct.insert(ct.end(), m_allValues[0] + (m_allValues[2] * start));
            return true;
        }
};

