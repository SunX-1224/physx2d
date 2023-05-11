#pragma once

#include "../utils/components.h"
#include <iostream>

namespace Physx2D{
	void updatePhysics(RigidBody2D* body, float delta_time);
	CollisionData edgeCircleCollision(CircleCollider* targetObject, CircleCollider* boundingObject);
	void collisionPhysics(CollisionData data, RigidBody2D* dynamicBody, RigidBody2D* body2);
}
