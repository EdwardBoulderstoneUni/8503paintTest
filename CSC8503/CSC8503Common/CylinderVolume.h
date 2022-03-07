#pragma once
#include "CollisionVolume.h"
#include "../../Common/Vector3.h"

namespace NCL
{
	namespace CSC8503
	{
		class CylinderVolume : CollisionVolume
		{
		public:
			CylinderVolume(float CylinderRadius = 1.0f)
			{
				type = VolumeType::Capsule;
				radius = CylinderRadius;
			}

			~CylinderVolume(){}

			float GetRadius()
			{
				return radius;
			}


			float GetHalfHeight()
			{
				return halfHeight;
			}

		protected:
			float radius;
			float halfHeight;
		};
	}
}


