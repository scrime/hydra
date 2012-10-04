/*  
hydra : Max/MSP & Pd wrapper object for sixense SDK interfacing library.
Compiled for and tested with Mac OSX 10.7
Copyright (C) 2012 Joseph Larralde
 
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>. 
*/

// include flext header
#include <flext.h>

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

// include sixense headers
// download sixense sdk from http://sixense.com/developers
#include <sixense.h>
#include <sixense_math.hpp>
#include <sixense_utils/derivatives.hpp>
#include <sixense_utils/button_states.hpp>
#include <sixense_utils/event_triggers.hpp>
#include <sixense_utils/controller_manager/controller_manager.hpp>

// check for appropriate flext version
#if !defined(FLEXT_VERSION) || (FLEXT_VERSION < 501)
#error You need at least flext version 0.5.1
#endif

class hydra:
	public flext_base
{
	FLEXT_HEADER_S(hydra,flext_base,setup)
 
public:
	
	hydra(int argc, const t_atom *argv);
	~hydra();
    
protected:

	void m_timer(void *);
	void m_nopoll();
	void m_poll(int i);
	//void m_quat(int i);
    
	Timer tmr;

	int interval;
    bool inited;
    bool running;
    //bool quat;
    
private:
    
	void send_hydra_values();
	
	static void setup(t_classid c)
	{
		// --- set up methods (class scope) ---
		
		FLEXT_CADDMETHOD_(c,0,"nopoll",m_nopoll);
		FLEXT_CADDMETHOD_I(c,0,"poll",m_poll);
		//FLEXT_CADDMETHOD_I(c,0,"quat",m_quat);
	}
	
	FLEXT_CALLBACK_T(m_timer)

	// for every registered method a callback has to be declared
	FLEXT_CALLBACK(m_nopoll)
	FLEXT_CALLBACK_I(m_poll)
	//FLEXT_CALLBACK_I(m_quat)
};

// instantiate the class (constructor can have a variable argument list)
// after the colon define the path/name of the help file (a path has a trailing /, a file has not)
FLEXT_NEW_V("hydra : driver for sixense/razer hydra gaming device, jl 2012",hydra)

// _____________________________ CONSTRUCTOR / DESTRUCTOR _______________________________

hydra::hydra(int argc, const t_atom *argv):
tmr(false), inited(false), running(false), interval(0)
{    
    AddInAnything("poll <int>, nopoll");
	AddOutAnything("hydra position, rotation and buttons values");
	
	FLEXT_ADDTIMER(tmr, m_timer);
}

hydra::~hydra()
{
    m_nopoll();
    sixenseExit();
}

//____________________________________ THE REAL CALLBACK ____________________________________

void hydra::m_timer(void *)
{
	//_____ " tick " function _____ :
	send_hydra_values();
}

void hydra::send_hydra_values()
{
	t_atom atom;
	AtomList list;
    
	sixenseAllControllerData acd;
	int base, cont;
	
    for( base=0; base<sixenseGetMaxBases(); base++ )
    {
        sixenseSetActiveBase(base);
        sixenseGetAllNewestData( &acd );
        sixenseUtils::getTheControllerManager()->update( &acd );
        
        for( cont=0; cont<sixenseGetMaxControllers(); cont++ )
        {
            
            if( sixenseIsControllerEnabled( cont ) )
            {                
                //LEFT OUTLET (position and buttons)
                list.Clear();
                
                // base and controller identifiers
                SetInt(atom, base);
                list.Append(atom);
                SetInt(atom, cont);
                list.Append(atom);
                
                // position data
                SetFloat(atom, acd.controllers[cont].pos[0]);
                list.Append(atom);
                SetFloat(atom, acd.controllers[cont].pos[1]);
                list.Append(atom);
                SetFloat(atom, acd.controllers[cont].pos[2]);
                list.Append(atom);
                                
                // rotation data
                SetFloat(atom, acd.controllers[cont].rot_mat[0][0]);
                list.Append(atom);
                SetFloat(atom, acd.controllers[cont].rot_mat[0][1]);
                list.Append(atom);
                SetFloat(atom, acd.controllers[cont].rot_mat[0][2]);
                list.Append(atom);
                SetFloat(atom, acd.controllers[cont].rot_mat[1][0]);
                list.Append(atom);
                SetFloat(atom, acd.controllers[cont].rot_mat[1][1]);
                list.Append(atom);
                SetFloat(atom, acd.controllers[cont].rot_mat[1][2]);
                list.Append(atom);
                SetFloat(atom, acd.controllers[cont].rot_mat[2][0]);
                list.Append(atom);
                SetFloat(atom, acd.controllers[cont].rot_mat[2][1]);
                list.Append(atom);
                SetFloat(atom, acd.controllers[cont].rot_mat[2][2]);
                list.Append(atom);

                // rotation data in quaternion
                SetFloat(atom, acd.controllers[cont].rot_quat[0]);
                list.Append(atom);
                SetFloat(atom, acd.controllers[cont].rot_quat[1]);
                list.Append(atom);
                SetFloat(atom, acd.controllers[cont].rot_quat[2]);
                list.Append(atom);
                SetFloat(atom, acd.controllers[cont].rot_quat[3]);
                list.Append(atom);
                
                // josystick
                SetFloat(atom, acd.controllers[cont].joystick_x);
                list.Append(atom);
                SetFloat(atom, acd.controllers[cont].joystick_y);
                list.Append(atom);
                
                // trigger
                SetFloat(atom, acd.controllers[cont].trigger);
                list.Append(atom);
                
                // buttons
                SetInt(atom, acd.controllers[cont].buttons);
                list.Append(atom);
                
                // output values
                ToOutList(0, list);
            }
        }
    }
}

void hydra::m_nopoll()
{
	tmr.Reset();
	interval = 0;
	post("stopped polling hydra");
	running = false;
}

void hydra::m_poll(int i)
{
    int cont;
    
    if(!inited)
    {
        sixenseInit();
        sixenseUtils::getTheControllerManager()->setGameType(sixenseUtils::ControllerManager::ONE_PLAYER_TWO_CONTROLLER);
        
        // after these calls the controllers will be tracked consistently in the full electromagnetic spherical field
        // but when sending the first poll message, they need to be above the base, otherwise values will be inverted
        for( cont=0; cont<sixenseGetMaxControllers(); cont++ ) {
            sixenseAutoEnableHemisphereTracking(cont);
        }
        
        inited = true;
    }
    
	int j = (i>=1) ? i : 1;
	tmr.Periodic(j*0.001);
	running = true;
	interval = j;
	post("hydra polling interval : %i ms", j);
}

/*
void hydra::m_quat(int i)
{
    quat = (i==0) ? false : true;
}
*/