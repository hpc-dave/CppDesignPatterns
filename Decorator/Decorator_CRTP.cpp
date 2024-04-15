
#include <concepts>
#include <cstdint>
#include <ostream>
#include <type_traits>
// Have a look at the curiously recurring template pattern (CRTP): https://en.wikipedia.org/wiki/Curiously_recurring_template_pattern
// And note, that this was discovered by accident, this is not a specialized compiler feature :D

template <typename T>
concept arithmetic = std::is_arithmetic_v<T>;

class Money {
public:
    Money() = default;

    template <std::integral T>  // C++20 concept
    constexpr explicit Money(T v)
        : value_{static_cast<std::int64_t>(v)} {}

    template <std::floating_point T>  // C++20 concept
    constexpr explicit Money(T v)
        : value_{static_cast<std::int64_t>(v * 100.0)} {}

    constexpr float as_float() const { return value_ * 0.01F; }
    constexpr std::int64_t as_int64_t() const { return value_; }

private:
    std::int64_t value_{};
};

template <arithmetic T>  // C++20 concept
constexpr Money operator*(Money money, T factor) {
    return Money{static_cast<std::int64_t>(money.as_int64_t() * factor)};
}

template <arithmetic T>  // C++20 concept
constexpr Money operator*(T factor, Money money) {
    return Money{static_cast<std::int64_t>(factor * money.as_int64_t())};
}

constexpr Money operator+(Money lhs, Money rhs) {
    return Money{lhs.as_int64_t() + rhs.as_int64_t()};
}

constexpr Money operator-(Money lhs, Money rhs) {
    return Money{lhs.as_int64_t() - rhs.as_int64_t()};
}

constexpr bool operator==(Money lhs, Money rhs) {
    return lhs.as_int64_t() == rhs.as_int64_t();
}

constexpr bool operator!=(Money lhs, Money rhs) {
    return !(lhs == rhs);
}

constexpr bool operator<(Money lhs, Money rhs) {
    return lhs.as_int64_t() < rhs.as_int64_t();
}

std::ostream& operator<<(std::ostream& os, Money money) {
    return os << money.as_float();
}

// ---- Item.h ------
// #include <Money>
template<typename Derived>
class Item {
protected:
    ~Item() = default;
public:
    Money price() const { return derived().price(); }
    std::string name() const { return derived().name(); }

    Derived& derived() { return static_cast<Derived&>(*this); }
    const Derived& derived() const { return static_cast<const Derived&>(*this); }
};

// ---- Coffee.h -----
// #include <Item>
#include <string>

class Coffee : public Item<Coffee> {
public:
    Coffee(std::string name, Money price)
        : name_{std::move(name)}, price_{std::move(price)} {
    }

    const std::string& name() const { return name_; }
    Money price() const { return price_; }

private:
    std::string name_;
    Money price_;
};

// ---- Milk.h ----
// #include <Item>
#include <memory>
#include <stdexcept>
template<typename TItem>
class Milk : public Item<Milk<TItem>> {
public:
    template<typename... Args>
    Milk(Args&&... args)
        : item_(std::forward<Args>(args)...),
          milk_surcharge_{0.2} {
    }

    Money price() const { return item_.price() + milk_surcharge_; }

private:
    TItem item_;
    Money milk_surcharge_;
};

// ---- Tax.h ----
// #include <DecoratedItem>
#include <memory>
#include <stdexcept>
template <typename TItem>
    class Tax : public Item <Tax<TItem>> {
public:
    template <typename... Args>
    Tax(double factor, Args&&... args)
        : item_(std::forward<Args>(args)...),
          factor_{1. + factor} {
    }

    Money price() const { return item_.price() * factor_; }

private:
    TItem item_;
    double factor_;
};

#include <iostream>
int main() {
    Tax<Milk<Coffee>> espresso(0.19, "Espresso", Money{1.0});
    std::cout << "Espresso: " << espresso.price() << '\n';
}
