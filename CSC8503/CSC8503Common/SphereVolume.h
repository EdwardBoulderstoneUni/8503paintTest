#pragma once
#include "CollisionVolume.h"

namespace NCL {
	class SphereVolume : CollisionVolume
	{
	public:
		
		SphereVolume(float sphereRadius = 1.0f) {
			type	= VolumeType::Sphere;
			radius	= sphereRadius;
			//isTrigger = false;
		}
		~SphereVolume() {}

		/*void SetTriggerState(bool triggerState) 
		{
			isTrigger = triggerState;
		}

		bool GetTriggerState() const
		{
			return isTrigger;
		}*/

		float GetRadius() const {
			return radius;
		}
		
	protected:
		float	radius;
		//bool isTrigger;
	}; 
}

