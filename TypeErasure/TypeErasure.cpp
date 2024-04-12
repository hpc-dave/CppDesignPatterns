
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

// ---- Sphere.h ----
// #include <Point.h>

class Sphere {
public:
    explicit Sphere(double radius, Point center = Point{})
        : radius_(radius), center_(center) {}

    double GetRadius() const { return radius_; }

    Point GetCenter() const { return center_; }

private:
    double radius_;
    Point center_;
};

// ---- Box.h ----
// #include <Point.h>
class Box {
public:
    explicit Box(double width, double length, double height, Point center = Point{})
        : width_(width), length_(length), height_(height), center_(center) {
    }
    double GetWidth() const { return width_; }
    double GetLength() const { return length_; }
    double GetHeight() const { return height_; }

    Point GetCenter() const { return center_; }

private:
    double width_;
    double length_;
    double height_;
    Point center_;
};

// ---- Object.h

#include <utility>
#include <memory>

class Object {
public:
    template<typename TObject>
    Object(const TObject& object)
        : pimpl_(std::make_unique<Model<TObject>>(object)) {
    }

    template <typename TObject, typename DrawStrategy>
    Object(const TObject& object, DrawStrategy drawer)
        : pimpl_(std::make_unique<ExtendedModel<TObject, DrawStrategy>>(object, std::move(drawer))) {
    }

    Object(const Object& other) 
        : pimpl_(other.pimpl_->clone()){
    }

    Object& operator=(const Object& other) {
        // Copy and swap idiom
        Object tmp(other);
        std::swap(pimpl_, tmp.pimpl_);
        return *this;
    }

    ~Object() = default;
    Object(Object&&) = default;
    Object& operator=(Object&&) = default;

private:
    friend void free_draw(const Object& object) {
        object.pimpl_->do_draw();
    }

    class Concept { // External Polymorphism
    public:
        virtual ~Concept() = default;
        virtual void do_draw() const = 0;
        virtual std::unique_ptr<Concept> clone() const = 0; // Prototype
    };

    template <typename TObject>
    class Model : public Concept {
    public:
        Model(const TObject& object)
            : object_{object} {
        }

        void do_draw() const final {
            free_draw(object_);
        }

        std::unique_ptr<Concept> clone() const final {
            return std::make_unique<Model>(*this);
        }

    private:
        TObject object_;
    };

    template <typename TObject, typename DrawStrategy>
    class ExtendedModel : public Concept {
    public:
        ExtendedModel(const TObject& object, DrawStrategy drawer)
            : object_{object}, drawer_{std::move(drawer)} {
        }

        void do_draw() const final {
            drawer_(object_);
        }

        std::unique_ptr<Concept> clone() const final {
            return std::make_unique<ExtendedModel>(*this);
        }

    private:
        TObject object_;
        DrawStrategy drawer_;
    };

    std::unique_ptr<Concept> pimpl_; // pimpl idiom / Bridge
};

// ---- GLDrawStrategy.h ----
// #include <Sphere.h>
// #include <Box.h>
#include <iostream>

namespace gl {
enum class Color {
    red,
    green,
    blue
};

std::string to_string(const Color& color) {
    switch (color) {
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
        : color_(color) {}

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
}  // end namespace gl

// ---- SphereDraw.h ----
// #include <Sphere.h>
#include <iostream>

void free_draw(const Sphere& sphere) {
    std::cout << "Sphere with radius = " << sphere.GetRadius() << " at " << sphere.GetCenter() << '\n';
}

// ---- BoxDraw.h ----
// #include <Box.h>
#include <iostream>

void free_draw(const Box& box) {
    std::cout << "Box with width = " << box.GetWidth()
              << " length = " << box.GetLength()
              << " height = " << box.GetHeight()
              << " at " << box.GetCenter() << '\n';
}

// ---- main.cpp-----
#include <memory>
#include <vector>

// to test the implementation, we add a new object here
class Cylinder {
public:
    Cylinder(double radius, Point center = Point{})
        : radius_(radius), center_(center) {
    }

    double GetRadius() const { return radius_; }
    Point GetCenter() const { return center_; }

private:
    double radius_;
    Point center_;
};

void free_draw(const Cylinder& cylinder){
    std::cout << "Cylinder with radius = " << cylinder.GetRadius() << " at " << cylinder.GetCenter() << '\n';
}

using Objects = std::vector<Object>;

void DrawAllObjects(const Objects& objects) {
    for (const auto& object : objects) {
        free_draw(object);
    }
}

int main() {
    Objects objects;
    objects.emplace_back(Sphere{1.0});
    objects.emplace_back(Box{0.1, 0.2, 0.3, Point{}}, gl::GLDrawStrategy{gl::Color::blue});
    objects.emplace_back(Cylinder{0.15, Point{0.2, 0.3}});
    objects.emplace_back(Cylinder{0.15, Point{0.2, 0.3}}, [](const Cylinder& c) {
        std::cout << "This is a custom strategy for the cylinder (radius = " << c.GetRadius()
                  << ", center = " << c.GetCenter() << "\n";
    });

    DrawAllObjects(objects);
}
