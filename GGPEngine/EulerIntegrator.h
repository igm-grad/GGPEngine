#pragma once
#include "Integrator.h"
#include "GameTimer.h"

/* =========================================================================================================
				This class implements Euler Integration. 
	
					Y(t+1) = Y(t) + ( H * F(Y,t))
	
	In this class, the Position of the game objects are being integrated. The values represent the Following:
		
		For Linear integration:
		Y(t)  = Position of the object at the given time (t)
		H	  = Time step (may be synchronized with the core Engine or not, Upto the programmer)
		F(Y,t)= Velocity of the object at the given time (t)

		For Roational Integration:
		Y(t)  = quaternion Representing orientation at given time (t).
		H	  = Time step (may be synchronized with the core Engine or not, Upto the programmer)
		F(Y,t)= Derivative of the quaternion representing the orientation at the given time (t)

		============================================================================================================
*/


class EulerIntegrator: public Integrator
{
public:
	EulerIntegrator();
	EulerIntegrator(GameTimer time);					// The timer is set the same as the CoreEngine's Timer. 
	~EulerIntegrator();

	void setTimer(GameTimer time);						// If the user wants the integrator to run on a different timer, exclusive of the Core engine's (Async to the CoreEngine)

protected:
	void IntegrateGameObject(RigidBody& gameObject);	// Implements Basic Euler Integration (Take a simple Euler Step forward in time by time-step defined in the timer)

private:
	GameTimer timer;									// EulerIntegrator's own Timer.
};

