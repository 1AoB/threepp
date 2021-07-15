
#include "threepp/objects/Mesh.hpp"
#include "threepp/core/Raycaster.hpp"

using namespace threepp;

namespace {

    std::optional<Intersection> checkIntersection( Object3D *object, Material *material, Raycaster &raycaster, Ray &ray, const Vector3 &pA, const Vector3 &pB, const Vector3 &pC, Vector3 &point ) {

        Vector3 _intersectionPointWorld{};

        if ( material->side == BackSide ) {

            ray.intersectTriangle( pC, pB, pA, true, point );

        } else {

            ray.intersectTriangle( pA, pB, pC, material->side != DoubleSide, point );

        }

        if ( std::isnan(point.x) ) return std::nullopt;

        _intersectionPointWorld.copy( point );
        _intersectionPointWorld.applyMatrix4( object->matrixWorld );

        const auto distance = raycaster.ray.origin.distanceTo( _intersectionPointWorld );

        if ( distance < raycaster.near || distance > raycaster.far ) return std::nullopt;

        Intersection intersection{};
        intersection.distance = distance;
        intersection.point = _intersectionPointWorld;
        intersection.object = object;

        return intersection;
    }

    std::optional<Intersection> checkBufferGeometryIntersection( Object3D *object, Material *material, Raycaster &raycaster, Ray &ray, const FloatBufferAttribute &position, const FloatBufferAttribute *uv, const FloatBufferAttribute *uv2, int a, int b, int c ) {

        Vector3 _vA{};
        Vector3 _vB{};
        Vector3 _vC{};
        Vector3 _intersectionPoint{};

        position.setFromBufferAttribute(_vA, a);
        position.setFromBufferAttribute(_vB, b);
        position.setFromBufferAttribute(_vC, c);

        const auto intersection = checkIntersection( object, material, raycaster, ray, _vA, _vB, _vC, _intersectionPoint );

//        if ( intersection ) {
//
//            if ( uv ) {
//
//                _uvA.fromBufferAttribute( uv, a );
//                _uvB.fromBufferAttribute( uv, b );
//                _uvC.fromBufferAttribute( uv, c );
//
//                intersection.uv = Triangle.getUV( _intersectionPoint, _vA, _vB, _vC, _uvA, _uvB, _uvC, new Vector2() );
//
//            }
//
//            if ( uv2 ) {
//
//                _uvA.fromBufferAttribute( uv2, a );
//                _uvB.fromBufferAttribute( uv2, b );
//                _uvC.fromBufferAttribute( uv2, c );
//
//                intersection.uv2 = Triangle.getUV( _intersectionPoint, _vA, _vB, _vC, _uvA, _uvB, _uvC, new Vector2() );
//
//            }
//
//            const face = {
//                    a: a,
//                    b: b,
//                    c: c,
//                    normal: new Vector3(),
//                    materialIndex: 0
//            };
//
//            Triangle.getNormal( _vA, _vB, _vC, face.normal );
//
//            intersection.face = face;
//
//        }

        return intersection;

    }

}

void Mesh::raycast(Raycaster &raycaster, std::vector<Intersection> &intersects) {

    if ( material_ == nullptr ) return;

    Sphere _sphere{};

    // Checking boundingSphere distance to ray

    if ( !geometry_->boundingSphere ) geometry_->computeBoundingSphere();

    _sphere.copy( *geometry_->boundingSphere );
    _sphere.applyMatrix4( matrixWorld );

    if ( !raycaster.ray.intersectsSphere( _sphere ) ) return;

    //

    Ray _ray{};
    Matrix4 _inverseMatrix{};

    _inverseMatrix.copy( matrixWorld ).invert();
    _ray.copy( raycaster.ray ).applyMatrix4( _inverseMatrix );

    // Check boundingBox before continuing

    if ( geometry_->boundingBox ) {

        if ( !_ray.intersectsBox( *geometry_->boundingBox ) ) return;

    }

    std::optional<Intersection> intersection;


        const auto index = geometry_->getIndex();
        const auto position = geometry_->getAttribute<float>("position");
        const auto uv = geometry_->getAttribute<float>("uv");
        const auto uv2 = geometry_->getAttribute<float>("uv2");
        const auto groups = geometry_->groups;
        const auto drawRange = geometry_->drawRange;

        if ( index != nullptr ) {

            // indexed buffer geometry

            if ( false /*Array.isArray( material )*/ ) {

//                for ( int i = 0, il = groups.size(); i < il; i ++ ) {
//
//                    const auto &group = groups[ i ];
//                    const auto &groupMaterial = material[ group.materialIndex ];
//
//                    const auto start = std::max( group.start, drawRange.start );
//                    const auto end = std::min( ( group.start + group.count ), ( drawRange.start + drawRange.count ) );
//
//                    for ( int j = start, jl = end; j < jl; j += 3 ) {
//
//                        const a = index.getX( j );
//                        const b = index.getX( j + 1 );
//                        const c = index.getX( j + 2 );
//
//                        intersection = checkBufferGeometryIntersection( this, groupMaterial, raycaster, _ray, position, morphPosition, morphTargetsRelative, uv, uv2, a, b, c );
//
//                        if ( intersection ) {
//
//                            intersection.faceIndex = std::floor( j / 3 ); // triangle number in indexed buffer semantics
//                            intersection.face.materialIndex = group.materialIndex;
//                            intersects.push( intersection );
//
//                        }
//
//                    }
//
//                }

            } else {

                const int start = std::max( 0, drawRange.start );
                const int end = std::min( index->count(), ( drawRange.start + drawRange.count ) );

                for ( int i = start, il = end; i < il; i += 3 ) {

                    const auto a = index->getX( i );
                    const auto b = index->getX( i + 1 );
                    const auto c = index->getX( i + 2 );

                    intersection = checkBufferGeometryIntersection( this, material(), raycaster, _ray, *position, uv, uv2, a, b, c );

                    if ( intersection ) {

                        intersection->faceIndex = (int) std::floor( i / 3 ); // triangle number in indexed buffer semantics
                        intersects.emplace_back( *intersection );

                    }

                }

            }

        } else if ( position != nullptr ) {

            // non-indexed buffer geometry

            if ( false /*Array.isArray( material )*/ ) {

//                for ( int i = 0, il = groups.length; i < il; i ++ ) {
//
//                    const group = groups[ i ];
//                    const groupMaterial = material[ group.materialIndex ];
//
//                    const start = std::max( group.start, drawRange.start );
//                    const end = std::min( ( group.start + group.count ), ( drawRange.start + drawRange.count ) );
//
//                    for ( let j = start, jl = end; j < jl; j += 3 ) {
//
//                        const a = j;
//                        const b = j + 1;
//                        const c = j + 2;
//
//                        intersection = checkBufferGeometryIntersection( this, groupMaterial, raycaster, _ray, position, morphPosition, morphTargetsRelative, uv, uv2, a, b, c );
//
//                        if ( intersection ) {
//
//                            intersection.faceIndex = std::floor( j / 3 ); // triangle number in non-indexed buffer semantics
//                            intersection.face.materialIndex = group.materialIndex;
//                            intersects.push( intersection );
//
//                        }
//
//                    }
//
//                }

            } else {

                const int start = std::max( 0, drawRange.start );
                const int end = std::min( position->count(), ( drawRange.start + drawRange.count ) );

                for ( int i = start, il = end; i < il; i += 3 ) {

                    const int a = i;
                    const int b = i + 1;
                    const int c = i + 2;

                    intersection = checkBufferGeometryIntersection( this, material(), raycaster, _ray, *position, uv, uv2, a, b, c );

                    if ( intersection ) {

                        intersection->faceIndex = (int) std::floor( i / 3 ); // triangle number in non-indexed buffer semantics
                        intersects.emplace_back( *intersection );

                    }

                }

            }

        }
    
}