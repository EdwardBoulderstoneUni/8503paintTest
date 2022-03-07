#include "StateGameObject_custom.h"
#include "../CSC8503Common/StateTransition.h"
#include "../CSC8503Common/StateMachine.h"
#include "../CSC8503Common/State.h"

using namespace NCL;
using namespace CSC8503;

NCL::CSC8503::StateGameObject_custom::StateGameObject_custom()
{
	float distance;		//distance between object and destination
	counter = 0.0f;//Counter?
	stateMachine_custom = new StateMachine();

	//Make States using lambda
	State* state_forward = new State([&](float dt)->void
	{
			this->MoveFront(dt);
	}
	);

	State* state_back = new State([&](float dt)->void
	{
			this->MoveBack(dt);
	}
	);

	State* state_close = new State([&](float dt)->void
		{
			this->pointNearBy(dt);
		}
	);

	//Add state to StateMachine
	stateMachine_custom->AddState(state_forward);
	stateMachine_custom->AddState(state_back);

	//Add state transition method
	stateMachine_custom->AddTransition(new StateTransition(state_forward, state_back, [&]()->bool
		{
			return (this->GetTransform().GetPosition() - Vector3(20, 10, 20)).Length() > 6 ? true : false;
		}
	));

	stateMachine_custom->AddTransition(new StateTransition(state_back, state_forward, [&]()->bool
		{
			return (this->GetTransform().GetPosition() - Vector3(20, 10, 20)).Length() < 3 ? true : false;
		}
	));

	stateMachine_custom->AddTransition(new StateTransition(state_back, state_close, [&]()->bool
		{
			return (this->GetTransform().GetPosition() - Vector3(20, 5, 20)).Length() < 6 ? true : false;
				
			
		}

	));
}

NCL::CSC8503::StateGameObject_custom::~StateGameObject_custom()
{
	delete stateMachine_custom;
}

void NCL::CSC8503::StateGameObject_custom::Update(float dt)
{
	stateMachine_custom->Update(dt);
}

void NCL::CSC8503::StateGameObject_custom::pointNearBy(float dt)
{
	GetPhysicsObject()->AddForce({0,-20,0});
	GetRenderObject()->SetColour({ 1,1,0,1 });
	counter += dt;
}

void NCL::CSC8503::StateGameObject_custom::MoveFront(float dt)
{
	GetPhysicsObject()->AddForce({20,0,0}
		
	);
	GetRenderObject()->SetColour({1,0,0,1});
	counter += dt;
}

void NCL::CSC8503::StateGameObject_custom::MoveBack(float dt)
{
	GetPhysicsObject()->AddForce({ -20,0,0 });
	GetRenderObject()->SetColour({ 0,1,0,1 });
	counter += dt;
}

