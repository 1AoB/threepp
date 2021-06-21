// https://github.com/mrdoob/three.js/blob/r129/src/core/Uniform.js

#ifndef THREEPP_UNIFORM_HPP
#define THREEPP_UNIFORM_HPP

#include <any>
#include <optional>
#include <utility>

namespace threepp {

    class Uniform {

    public:
        std::optional<bool> needsUpdate;

        explicit Uniform() = default;

        explicit Uniform(std::any value) : value_(std::move(value)) {}

        [[nodiscard]] bool hasValue() const {
            return value_.has_value();
        }

        std::any value() {
            return value_;
        }

        template<class T>
        [[nodiscard]] T value() {
            return std::any_cast<T>(value_);
        }

        void setValue(std::any value) {
            this->value_ = std::move(value);
        }

    private:
        std::any value_;
    };

}// namespace threepp

#endif//THREEPP_UNIFORM_HPP
