#include "PositionConstraint.h"
#include "../../Common/Vector3.h"
#include "../CSC8503Common/GameObject.h"

using namespace NCL;
using namespace CSC8503;

//Problem
void PositionConstraint::UpdateConstraint(float dt)
{
	Vector3 relativePos = objectA->GetTransform().GetPosition()-objectB->GetTransform().GetPosition();
	
	float currentDistance = relativePos.Length();
	float offset = distance - currentDistance;
	if (abs(offset) > 0.0f) { 

		Vector3 force = Vector3(1,1,2);
		Vector3 offsetDir = relativePos.Normalised();

		PhysicsObject * physA = objectA -> GetPhysicsObject();
		PhysicsObject * physB = objectB -> GetPhysicsObject();
		
		Vector3 relativeLinearVelocity = physA -> GetLinearVelocity() -physB -> GetLinearVelocity();
		//Vector3 relativeAngularVelocity = physA->GetAngularVelocity() - physB->GetAngularVelocity();//Angular

		float constraintMass = physA -> GetInverseMass() + physB -> GetInverseMass();
		
			if (constraintMass > 0.0f) {
			 // how much of their relative force is affecting the constraint
			float velocityLinearDot = Vector3::Dot(relativeLinearVelocity, offsetDir);//Linear 
			//float velocityAngularDot = Vector3::Dot(relativeAngularVelocity, offsetDir);//Angular
			
			float biasFactor = 0.01f;
			float bias = -(biasFactor / dt) * offset;
			
			float lambda_Linear = -(velocityLinearDot + bias) / constraintMass;
			//float lambda_Angular = -(velocityAngularDot + bias) / constraintMass;//Angular
			
			 Vector3 aImpulse = offsetDir * lambda_Linear;
			 Vector3 bImpulse = -offsetDir * lambda_Linear;

			 //Vector3 aAngular = offsetDir * lambda_Angular;//Angular
			 //Vector3 bAngular = -offsetDir * lambda_Angular;//Angular
			
			 physA -> ApplyLinearImpulse(aImpulse); // multiplied by mass here
			 physB -> ApplyLinearImpulse(bImpulse); // multiplied by mass here
			

			 /*physA->ApplyAngularImpulse(Vector3::Cross(objectA->GetTransform().GetPosition(), aAngular));
			 physB->ApplyAngularImpulse(Vector3::Cross(objectB->GetTransform().GetPosition(), bAngular));*/

			/* physA->AddForce(force);
			 physB->AddForce(force);*/
		}	
	}
}