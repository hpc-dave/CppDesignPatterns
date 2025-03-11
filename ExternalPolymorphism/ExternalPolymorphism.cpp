#include <iostream>

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

void free_get_info(const Sphere& s){
    std::cout << "I am a Sphere at point " << s.GetCenter() << " with radius " << s.GetRadius() << std::endl;
}

// ---- Box.h ----
// #include <Point.h>
class Box {
public:
    explicit Box(double w, double l, double h, Point c = Point{})
        : width(w), length(l), height(h), center(c) {
    }
    double GetWidth() const { return width; }
    double GetLength() const { return length; }
    double GetHeight() const { return height; }

    Point GetCenter() const { return center; }

    void GetInfo() const { std::cout << "I am a Box at point " << center << " with width = "
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

// ---- Object.h

#include <utility>

class ObjectConcept {
public:
    virtual ~ObjectConcept() = default;
    virtual void GetInfo() const = 0;
};

template<typename TObject>
class ObjectModel : public ObjectConcept {
public:
    ObjectModel(const TObject& o)
        : object{o} {
        }
    
    void GetInfo() const override {
        // free_get_info(object);
        object.GetInfo();
    }

private:
    TObject object;
};


// ---- main.cpp-----
#include <vector>
#include <memory>

using Objects = std::vector<std::unique_ptr<ObjectConcept>>;

void GetAllInfo(const Objects& objects) {
    for (const auto& object : objects) {
        object->GetInfo();
    }
}

template<typename TObject>
auto make_object(const TObject& object){
    return std::make_unique<ObjectModel<TObject>>(object);
}

int main() {
    Objects objects;
    objects.emplace_back(make_object(Sphere{1.0}));
    objects.emplace_back(make_object(Box{0.1, 0.2, 0.3, Point{}}));

    GetAllInfo(objects);
}
