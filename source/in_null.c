/*
Copyright (C) 1996-1997 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
// in_null.c -- for systems without a mouse

#include "quakedef.h"
#include <3ds.h>

extern int bind_grab;

extern bool new3ds_flag;
extern bool croshhairmoving;
extern float crosshair_opacity;

extern cvar_t in_analog_strafe;
extern cvar_t in_x_axis_adjust;
extern cvar_t in_y_axis_adjust;
extern cvar_t in_mlook; //Heffo - mlook cvar

void IN_Init (void)
{
	Cvar_RegisterVariable (&in_analog_strafe);
}

void IN_Shutdown (void)
{

}

void IN_Commands (void)
{

}

float IN_CalcInput(int axis, float speed, float tolerance, float acceleration) {

	float value = ((float) axis / 154.0f);

	if (value == 0.0f) {
		return 0.0f;
	}

	float abs_value = fabs(value);

	if (abs_value < tolerance) {
		return 0.0f;
	}

	abs_value -= tolerance;
	abs_value /= (1.0f - tolerance);
	abs_value = powf(abs_value, acceleration);
	abs_value *= speed;

	if (value < 0.0f) {
		value = -abs_value;
	} else {
		value = abs_value;
	}
	return value;
}

extern cvar_t scr_fov;
extern int original_fov, final_fov;
void IN_Move (usercmd_t *cmd)
{
	// TODO: Detect circle pad pro?
	circlePosition left;
	circlePosition right;

	V_StopPitchDrift();

	// Read the pad states
	hidCircleRead(&left);
	hidCstickRead(&right);

	// Convert the inputs to floats in the range [-1, 1].
	// Implement the dead zone.
	float speed;
	float deadZone = in_tolerance.value;
	float acceleration = in_acceleration.value;
	float look_x, look_y;

	// 
	// Analog look tweaks
	//
	speed = sensitivity.value;

	if (!new3ds_flag)
		speed -= 2;

	// cut look speed in half when facing enemy, unless mag is empty
	if ((in_aimassist.value) && (sv_player->v.facingenemy == 1) && cl.stats[STAT_CURRENTMAG] > 0) {
		speed *= 0.5;
	}
	// additionally, slice look speed when ADS/scopes
	if (cl.stats[STAT_ZOOM] == 1)
		speed *= 0.5;
	else if (cl.stats[STAT_ZOOM] == 2)
		speed *= 0.25;
	
	// Are we using the left or right stick for looking?
	if (!new3ds_flag) { // Left
		look_x = IN_CalcInput(left.dx, speed, deadZone, acceleration);
		look_y = IN_CalcInput(left.dy, speed, deadZone, acceleration);
	} else { // Right
		look_x = IN_CalcInput(right.dx, speed, deadZone, acceleration);
		look_y = IN_CalcInput(right.dy, speed, deadZone, acceleration);
	}

	const float yawScale = 30.0f;
	cl.viewangles[YAW] -= yawScale * look_x * host_frametime;

	// Set the pitch.
	const bool invertPitch = m_pitch.value < 0;
	const float pitchScale = yawScale * (invertPitch ? -1 : 1);

	cl.viewangles[PITCH] += pitchScale * look_y * host_frametime;

	// Don't look too far up or down.
	if (cl.viewangles[PITCH] > 80.0f)
		cl.viewangles[PITCH] = 80.0f;
	if (cl.viewangles[PITCH] < -70.0f)
		cl.viewangles[PITCH] = -70.0f;

	// Ability to move with the left nub on NEW model systems
	if (new3ds_flag) {
		float move_x, move_y;

		cl_backspeed = cl_forwardspeed = cl_sidespeed = sv_player->v.maxspeed;
		cl_sidespeed *= 0.8;
		cl_backspeed *= 0.7;

		move_x = IN_CalcInput(left.dx, cl_sidespeed, deadZone, acceleration);

		if (left.dy > 0)
			move_y = IN_CalcInput(left.dy, cl_forwardspeed, deadZone, acceleration);
		else
			move_y = IN_CalcInput(left.dy, cl_backspeed, deadZone, acceleration);

		cmd->sidemove += move_x;
		cmd->forwardmove += move_y;

		// crosshair stuff
		if (left.dx < 50 && left.dx > -50 && left.dy < 50 && left.dy > -50) {
			croshhairmoving = false;

			crosshair_opacity += 22;

			if (crosshair_opacity >= 255)
				crosshair_opacity = 255;
		} else {
			croshhairmoving = true;
			crosshair_opacity -= 8;
			if (crosshair_opacity <= 128)
				crosshair_opacity = 128;
		}
	}
}

