#ifndef EITHER_TEMPLATE_H
#define EITHER_TEMPLATE_H

template<class L, class R>
class Either {

public:
    explicit Either(const L &left);
    explicit Either(const R &right);

    bool isLeft() const;
    L getLeft() const;
    bool isRight() const;
    R getRight() const;

private:
    bool m_isLeft;
    L m_left;
    R m_right;
};

template<class L, class R>
Either<L,R>::Either(const L &left)
{
    m_isLeft = true;
    m_left = left;
}

template<class L, class R>
Either<L,R>::Either(const R &right)
{
    m_isLeft = false;
    m_right = right;
}

template<class L, class R>
bool
Either<L,R>::isLeft() const
{
    return m_isLeft;
}

template<class L, class R>
L
Either<L,R>::getLeft() const
{
    return m_left;
}

template<class L, class R>
bool
Either<L,R>::isRight() const
{
    return !m_isLeft;
}

template<class L, class R>
R
Either<L,R>::getRight() const
{
    return m_right;
}

//template<class L, class R>
//QDebug operator<<(QDebug debug, const Either<L,R> &either) {
//    QDebugStateSaver saver(debug);
//    if (either.isLeft()) {
//        debug.nospace().noquote() << "Either(left=" << either.getLeft() << ")";
//    } else {
//        debug.nospace().noquote() << "Either(right=" << either.getRight() << ")";
//    }
//    return debug;
//}

template<class L, class R>
inline bool operator==(const Either<L,R> &lhs, const Either<L,R> &rhs)
{
    if (lhs.isLeft() && rhs.isLeft())
        return lhs.getLeft() == rhs.getLeft();
    if (lhs.isRight() && rhs.isRight())
        return lhs.getRight() == rhs.getRight();
    return false;
}

template<class L, class R>
inline bool operator!=(const Either<L,R> &lhs, const Either<L,R> &rhs)
{
    return !(lhs == rhs);
}

#endif // EITHER_TEMPLATE_H
