#include <concepts>
#include <cstdlib>
#include <type_traits>
#include <vector>
#include <set>
#include <iostream>

template <typename T>
concept AssociativeContainer =
    std::is_class_v<T> &&
    std::regular<T> &&
    requires {
        typename T::value_type;
        typename T::key_type;
        typename T::iterator;
        typename T::const_iterator;
    } &&
    requires(T cont, T::value_type value) {  // the typename keyword is optional in this context
        { cont.begin() } -> std::same_as<typename T::iterator>;
        { cont.end() } -> std::same_as<typename T::iterator>;
        cont.insert(value);
        cont.find(std::declval<typename T::key_type>());
    } &&
    requires(T const cont) {
        { cont.size() } -> std::convertible_to<size_t>;  // or alternatively std::integral
        { cont.empty() } -> std::same_as<bool>;
        { cont.begin() } -> std::same_as<typename T::const_iterator>;
        { cont.end() } -> std::same_as<typename T::const_iterator>;
        { cont.cbegin() } -> std::same_as<typename T::const_iterator>;
        { cont.cend() } -> std::same_as<typename T::const_iterator>;
    };

template <typename T>
struct IsAssociativeContainer : std::bool_constant<AssociativeContainer<T>> {};

template <typename T>
constexpr bool IsAssociativeContainer_v = IsAssociativeContainer<T>::value;


template<typename T, typename V>
void addElement(T& container, const V& value) {
    std::cout << "Adding to Container\n";
    container.push_back(value);
}

template <typename T, typename V>
    requires AssociativeContainer<T>
void addElement(T& container, const V& value) {
    std::cout << "Adding to Associative Container\n";
    container.insert(value);
}

template <typename T>
void print(T const& container) {
    std::cout << "\n (";
    for (auto const& element : container) {
        std::cout << " " << element;
    }
    std::cout << " )\n\n";
}

int main() {
    std::vector<int> v{};
    std::set<int> s{};

    for (int i = 0; i < 10; ++i) {
        addElement(v, i);
        addElement(s, i);
    }

    print(v);
    print(s);

    return EXIT_SUCCESS;
}

// compile with g++ --std=c++20 -o exec AssociativeContainer.cpp
