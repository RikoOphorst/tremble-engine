#pragma once
#include "../math/math.h"

namespace tremble
{
	class PhysicsGeometry
	{
	public:
	    virtual ~PhysicsGeometry() = default;

	    enum GeometryType
		{
			Box = 100,
			Sphere,
			Capsule,
            TriangleMesh,
            ConvexMesh,
			Invalid = 69
		};

        enum ScalingType
        {
            GLOBAL_SCALE = 200,
            RELATIVE_TO_NODE,
            RELATIVE_TO_PARENT
        };

		PhysicsGeometry(GeometryType);
		virtual physx::PxGeometry* GetPxGeometry() const = 0;
        virtual PhysicsGeometry* CreateCopyForScaled() const = 0;
		GeometryType GetType() const { return geometry_type_; }
        virtual void Scale(const Vector3& scale) = 0;
	protected:
		GeometryType geometry_type_;
	};
}