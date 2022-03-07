#pragma once
#include "../CSC8503Common/GameWorld.h"
#include <set>

namespace NCL {
	namespace CSC8503 {
		class PhysicsSystem	{
		public:
			PhysicsSystem(GameWorld& g);
			~PhysicsSystem();

			void Clear();

			void Update(float dt);

			void UseGravity(bool state) {
				applyGravity = state;
			}

			void SetGlobalDamping(float d) {
				globalDamping = d;
			}

			void SetGravity(const Vector3& g);
		protected:
			void BasicCollisionDetection();
			void BroadPhase();
			void NarrowPhase();

			void ClearForces();

			void IntegrateAccel(float dt);
			void IntegrateVelocity(float dt);

			void UpdateConstraints(float dt);

			void UpdateCollisionList();
			void UpdateObjectAABBs();

			void ImpulseResolveCollision(GameObject& a , GameObject&b, CollisionDetection::ContactPoint& p) ;
			void ResolveSpringCollision(GameObject& a, GameObject& b, CollisionDetection::ContactPoint& p) ;

			void SlowenObjectResolveCollision(GameObject& object) ;
			void FastenObjectResolveCollision(GameObject& object) ;
			void BoosterCubeResoloveCollision(GameObject& a, GameObject& b, CollisionDetection::ContactPoint& p);

			//B
			void ResolutionConstraint(GameObject&a,GameObject &b,CollisionDetection::ContactPoint& p);

			GameWorld& gameWorld;

			bool	applyGravity;
			Vector3 gravity;
			float	dTOffset;
			float	globalDamping;

			std::set<CollisionDetection::CollisionInfo> allCollisions;
			std::set < CollisionDetection::CollisionInfo > broadphaseCollisions;


			bool useBroadPhase		= true;
			int numCollisionFrames	= 5;
		};
	}
}

