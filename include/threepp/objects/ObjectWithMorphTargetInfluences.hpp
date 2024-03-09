
#ifndef THREEPP_OBJECTWITHMORPHTARGETINFLUENCES_HPP
#define THREEPP_OBJECTWITHMORPHTARGETINFLUENCES_HPP

#include "threepp/core/Object3D.hpp"

#include <vector>

namespace threepp {

    class ObjectWithMorphTargetInfluences: public virtual Object3D {

    public:
        std::vector<float>& morphTargetInfluences() {

            if (copyMorphTargetInfluences_) {
                return *copyMorphTargetInfluences_;
            }

            return morphTargetInfluences_;
        }

        void copyMorphTargetInfluences(std::vector<float>* influences) {

            copyMorphTargetInfluences_ = influences;
        }

        void reset() {

            copyMorphTargetInfluences_ = nullptr;
        }

    private:
        std::vector<float> morphTargetInfluences_;
        std::vector<float>* copyMorphTargetInfluences_ = nullptr;
    };

}// namespace threepp

#endif//THREEPP_OBJECTWITHMORPHTARGETINFLUENCES_HPP
