#include <functional>
#include <memory>
#include <utility>
#include <set>
#include <iostream>

namespace utils {

/*!
 * \brief a generic observer for implementation of the Observer pattern
 * @tparam Subject type of object which is observed
 * @tparam StateTag a tag which allows differentiating between different callbacks
 */
template <typename Subject, typename StateTag>
class Observer {
public:
    using OnUpdate = std::function<void(Subject const&, StateTag)>;

    /*!
     * @brief constructor
     * @param onUpdate function pointer to update function
     *
     * The signature of the update function has to be void(const Subject&, StateTag)
     */
    explicit Observer(OnUpdate onUpdate)
        : onUpdate_{std::move(onUpdate)} {
        // Possibly respond on an invalid/empty std::function instance
    }

    /*!
     * @brief update function called by the subject to notify a change
     * @param subject reference to the subject
     * @param property the state that was changed
     */
    void Update(const Subject& subject, StateTag property) {
        onUpdate_(subject, property);
    }

private:
    OnUpdate onUpdate_;  //!< function pointer with the update function
};

template <typename T>
concept Observable =
    requires {
        typename T::StateChange;
    } && requires(T t, Observer<T, typename T::StateChange>* o, typename T::StateChange s) {
        { t.Attach(o) } -> std::same_as<bool>;
        { t.Detach(o) } -> std::same_as<bool>;
        t.Notify(s);
    };  // NOLINT

namespace detail {
/*!
 * @brief a concept class for external polymorphism to handle observers
 *
 * The class is empty, because we are only interested in the
 * automatically generated virtual destructor. Otherwise the
 * observer is not called by the owning object, only
 * by the observed object, and that one knows the type!
 */
class ObserverHandleConcept {
};

}  // end namespace detail

using UniqueObserverHandle = std::unique_ptr<detail::ObserverHandleConcept>;

/*!
 * @brief an observer handle for managing the lifetime of observers
 * @tparam T an observer type
 * No functions except the constructor are required, since the owning
 * instance of an observer does not require any further direct access.
 */
template <typename Subject, typename StateTag>
class ObserverHandleModel : public detail::ObserverHandleConcept {
public:
    using ObserverType = Observer<Subject, StateTag>;
    explicit ObserverHandleModel(ObserverType observer) : obs(std::move(observer)) {}

private:
    ObserverType obs;  //!< actual instance of the observer
};

/*!
 * @brief A convenience function to get an observer handle
 * @tparam Lambda Update function type
 * @tparam T the observable type
 * @param on_update actual update function
 */
template <Observable T, typename Lambda>
UniqueObserverHandle make_observer_handle(Lambda on_update) {
    using ObserverType = T::ObserverType;
    using TObsModel = utils::ObserverHandleModel<T, typename T::StateChange>;

    ObserverType obs(on_update);
    return std::make_unique<TObsModel>(std::move(obs));
}

}  // namespace utils


class Foo{
public:
    enum class StateChange {
        DoA,
        DoB
    };

    using ObserverType = utils::Observer<Foo, StateChange>;

    bool Attach(ObserverType* o) {
        auto [pos, success] = observers.emplace(o);
        return success;
    }

    bool Detach(ObserverType* o) {
        return (observers.erase(o) > 0U);
    }

    void Notify(StateChange property) {
        for (auto iter = observers.begin(); iter != observers.end();) {
            auto const pos = iter++;
            (*pos)->Update(*this, property);
        }
    }

    void DoingA() {
        std::cout << "Foo::DoingA is called\n";
        Notify(StateChange::DoA);
    }
    void DoingB() {
        std::cout << "Foo::DoingB is called\n";
        Notify(StateChange::DoB);
    }

private:
    std::set<ObserverType*> observers;
};

int main() {
    static_assert(utils::Observable<Foo>, "Foo is not observable");

    Foo::ObserverType obs([](const Foo& foo, Foo::StateChange s) {
        switch(s) {
        case Foo::StateChange::DoA:
            std::cout << "Observer reports: Foo is doing A" << std::endl;
            break;
        case Foo::StateChange::DoB:
            std::cout << "Observer reports: Foo is doing B" << std::endl;
            break;
        }
    });

    Foo foo;
    foo.Attach(&obs);
    foo.DoingA();
    foo.DoingB();
}
// compile with g++ --std=c++20 -o observer GenericObserver.cpp
