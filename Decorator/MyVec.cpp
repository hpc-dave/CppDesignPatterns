#include <concepts>
#include <array>
#include <iostream>
#include <type_traits>
#include <cmath>

template <typename T>
concept VectorBaseContainer =
    requires {
        typename T::value_type;
        typename T::size_type;
    } && requires(const T t) {
        { t.size() } -> std::same_as<typename T::size_type>;
        { t.data() } -> std::same_as<const typename T::value_type*>;
    } && requires(T t) {
        { t.data() } -> std::same_as<typename T::value_type*>;
        { std::bool_constant<(T{}.size(), true)>() } -> std::same_as<std::true_type>;   // check for constexpr
    };

template <typename T, std::size_t N>
using VectorData = std::array<T, N>;

template<typename OS, typename T, std::size_t N>
OS& operator<<(OS& os, const VectorData<T, N>& v) {
    if constexpr(N > 0) {
        for (std::size_t n{0}; n < N - 1; n++)
            os << v.data()[n] << ", ";
        os << v.data()[N - 1];
    }
    return os;
}

// ---- VectorDecorators.h ----
template <VectorBaseContainer Base>
    requires(Base{}.size() >= 1)
class AccessX : public Base {
public:
    using T = typename Base::value_type;
    template <typename... Args>
    constexpr AccessX(Args&&... args)
        : Base{std::forward<Args>(args)...} {}

    constexpr T& x() noexcept { return this->data()[0]; }
    constexpr T x() const noexcept { return this->data()[0]; }
};

template <VectorBaseContainer Base>
    requires( Base{}.size() >= 2)
class AccessY : public Base {
public:
    using T = typename Base::value_type;
    template <typename... Args>
    constexpr AccessY(Args&&... args)
        : Base{std::forward<Args>(args)...} {}

    constexpr T& y() noexcept { return this->data()[1]; }
    constexpr T y() const noexcept { return this->data()[1]; }
};

template <VectorBaseContainer Base>
    requires(Base{}.size() >= 3)
class AccessZ : public Base {
public:
    using T = typename Base::value_type;
    template <typename... Args>
    constexpr AccessZ(Args&&... args)
        : Base{std::forward<Args>(args)...} {}

    constexpr T& z() noexcept { return this->data()[2]; }
    constexpr T z() const noexcept { return this->data()[2]; }
};

template <VectorBaseContainer Base>
    requires(Base{}.size() >= 1)
class AccessI : public Base {
public:
    using T = typename Base::value_type;
    template <typename... Args>
    constexpr AccessI(Args&&... args)
        : Base{std::forward<Args>(args)...} {}

    constexpr T& i() noexcept { return this->data()[0]; }
    constexpr T i() const noexcept { return this->data()[0]; }
};

template <VectorBaseContainer Base>
    requires(Base{}.size() >= 1)
class AccessJ : public Base {
public:
    using T = typename Base::value_type;
    template <typename... Args>
    constexpr AccessJ(Args&&... args)
        : Base{std::forward<Args>(args)...} {}

    constexpr T& j() noexcept { return this->data()[1]; }
    constexpr T j() const noexcept { return this->data()[1]; }
};

template <VectorBaseContainer Base>
    requires(Base{}.size() >= 2)
class AccessK : public Base {
public:
    using T = typename Base::value_type;
    template <typename... Args>
    constexpr AccessK(Args&&... args)
        : Base{std::forward<Args>(args)...} {}

    constexpr T& k() noexcept { return this->data()[0]; }
    constexpr T k() const noexcept { return this->data()[0]; }
};

template <VectorBaseContainer Base, std::size_t N = Base{}.size()>
class AccessXYZ : public AccessXYZ<Base, N - 1> {
public:
    using T = typename Base::value_type;
    template <typename... Args>
    constexpr AccessXYZ(Args&&... args)
        : AccessXYZ<Base, N - 1>{std::forward<Args>(args)...} {}
};

template <VectorBaseContainer Base>
class AccessXYZ<Base, 3> : public AccessXYZ<Base, 2> {
public:
    using T = typename Base::value_type;
    template <typename... Args>
    constexpr AccessXYZ(Args&&... args)
        : AccessXYZ<Base, 2>{std::forward<Args>(args)...} {}

    constexpr T& z() noexcept { return this->data()[2]; }
    constexpr T z() const noexcept { return this->data()[2]; }
};

template <VectorBaseContainer Base>
class AccessXYZ<Base, 2> : public AccessXYZ<Base, 1> {
public:
    using T = typename Base::value_type;
    template <typename... Args>
    constexpr AccessXYZ(Args&&... args)
        : AccessXYZ<Base, 1>{std::forward<Args>(args)...} {}

    constexpr T& y() noexcept { return this->data()[1]; }
    constexpr T y() const noexcept { return this->data()[1]; }
};

template <VectorBaseContainer Base>
class AccessXYZ<Base, 1> : public AccessXYZ<Base, 0> {
public:
    using T = typename Base::value_type;
    template <typename... Args>
    constexpr AccessXYZ(Args&&... args)
        : AccessXYZ<Base, 0>{std::forward<Args>(args)...} {}

    constexpr T& x() noexcept { return this->data()[0]; }
    constexpr T x() const noexcept { return this->data()[0]; }
};

template <VectorBaseContainer Base>
class AccessXYZ<Base, 0> : public Base {
public:
    using T = typename Base::value_type;
    template <typename... Args>
    constexpr AccessXYZ(Args&&... args)
        : Base{std::forward<Args>(args)...} {}
};

namespace vfunc {
template<VectorBaseContainer V>
constexpr V dot(V v1, const V& v2) {
    for (std::size_t n{0}; n < v1.size(); n++)
        v1[n] *= v2[n];
    return v1;
}

template <VectorBaseContainer V>
constexpr typename V::value_type sum(const V& v) {
    typename V::value_type s{0};
    for(const auto& e: v)
        s += e;
    return s;
}

template<VectorBaseContainer V>
constexpr typename V::value_type length(V v) {
    v = vfunc::dot(v, v);
    return std::sqrt(vfunc::sum(v));
}
}

class DoubleVec : public AccessZ<AccessY<AccessX<VectorData<double, 3>>>> {
public:
    using value_type = double;
    using SelfType = DoubleVec;
    using BaseType = AccessZ<AccessY<AccessX<VectorData<double, 3>>>>;

    template <typename... Args>
    constexpr DoubleVec(Args&&... args)
        : BaseType{std::forward<Args>(args)...} {}

    template<VectorBaseContainer V>
    constexpr SelfType dot(const V& v) const {
        return vfunc::dot(*this, v);
    }

    template <VectorBaseContainer V>
    constexpr value_type length() const {
        return vfunc::length(*this);
    }
};

int main() {
    using MyVec3 = AccessZ<AccessY<AccessX<VectorData<double, 3>>>>;
    using MyIndex = AccessK<AccessJ<AccessI<VectorData<int, 3>>>>;

    using MyXYZVec = AccessXYZ<VectorData<double, 3>>;
    using MyXYVec = AccessXYZ<VectorData<double, 2>>;

    // constexpr MyVec3 myvec{0., 1., 2.};
    // constexpr auto res = vfunc::dot(myvec, myvec);

    constexpr MyXYZVec myxyzvec{0.5, 1.5, 2.5};
    std::cout << myxyzvec.x() << std::endl;

    constexpr MyXYVec myxyvec{0.5, 1.5};
    std::cout << myxyvec.y() << std::endl;

    MyVec3 myvec{0., 1., 2.};
    auto resdot = vfunc::dot(myvec, myvec);
    MyIndex myind{2, 3, 4};

    std::cout << myvec << std::endl;
    std::cout << myind << std::endl;

    MyVec3 myvec2{1., 2., 3.};
    MyVec3 mydot = vfunc::dot(myvec, myvec2);
    std::cout << mydot << std::endl;

    std::cout << vfunc::length(myvec2) << std::endl;

    DoubleVec mydoublevec{-1., -2., -3.};
    std::cout << mydoublevec << std::endl;
}
