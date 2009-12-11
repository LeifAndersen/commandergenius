/*
 * CPhysicsSettings.cpp
 *
 *  Created on: 26.11.2009
 *      Author: gerstrong
 */

#include "CPhysicsSettings.h"

CPhysicsSettings::CPhysicsSettings() {
	// used for jumping
	player.maxjumpspeed = 103;
	player.defaultjumpupdecreasespeed = 3;
	player.jumpdecrease_x = 54;

	player.max_x_speed = 73;

	// when falling keen's Y inertia increases up to maximum
	player.max_fallspeed = 103;
	player.fallspeed_decrease = 9;

	// Pogo values
	player.pogoforce_x = 20;
}

CPhysicsSettings::~CPhysicsSettings() {
	// TODO Auto-generated destructor stub
}
