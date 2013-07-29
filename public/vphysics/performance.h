//========= Copyright � 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef PERFORMANCE_H
#define PERFORMANCE_H
#ifdef _WIN32
#pragma once
#endif

const float DEFAULT_MIN_FRICTION_MASS = 10.0f / 1000;
const float DEFAULT_MAX_FRICTION_MASS = 2500.0f;
struct physics_performanceparams_t
{
	int		maxCollisionsPerObjectPerTimestep;		// object will be frozen after this many collisions (visual hitching vs. CPU cost)
	int		maxCollisionChecksPerTimestep;			// objects may penetrate after this many collision checks (can be extended in AdditionalCollisionChecksThisTick)
	float	maxVelocity;							// limit world space linear velocity to this (in / s)
	float	maxAngularVelocity;						// limit world space angular velocity to this (degrees / s)
	float	lookAheadTimeObjectsVsWorld;			// predict collisions this far (seconds) into the future
	float	lookAheadTimeObjectsVsObject;			// predict collisions this far (seconds) into the future
	float	minFrictionMass;						// min mass for friction solves (constrains dynamic range of mass to improve stability)
	float	maxFrictionMass;						// mas mass for friction solves

	void Defaults()
	{
		maxCollisionsPerObjectPerTimestep = 6 * 3;
		maxCollisionChecksPerTimestep = 250 * 3;
		maxVelocity = 2000.0f * 3;
		maxAngularVelocity = 360.0f * 10.0f * 3;
		lookAheadTimeObjectsVsWorld = 1.0f * 3;
		lookAheadTimeObjectsVsObject = 0.5f * 3;
		minFrictionMass = DEFAULT_MIN_FRICTION_MASS;
		maxFrictionMass = DEFAULT_MAX_FRICTION_MASS;
	}
};


#endif // PERFORMANCE_H
