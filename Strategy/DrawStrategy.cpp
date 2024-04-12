
#include <iostream>

// ---- Point.h ----
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

// ---- Object.h ----
class Object {
public:
    virtual ~Object() = default;
    virtual void draw() const = 0;
};

// ---- Sphere.h ----
// #include <Point.h>
#include <memory>
#include <functional>
#include <stdexcept>
class Sphere : public Object {
public:
    using DrawStrategy = std::function<void(const Sphere&)>;
    explicit Sphere(double radius, Point center, DrawStrategy drawer)
        : radius_(radius),
          center_(center),
          drawer_(std::move(drawer)) {
            if (!drawer_) {
                throw std::invalid_argument("Invalid draw strategy");
            }
        }

    void draw() const override { drawer_(*this); }

    double GetRadius() const { return radius_; }

    Point GetCenter() const { return center_; }

private:
    double radius_;
    Point center_;
    DrawStrategy drawer_;
};

// ---- Box.h ----
// #include <Point.h>
class Box : public Object {
public:
    using DrawStrategy = std::function<void(const Box&)>;
    explicit Box(double width, double length, double height, Point center, DrawStrategy drawer)
        : width_(width), length_(length), height_(height), center_(center), drawer_(std::move(drawer)) {
    }

    void draw() const override { drawer_(*this); }

    double GetWidth() const { return width_; }
    double GetLength() const { return length_; }
    double GetHeight() const { return height_; }

    Point GetCenter() const { return center_; }

private:
    double width_;
    double length_;
    double height_;
    Point center_;
    DrawStrategy drawer_;
};

// ---- GLDrawStrategy.h ----
// #include <Sphere.h>
// #include <Box.h>
namespace gl {

enum class Color {
    red,
    green,
    blue
};

std::string to_string(const Color& color) {
    switch(color) {
        case Color::red:
            return "red";
        case Color::green:
            return "green";
        case Color::blue:
            return "blue";
        default:
            return "unknown";
        }
}

class GLDrawStrategy {
public:
    explicit GLDrawStrategy(Color color)
    : color_(color){}

    void operator()(const Sphere& sphere) const {
        std::cout << "Sphere with radius = " << sphere.GetRadius()
                  << " at " << sphere.GetCenter()
                  << " and color = " << to_string(color_) << '\n';
    }

    void operator()(const Box& box) const {
        std::cout << "Box with width = " << box.GetWidth()
                  << " length = " << box.GetLength()
                  << " height = " << box.GetHeight()
                  << " at " << box.GetCenter()
                  << " and color = " << to_string(color_) << '\n';
    }

private:
    Color color_;
};

} // end namespace gl

// ---- main.cpp ----
// #include <Sphere.h>
// #include <Box.h>
// #include <GLDrawStrategy.h>
#include <vector>
#include <memory>
using ObjectPtr = std::unique_ptr<Object>;
using Objects = std::vector<ObjectPtr>;

void DrawAllObjects(const Objects& objects) {
    for (const auto& object : objects) {
        object->draw();
    }
}

int main() {
    Objects objects;
    objects.emplace_back(std::make_unique<Sphere>(1.0, Point{}, gl::GLDrawStrategy{gl::Color::red}));
    objects.emplace_back(std::make_unique<Box>(0.1, 0.2, 0.3, Point{}, gl::GLDrawStrategy{gl::Color::blue}));

    DrawAllObjects(objects);

}
