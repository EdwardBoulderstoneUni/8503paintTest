#pragma once
#include < functional >
namespace NCL {
	namespace CSC8503 {
		typedef std::function < void(float) > StateUpdateFunction;
		class State		{
		public:
			State() {}
			State(StateUpdateFunction someFunc) {
				func = someFunc;
			}
			void Update(float dt) {
				if (func != nullptr) {
					func(dt);
					
				}
			}


			virtual ~State() {}
			//virtual void Update() = 0; //Pure virtual base class

		protected:
			StateUpdateFunction func;
		};

		typedef void(*StateFunc)(void*);

		
	}
}