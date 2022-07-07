#ifndef MULTIPOINTER_MULTIPOINTER_H
#define MULTIPOINTER_MULTIPOINTER_H

#include <vector>

namespace Utility {

    template<class T>
    class MultiPointer
    {
        using type = T;
        using typePtr = T *;
        using typeConstPtr = const T *;
        using typeRef = T &;
        using typeConstRef = const T &;

    public:

        template<typename... Ts>
        explicit MultiPointer(Ts&&... ts);

        template<typename U, typename... Us>
        void add(U&& first, Us&&... rest);

        template<typename U, typename... Us>
        [[maybe_unused]]
        void remove(U&& first, Us&&... rest);

        type& operator*() const;

        MultiPointer<type>& operator =([[maybe_unused]] typeConstRef other);

    private:
        std::vector<typePtr> m_data;

        void add();

        void remove();

        void add_in(typePtr ptr);

        void add_in(typeRef ref);

        void remove_in(typePtr ptr);

        void remove_in(typeRef ref);
    };















    template<typename T>
    template<typename... Ts>
    MultiPointer<T>::MultiPointer(Ts&&... ts)
    {
        add(std::forward<Ts>(ts)...);
    }

    template<typename T>
    template<typename U, typename... Us>
    void MultiPointer<T>::add(U&& first, Us&&... rest)
    {
        static_assert(std::same_as<U&&, typeRef> || std::same_as<U&&, typePtr&&>, "Provided type not assignable");
        add_in(first);
        add(std::forward<Us>(rest)...);
    }

    template<typename T>
    template<typename U, typename... Us>
    void MultiPointer<T>::remove(U&& first, Us&&... rest)
    {
        static_assert(std::same_as<U&&, typeRef> || std::same_as<U&&, typePtr&&>, "Provided type not assignable");
        remove_in(first);
        remove(std::forward<Us>(rest)...);
    }

    template<typename T>
    T& MultiPointer<T>::operator*() const
    {
        return *m_data[0];
    }

    template<typename T>
    MultiPointer<T>& MultiPointer<T>::operator =([[maybe_unused]] typeConstRef other)
    {
        for (auto iter : m_data)
        {
            *iter = other;
        }

        return *this;
    }

    template<typename T>
    void MultiPointer<T>::add()
    {}

    template<typename T>
    void MultiPointer<T>::remove()
    {}

    template<typename T>
    void MultiPointer<T>::add_in(typePtr ptr)
    {
        if (std::find(m_data.begin(), m_data.end(), ptr) != m_data.end()) return;

        if (!m_data.empty())
        {
            *ptr = *m_data[0];
        }
        m_data.push_back(ptr);
    }

    template<typename T>
    void MultiPointer<T>::add_in(typeRef ref)
    {
        if (std::find(m_data.begin(), m_data.end(), &ref) != m_data.end()) return;

        if (!m_data.empty())
        {
            ref = *m_data[0];
        }

        m_data.push_back(&ref);
    }

    template<typename T>
    void MultiPointer<T>::remove_in(typePtr ptr)
    {
        m_data.erase(std::remove(m_data.begin(), m_data.end(), ptr), m_data.end());
    }

    template<typename T>
    void MultiPointer<T>::remove_in(typeRef ref)
    {
        m_data.erase(std::remove(m_data.begin(), m_data.end(), &ref), m_data.end());
    }
}

#endif //MULTIPOINTER_MULTIPOINTER_H
