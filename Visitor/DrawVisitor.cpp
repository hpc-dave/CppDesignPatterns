
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
OS& operator<<(OS& os, const Point& point){
    os <<'[' << point.x_ << ' ' << point.y_ << ' ' << point.z_ << ']';
    return os;
}

// ---- Sphere.h ----
// #include <Point.h>

class Sphere {
public:
    explicit Sphere(double radius, Point center = Point{})
    : radius_(radius), center_(center){}

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
    : width_(width), length_(length), height_(height), center_(center){
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

// ---- SphereDraw.h ----
// #include <Sphere.h>
#include <iostream>

void free_draw(const Sphere& sphere){
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

// ---- SphereVolume.h ----
// #include <Sphere.h>
#include <numbers>

double free_volume(const Sphere& sphere) {
    return 4. / 3. * std::numbers::pi * sphere.GetRadius() * sphere.GetRadius() * sphere.GetRadius();
}

// ---- BoxVolume.h ----
// #include <Box.h>
#include <iostream>

double free_volume(const Box& box) {
    return box.GetWidth() * box.GetLength() * box.GetHeight();
}

// ---- Draw.h ----

class Draw {
public:
    template<typename TObject>
    void operator()(const TObject& object) {
        free_draw(object);
    }
};

// ---- main.cpp ----
// #include <Draw.h>
// #include <Sphere.h>
// #include <Box.h>
// #include <SphereDraw.h>  <-- Note that those could also be part of Draw.h
// #include <BoxDraw.h>     <-- Note that those could also be part of Draw.h
#include <variant>
#include <vector>
using Object = std::variant<Sphere, Box>;
using Objects = std::vector<Object>;

// Inside this function, the Draw FunctionObject is used
// as a Visitor
void DrawAllObjects(const Objects& objects) {
    for(const auto& object : objects) {
        std::visit(Draw{}, object);
    }
}

// Here an example with a lambda is provided
// and a return value used
void ComputeVolumeAllObjects(const Objects& objects) {
    auto volume = [](auto obj) -> double {
        return free_volume(obj);
    };
    for (const auto& object : objects) {
        std::cout << "The computed volume is : " << std::visit(volume, object) << '\n';
    }
}
int main() {
    Objects objects;
    objects.emplace_back(Sphere{1.0});
    objects.emplace_back(Box{0.1, 0.2, 0.3});
    objects.emplace_back(Sphere{2.0, Point(0.5, 0.6, 0.7)});
    objects.emplace_back(Box{0.1, 0.2, 0.3, Point(-0.5, -0.6, -0.7)});

    DrawAllObjects(objects);

    ComputeVolumeAllObjects(objects);
}
