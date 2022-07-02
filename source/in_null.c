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

extern cvar_t in_analog_strafe;
extern cvar_t in_x_axis_adjust;
extern cvar_t in_y_axis_adjust;
extern cvar_t in_mlook; //Heffo - mlook cvar

void IN_Init (void)
{

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
	// naievil -- fixme this operates incorrectly
	unsigned char analog_strafe = 0;
	// Don't let the pitch drift back to centre if analog nub look is on.
	if (in_mlook.value)
		V_StopPitchDrift();
	else {
		if (in_analog_strafe.value || (in_strafe.state & 1))	{
			analog_strafe = 1;
		}
	}

	// Read the pad state.
	circlePosition pos;
	//Read the CirclePad position
	hidCircleRead(&pos);

	//Print the CirclePad position

	// Convert the inputs to floats in the range [-1, 1].
	// Implement the dead zone.
	float speed;
	float deadZone = in_tolerance.value;
	float acceleration = in_acceleration.value;
	int   x_adjust = 0;
	int   y_adjust = 0;

	//shpuld begin
	if (!analog_strafe) {
		speed = sensitivity.value;

		// ==== Aim Assist + ====
		// cut look speed in half when facing enemy, unless
		// mag is empty
		if ((in_aimassist.value) && (sv_player->v.facingenemy == 1) && cl.stats[STAT_CURRENTMAG] > 0) {
			speed *= 0.5;
		}
		// additionally, slice look speed when ADS/scopes
		if (cl.stats[STAT_ZOOM] == 1)
			speed *= 0.5;
		else if (cl.stats[STAT_ZOOM] == 2)
			speed *= 0.25;
	} else {
		speed = sv_player->v.maxspeed/150;
		if (cl.stats[STAT_ZOOM] == 1)
			speed *= 2;
		else if (cl.stats[STAT_ZOOM] == 2)
			speed *= 4;
	}
	//shpuld end

	float x = IN_CalcInput(pos.dx+x_adjust, speed, deadZone, acceleration);
	float y = IN_CalcInput(pos.dy+y_adjust, speed, deadZone, acceleration);

	// Set the yaw.

	// Analog nub look?
	if (!analog_strafe) {
		const float yawScale = 30.0f;
		cl.viewangles[YAW] -= yawScale * x * host_frametime;

		if (in_mlook.value)
		{
			// Set the pitch.
			const bool invertPitch = m_pitch.value < 0;
			const float pitchScale = yawScale * (invertPitch ? -1 : 1);

			cl.viewangles[PITCH] += pitchScale * y * host_frametime;

			// Don't look too far up or down.
			if (cl.viewangles[PITCH] > 80.0f)
				cl.viewangles[PITCH] = 80.0f;
			if (cl.viewangles[PITCH] < -70.0f)
				cl.viewangles[PITCH] = -70.0f;

			
		}
		else
		{
			// Move using up and down.
			cmd->forwardmove -= cl_forwardspeed * y;
		}
	} else {
		cmd->sidemove += cl_sidespeed * x;
		cmd->forwardmove -= cl_forwardspeed * y;
	}
}

