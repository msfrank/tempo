//#ifndef TEMPO_UTILS_PATH_TEMPLATE_H
//#define TEMPO_UTILS_PATH_TEMPLATE_H
//
//#include <vector>
//
//namespace tempo_utils {
//
//    template<class T>
//    class Path {
//    public:
//        Path();
//        explicit Path(const std::vector<T> &parts);
//        explicit Path(std::initializer_list<T> &parts);
//        Path(const Path<T> &init, const T &last);
//        Path(const std::vector<T> &init, const T &last);
//        Path(const Path<T> &other);
//        Path(Path<T> &&other) noexcept;
//
//        Path<T> &operator=(const Path<T> &other);
//        Path<T> &operator=(Path<T> &&other) noexcept;
//
//        bool isEmpty() const;
//
//        int numParts() const;
//        T getPart(int index) const;
//        T getHead() const;
//        T getLast() const;
//
//        Path<T> getInit() const;
//        Path<T> getTail() const;
//
//        bool isParentOf(const Path<T> &other) const;
//        bool isChildOf(const Path<T> &other) const;
//        bool isAncestorOf(const Path<T> &other) const;
//        bool isDescendentOf(const Path<T> &other) const;
//
//        bool operator==(const Path<T> &other) const;
//        bool operator!=(const Path<T> &other) const;
//
//        template <typename H>
//        friend H AbslHashValue(H h, const Path<T> &path) {
//            return H::combine(std::move(h), *path.m_path);
//        }
//
//    private:
//        std::shared_ptr<std::vector<T>> m_path;
//        int m_pos;
//        int m_count;
//
//        Path(std::shared_ptr<const std::vector<T>> path, int pos, int count);
//    };
//
//    template<class T>
//    Path<T>::Path() : m_pos(0), m_count(0)
//    {
//    }
//
//    template<class T>
//    Path<T>::Path(std::shared_ptr<const std::vector<T>> path, int pos, int count)
//        : m_path(path),
//          m_pos(pos),
//          m_count(count)
//    {
//    }
//
//    template<class T>
//    Path<T>::Path(const std::vector<T> &parts)
//        : m_path(std::make_shared<std::vector<T>>(parts)),
//          m_pos(0),
//          m_count(parts.size())
//    {
//    }
//
//    template<class T>
//    Path<T>::Path(std::initializer_list<T> &parts)
//        : m_path(std::make_shared<std::vector<T>>(parts.begin(), parts.end())),
//          m_pos(0),
//          m_count(parts.size())
//    {
//    }
//
//    template<class T>
//    Path<T>::Path(const Path<T> &init, const T &last)
//        : Path(init)
//    {
//        m_path->push_back(last);
//        m_count++;
//    }
//
//    template<class T>
//    Path<T>::Path(const std::vector<T> &init, const T &last)
//        : Path(init)
//    {
//        m_path->push_back(last);
//        m_count++;
//    }
//
//    template<class T>
//    Path<T>::Path(const Path<T> &other)
//        : m_path(other.m_path),
//          m_pos(other.m_pos),
//          m_count(other.m_count)
//    {
//    }
//
//    template<class T>
//    Path<T>::Path(Path<T> &&other) noexcept
//    {
//        m_path = std::move(other.m_path);
//        m_pos = other.m_pos;
//        m_count = other.m_count;
//        other.m_pos = 0;
//        other.m_count = 0;
//    }
//
//    template<class T>
//    Path<T> &
//    Path<T>::operator=(const Path<T> &other)
//    {
//        if (this != &other) {
//            m_path = other.m_path;
//            m_pos = other.m_pos;
//            m_count = other.m_count;
//        }
//        return *this;
//    }
//
//    template<class T>
//    Path<T> &
//    Path<T>::operator=(Path<T> &&other) noexcept
//    {
//        if (this != &other) {
//            m_path = std::move(other.m_path);
//            m_pos = other.m_pos;
//            m_count = other.m_count;
//            other.m_pos = 0;
//            other.m_count = 0;
//        }
//        return *this;
//    }
//
//    template<class T>
//    bool
//    Path<T>::isEmpty() const
//    {
//        return m_count == 0;
//    }
//
//    template<class T>
//    int
//    Path<T>::numParts() const
//    {
//        return m_count;
//    }
//
//    template<class T>
//    T
//    Path<T>::getPart(int index) const
//    {
//        if (0 <= index && index < m_count)
//            return m_path->at(m_pos + index);
//        return {};
//    }
//
//    template<class T>
//    T
//    Path<T>::getHead() const
//    {
//        if (m_count == 0)
//            return {};
//        return m_path->at(m_pos);
//    }
//
//    template<class T>
//    T
//    Path<T>::getLast() const
//    {
//        if (m_count == 0)
//            return {};
//        return m_path->at(m_pos + (m_count - 1));
//    }
//
//    template<class T>
//    Path<T>
//    Path<T>::getInit() const
//    {
//        if (m_count == 0)
//            return {};
//        return Path(m_path, m_pos, m_count - 1);
//    }
//
//    template<class T>
//    Path<T>
//    Path<T>::getTail() const
//    {
//        if (m_count == 0)
//            return {};
//        return Path(m_path, m_pos + 1, m_count - 1);
//    }
//
//    template<class T>
//    bool
//    Path<T>::isParentOf(const Path<T> &other) const
//    {
//        if (m_count != other.m_count - 1)
//            return false;
//        for (int i = 0; i < m_count; i++) {
//            if (m_path->at(m_pos + i) != other.m_path->at(other.m_pos + i))
//                return false;
//        }
//        return true;
//    }
//
//    template<class T>
//    bool
//    Path<T>::isChildOf(const Path<T> &other) const
//    {
//        return other.isParentOf(*this);
//    }
//
//    template<class T>
//    bool
//    Path<T>::isAncestorOf(const Path<T> &other) const
//    {
//        if (other.m_count <= m_count)
//            return false;
//        for (int i = 0; i < m_count; i++) {
//            if (m_path->at(m_pos + i) != other.m_path->at(other.m_pos + i))
//                return false;
//        }
//        return true;
//    }
//
//    template<class T>
//    bool
//    Path<T>::isDescendentOf(const Path<T> &other) const
//    {
//        return other.isAncestorOf(*this);
//    }
//
//    template<class T>
//    bool
//    Path<T>::operator==(const Path<T> &other) const
//    {
//        if (m_count != other.m_count)
//            return false;
//        for (int i = 0; i < m_count; i++) {
//            if (m_path->at(m_pos + i) != other.m_path->at(other.m_pos + i))
//                return false;
//        }
//        return true;
//    }
//
//    template<class T>
//    bool
//    Path<T>::operator!=(const Path<T> &other) const
//    {
//        return !(*this == other);
//    }
//}
//
//#endif // TEMPO_UTILS_PATH_TEMPLATE_H