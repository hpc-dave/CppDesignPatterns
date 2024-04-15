

#include <concepts>
#include <cstdint>
#include <ostream>
#include <type_traits>

template <typename T>
concept arithmetic = std::is_arithmetic_v<T>;

class Money {
public:
    Money() = default;

    template< std::integral T >  // C++20 concept
    constexpr explicit Money(T v)
        : value_{static_cast<std::int64_t>(v)} {}

    template< std::floating_point T >  // C++20 concept
    constexpr explicit Money(T v)
        : value_{static_cast<std::int64_t>(v * 100.0)} {}

    constexpr float as_float() const { return value_ * 0.01F; }
    constexpr std::int64_t as_int64_t() const { return value_; }

private:
    std::int64_t value_{};
};

template< arithmetic T >  // C++20 concept
constexpr Money operator*(Money money, T factor) {
    return Money{static_cast<std::int64_t>(money.as_int64_t() * factor)};
}

template< arithmetic T >  // C++20 concept
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

class Item {
public:
    virtual ~Item() = default;
    virtual Money price() const = 0;
};

// ---- Coffee.h -----
// #include <Item>
#include <string>

class Coffee : public Item {
public:
    Coffee(std::string name, Money price)
        : name_{std::move(name)}, price_{std::move(price)}{
        }

    const std::string& GetName() const { return name_; }
    Money price() const override { return price_; }

private:
    std::string name_;
    Money price_;
};

// ---- DecoratedItem -----
// #include <Item>
#include <memory>
#include <stdexcept>
class DecoratedItem : public Item {
public:
    DecoratedItem(std::unique_ptr<Item>&& item)
        : item_(std::move(item)) {
        if(!item_){
            throw std::invalid_argument("Invalid item");
        }
    }

protected:
    Item& item() { return *item_; }
    const Item& item() const { return *item_; }

private:
    std::unique_ptr<Item> item_;
};

// ---- Milk.h ----
// #include <DecoratedItem>
#include <memory>
#include <stdexcept>
class Milk : public DecoratedItem {
public:
    Milk(std::unique_ptr<Item>&& item)
        : DecoratedItem(std::move(item)), milk_surcharge_{0.2} {
    }

    Money price() const override {
        return item().price() + milk_surcharge_;
    }

private:
    Money milk_surcharge_;
};

// ---- Tax.h ----
// #include <DecoratedItem>
#include <memory>
#include <stdexcept>
class Tax : public DecoratedItem {
public:
    Tax(double factor, std::unique_ptr<Item>&& item)
        : DecoratedItem(std::move(item)), factor_{1. + factor} {
    }

    Money price() const override {
        return item().price() * factor_;
    }

private:
    double factor_;
};

#include <iostream>
int
main() {
    std::unique_ptr<Item> espresso(
        std::make_unique<Tax>(0.19,
            std::make_unique<Milk>(
                std::make_unique<Coffee>("Espresso", Money{1.}))));
    std::cout << "Espresso: " << espresso->price() << '\n';
}