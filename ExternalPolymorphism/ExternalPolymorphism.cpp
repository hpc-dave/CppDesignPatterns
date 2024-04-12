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

class ObjectConcept {
public:
    virtual ~ObjectConcept() = default;
    virtual void draw() const = 0;
};

template<typename TObject, typename DrawStrategy>
class ObjectModel : public ObjectConcept {
public:
    ObjectModel(const TObject& object, DrawStrategy drawer)
        : object_{object}, drawer_{std::move(drawer)} {
        }
    
    void draw() const override {
        drawer_(object_);
    }
private:
    TObject object_;
    DrawStrategy drawer_;
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

// ---- main.cpp-----
#include <vector>
#include <memory>

using Objects = std::vector<std::unique_ptr<ObjectConcept>>;

void DrawAllObjects(const Objects& objects) {
    for (const auto& object : objects) {
        object->draw();
    }
}

template<typename TObject, typename DrawStrategy>
auto make_object(const TObject& object, DrawStrategy drawer){
    return std::make_unique<ObjectModel<TObject, DrawStrategy>>(object, drawer);
}

int main() {
    Objects objects;
    objects.emplace_back(make_object(Sphere{1.0}, gl::GLDrawStrategy{gl::Color::red}));
    objects.emplace_back(make_object(Box{0.1, 0.2, 0.3, Point{}}, gl::GLDrawStrategy{gl::Color::blue}));

    DrawAllObjects(objects);
}
