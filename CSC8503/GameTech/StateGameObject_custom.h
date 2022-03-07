#pragma once
#include "..\CSC8503Common\GameObject.h"
namespace NCL
{
    namespace CSC8503
    {
        class StateMachine;//Create an StateMachine class and GameObject class
        class StateGameObject_custom :public GameObject
        {
        public:
            StateGameObject_custom();
            ~StateGameObject_custom();

            //Virtual Update
            virtual void Update(float dt);
        
        protected:
            //Make states
            void MoveBack(float dt);
            void MoveFront(float dt);
            void pointNearBy(float dt);

            StateMachine* stateMachine_custom;

            //Counter?
            float counter;
        };
    }
}



