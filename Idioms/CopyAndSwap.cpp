// using ADL (Argument-dependent name lookup)

#include <array>
#include <vector>
#include <iostream>

namespace ns {
class Foo {
public:
    Foo() {}

    Foo(const double* arr_d, const std::array<double, 4> arr_s, const std::vector<double>& v)
        : arr_std(arr_s), vec(v) {
        std::copy(arr_d, arr_d + 4, arr_dumb);
    }

    Foo(const Foo& other) : Foo(other.arr_dumb, other.arr_std, other.vec) {}
    
    Foo(Foo&& other) noexcept : Foo() {
        swap(*this, other);
    }

    virtual ~Foo(){}

    Foo& operator=(Foo other) noexcept {
        swap(*this, other);
        return *this;
    }

    friend void swap(Foo& first, Foo& second) {
        std::cout << "Hi there, I am doing swappydi-swappy things!\n";
        using std::swap;
        swap(first.arr_dumb, second.arr_dumb);
        swap(first.arr_std, second.arr_std);
        swap(first.vec, second.vec);
    }

    void print(){
        for (int i{0}; i < 4; i++){
            std::cout << arr_dumb[i] << '\t';
        }
        std::cout << '\n';
        for (int i{0}; i < 4; i++) {
            std::cout << arr_std[i] << '\t';
        }
        std::cout << '\n';
        for (auto e : vec) {
            std::cout << e << '\t';
        }
        std::cout << std::endl;
    }

private:
    double arr_dumb[4];
    std::array<double, 4> arr_std;
    std::vector<double> vec;
};
}

int main() {
    using std::swap;
    double arr[] = {1., 2., 3., 4.};
    std::array<double, 4> arr_std = {0.1, 0.2, 0.3, 0.4};
    std::vector<double> vec = {-1., -2., -3., -4.};

    std::cout << "Regular Initialization\n";
    ns::Foo foo{arr, arr_std, vec};
    foo.print();

    std::cout << "\nCopy Initialization\n";
    ns::Foo foo2(foo);
    foo2.print();

    std::cout << "\nCopy Assignment Initialization\n";
    double arr2[] = {10., 20., 30., 40.};
    std::array<double, 4> arr2_std = {0.11, 0.22, 0.33, 0.44};
    std::vector<double> vec2 = {-10., -20., -30., -40.};
    ns::Foo foo3{arr2, arr2_std, vec2};
    std::cout << "Initializing another instance:\n";
    foo3.print();
    std::cout << "Conducting Assignment\n";
    foo2 = foo3;
    foo2.print();

    std::cout << "\nConducting Swap\n";
    std::cout << "Before:\n";
    foo.print();
    foo2.print();
    swap(foo2, foo);
    std::cout << "After:\n";
    foo.print();
    foo2.print();
}

// compile with g++ -o copy_and_swap CopyAndSwap.cpp
