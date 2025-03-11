#include <functional>
#include <iostream>
#include <memory>
#include <set>
#include <utility>

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

class ConstantTimeStep {
public:
    enum class StateChange {
        Update
    };
    using ValueType = double;
    using ObserverType = utils::Observer<ConstantTimeStep, StateChange>;

    explicit ConstantTimeStep(ValueType t) : dt(t) {
    }

    virtual ~ConstantTimeStep() {}

    ValueType GetTimeStepSize() const {
        return dt;
    }

    bool Attach(ObserverType* o) {
        // empty, because the time step is constant
        return true;
    }

    bool Detach(ObserverType* o) {
        // empty, because the time step is constant
        return true;
    }

    void Notify(StateChange s) {
        // empty, nothing to notify
    }

    operator ValueType() const { return dt; }

private:
    ValueType dt;
};

class AdaptiveTimeStep {
public:
    enum class StateChange {
        Update
    };
    using ValueType = double;
    using ObserverType = utils::Observer<AdaptiveTimeStep, StateChange>;

    explicit AdaptiveTimeStep(ValueType _dt) : dt(dt) {}

    ValueType GetTimeStepSize() const {
        return dt;
    }

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

    void AdaptTimeStepSize(ValueType _dt) {
        dt = _dt;
        Notify(StateChange::Update);
    }

    operator ValueType() { return dt; }

private:
    ValueType dt;
    std::set<ObserverType*> observers;
};

template <typename T>
concept TimeStepper =
    utils::Observable<T> &&
    std::convertible_to<T, typename T::ValueType> &&
    requires {
        typename T::ValueType;
    } && requires(const T t) {
        { t.GetTimeStepSize() } -> std::same_as<typename T::ValueType>;
    };

template <typename T>
concept AdaptiveTimeStepper = TimeStepper<T> &&
                              requires(T t, T::ValueType dt) {
                                  t.AdaptTimeStepSize(dt);
                              };
static_assert(TimeStepper<ConstantTimeStep>, "does not fullfill requirements");
static_assert(AdaptiveTimeStepper<AdaptiveTimeStep>, "does not fullfill requirements");

int main() {
    ConstantTimeStep dt_const{0.1};
    AdaptiveTimeStep dt_adapt{2.1};

    ConstantTimeStep::ObserverType obs_const([](const ConstantTimeStep&, ConstantTimeStep::StateChange) {});
    AdaptiveTimeStep::ObserverType obs_adapt([](const AdaptiveTimeStep& t, AdaptiveTimeStep::StateChange s) {
        std::cout << "The time step was adapted to " << t.GetTimeStepSize() << std::endl;
    });

    dt_adapt.Attach(&obs_adapt);

    dt_adapt.AdaptTimeStepSize(2.5);

}

// compile with g++ --std=c++20 -o timestep ObservedTimeStep.cpp
