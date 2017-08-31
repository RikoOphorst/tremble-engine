#pragma once
#include "physics_geometry.h"

namespace tremble
{
    class physx::PxGeometry;
	class PhysicsBoxGeometry;
	class PhysicsCapsuleGeometry;
	class PhysicsSphereGeometry;

    /**
    * @class tremble::PhysicsGeometryHolder
    * @brief Class, meant to transfer geometry from PxGeometry to PhysicsGeometry. Owns a Physics Geometry class
    */
	class PhysicsGeometryHolder
	{
	public:
        PhysicsGeometryHolder();
        PhysicsGeometryHolder(const PhysicsGeometryHolder& other);
        PhysicsGeometryHolder(const physx::PxGeometry& px_geometry);
        PhysicsGeometryHolder(const physx::PxGeometryHolder& px_geometry);
        void SetGeometry(const physx::PxGeometry& px_geometry); //!< News a new physicsGeometry in the physics memory allocator
        ~PhysicsGeometryHolder();

        void Clear();
        PhysicsGeometry* GetGeometry();
		PhysicsGeometry::GeometryType GetGeometryType() const;
		PhysicsBoxGeometry* GetBoxGeometry();
		PhysicsSphereGeometry* GetSphereGeometry();
		PhysicsCapsuleGeometry* GetCapsuleGeometry();
	private:
        PhysicsGeometry* m_physics_geometry_;
	};
}