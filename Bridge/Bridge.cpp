#include <memory>
#include <array>
#include <iostream>

using Vec = std::array<double, 3>;

class Mover{
public:
    virtual ~Mover() = default;
    virtual Vec GetNewCoords(Vec old) = 0;
    virtual void WhoAmI() = 0;
};

class Object {
protected:
    explicit Object(std::unique_ptr<Mover> mover)
        : move_pimpl(std::move(mover)){}
public:
    virtual ~Object() = default;
    virtual void move() = 0;
    virtual void WhereAmI() = 0;

protected:
    Mover* getMover() { return move_pimpl.get(); }
    Mover const* getMover() const { return move_pimpl.get(); }

private:
    std::unique_ptr<Mover> move_pimpl;
};

class SimpleMover : public Mover {
public:
    Vec GetNewCoords(Vec old) override {
        old[0] += 1.;
        return old;
    }
    void WhoAmI() override {
        std::cout << "I am a simple mover\n";
    }
};

class MovingObject : public Object {
public:
    MovingObject()
        : Object{std::make_unique<SimpleMover>()}{
        coords[0] = coords[1] = coords[2] = 0.;
    }
    void move() override {
        getMover()->WhoAmI();
        coords = getMover()->GetNewCoords(coords);
    }

    void WhereAmI() override {
        std::cout << "I am here: " << coords[0] << " " << coords[1] << " " << coords[2] << std::endl;
    }

private:
    Vec coords;
};

int main() {
    MovingObject obj;
    obj.WhereAmI();
    obj.move();
    obj.WhereAmI();
}
