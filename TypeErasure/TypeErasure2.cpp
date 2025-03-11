#include <iostream>
#include <memory>
#include <utility>
#include <vector>

struct Point {
    explicit Point(double x = 0., double y = 0., double z = 0.)
        : x_(x), y_(y), z_(z) {
    }
    double x_;
    double y_;
    double z_;

    template <typename OS>
    friend OS& operator<<(OS&, const Point&);
};

template <typename OS>
OS& operator<<(OS& os, const Point& point) {
    os << '[' << point.x_ << ' ' << point.y_ << ' ' << point.z_ << ']';
    return os;
}

class Sphere {
public:
    explicit Sphere(double r, Point c = Point{})
        : radius(r), center(c) {}

    double GetRadius() const { return radius; }

    Point GetCenter() const { return center; }

    void GetInfo() const {
        std::cout << "I am a Sphere at point " << center << " with radius " << radius << std::endl;
    }

private:
    double radius;
    Point center;
};

void free_get_info(const Sphere& s) {
    std::cout << "I am a Sphere at point " << s.GetCenter() << " with radius " << s.GetRadius() << std::endl;
}


class Box {
public:
    explicit Box(double w, double l, double h, Point c = Point{})
        : width(w), length(l), height(h), center(c) {
    }
    double GetWidth() const { return width; }
    double GetLength() const { return length; }
    double GetHeight() const { return height; }

    Point GetCenter() const { return center; }

    void GetInfo() const {
        std::cout << "I am a Box at point " << center << " with width = "
                  << width << ", length = " << length << " and height = " << height << std::endl;
    }

private:
    double width;
    double length;
    double height;
    Point center;
};

void free_get_info(const Box& b) {
    std::cout << "I am a Box at point " << b.GetCenter() << " with width = "
              << b.GetWidth() << ", length = " << b.GetLength() << " and height = " << b.GetHeight() << std::endl;
}

class Object {
public:
    template<typename TObject>
    Object(const TObject& object)
        : pimpl(std::make_unique<Model<TObject>>(object)){}

    Object(const Object& other):
        pimpl(other.pimpl->clone()) {}

    Object& operator=(Object other) {
        std::swap(pimpl, other.pimpl);
        return *this;
    }

    ~Object() = default;
    Object(Object&&) = default;
    Object& operator=(Object&&) = default;

private:
    friend void free_get_info(const Object& object){
        object.pimpl->get_info();
    }

    class Concept {
    public:
        virtual ~Concept() = default;
        virtual void get_info() const = 0;
        virtual std::unique_ptr<Concept> clone() const = 0;  // Prototype
    };

    template <typename TObject>
    class Model : public Concept {
    public:
        Model(const TObject& o) : object{o} {}

        void get_info() const override {
            free_get_info(object);
        }

        std::unique_ptr<Concept> clone() const override {
            return std::make_unique<Model>(*this);
        }

    private:
        TObject object;
    };

private:
    std::unique_ptr<Concept> pimpl;
};

using Objects = std::vector<Object>;

void GetAllInfo(const Objects& objects) {
    for (const auto& object : objects) {
        free_get_info(object);
    }
}

int main() {
    Objects objects;
    objects.emplace_back(Sphere{1.0});
    objects.emplace_back(Box{0.1, 0.2, 0.3, Point{}});
    objects.emplace_back(Sphere{2.5, Point{1., 2., 3.}});

    GetAllInfo(objects);
}

// compile with g++ -o TypeErasure2 TypeErasure2.cpp
