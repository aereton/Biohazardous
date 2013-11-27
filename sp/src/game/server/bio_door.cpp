//===== Copyright © 2013, SpotlightEntertainmentStudios, All rights reserved. ========
//
// Purpose: DoorTransitionProp V0.1
//------------------------------------------------------------------------------------
//			Note:	-
//====================================================================================
#include "cbase.h"
#include "props.h"
#include "BasePropDoor.h"
#include "doors.h"
#include "bio_player.h"

#define DOOR_HARDWARE_GROUP 1

class CBioPropDoor : public CDynamicProp
{
public:
	DECLARE_CLASS( CBioPropDoor, CDynamicProp );
	DECLARE_DATADESC();

	CBioPropDoor()
	{
		m_bDisabled = true;
	}

	void Spawn();
	void Precache();
	int ObjectCaps();

	inline bool IsDoorLocked();

private:
	void DoorOpen();
	
	void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);

	void FadeOutView(CBaseEntity *pEnt);
	void FadeInView(CBaseEntity *pEnt);
	void Transition();

	void TeleportThink();
	void FadeInThink();
	void FadeOutThink();

	// Input handlers
	void InputLock(inputdata_t &inputdata);
	void InputUnlock(inputdata_t &inputdata);
	void InputToggleLock(inputdata_t &inputdata);
	void InputEnable(inputdata_t &inputdata);
	void InputDisable(inputdata_t &inputdata);

	int		m_nHardwareType;

	EHANDLE m_hActivator;
	EHANDLE m_hPlayer;

	bool	m_bDisabled;
	bool	m_bLocked;

	COutputEvent	m_OnOpen;
	COutputEvent	m_OnLockedUse;
};

bool CBioPropDoor::IsDoorLocked()
{
	return m_bLocked;
}

BEGIN_DATADESC( CBioPropDoor )
	DEFINE_THINKFUNC( TeleportThink ),
	DEFINE_THINKFUNC( FadeInThink ),
	DEFINE_THINKFUNC( FadeOutThink ),

	DEFINE_KEYFIELD( m_nHardwareType, FIELD_INTEGER, "hardware" ),
	DEFINE_FIELD( m_hActivator, FIELD_EHANDLE ),
	DEFINE_FIELD( m_hPlayer, FIELD_EHANDLE ),
	
	DEFINE_INPUTFUNC( FIELD_VOID, "Lock", InputLock ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Unlock", InputUnlock ),
	DEFINE_INPUTFUNC( FIELD_VOID, "ToggleLock", InputToggleLock ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Enable", InputEnable ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Disable", InputDisable ),

	DEFINE_OUTPUT( m_OnOpen, "OnOpen" ),
	DEFINE_OUTPUT( m_OnLockedUse, "OnLockedUse" ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( bio_door, CBioPropDoor );

//==============================================================================
// Purpose: Called when spawning, after keyvalues have been handled.
//==============================================================================
void CBioPropDoor::Spawn()
{
	BaseClass::Spawn();

	m_bDisabled = true;
	DisableAutoFade();
	Precache();

	if( HasSpawnFlags(SF_DOOR_LOCKED))
	{
		m_bLocked = true;
	}

	SetSolid(SOLID_VPHYSICS);
	VPhysicsInitShadow(false,false);

	SetBodygroup( DOOR_HARDWARE_GROUP, m_nHardwareType );
	if((m_nHardwareType == 0) && (!HasSpawnFlags(SF_DOOR_LOCKED)))
	{
		// Doors with no hardware must always be locked.
		DevWarning(1, "Unlocked bio_door '%s' at (%.0f %.0f %.0f) has no hardware. All openable doors must have hardware!\n", GetDebugName(), GetAbsOrigin().x, GetAbsOrigin().y, GetAbsOrigin().z);
	}

	if ( !PropDataOverrodeBlockLOS() )
	{
		CalculateBlockLOS();
	}

	// Don't think yet
	SetNextThink( TICK_NEVER_THINK );
	SetThink( NULL );
}

//==============================================================================
// Purpose: Object caps...
//==============================================================================
int CBioPropDoor::ObjectCaps()
{
	return BaseClass::ObjectCaps() | ( HasSpawnFlags( SF_DOOR_IGNORE_USE ) ? 0 : (FCAP_IMPULSE_USE|FCAP_USE_IN_RADIUS) );
}

//==============================================================================
// Purpose: Precaching
//==============================================================================
void CBioPropDoor::Precache( void )
{
	BaseClass::Precache();
}

//==============================================================================
// Purpose: Called when player is pressing [E] on us
//==============================================================================
void CBioPropDoor::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	// Not if we're disabled by PointOfInterest!
	if( m_bDisabled )
		return;

	if( IsDoorLocked() )
	{
		m_OnLockedUse.FireOutput( pActivator, pCaller );
	}
	else
	{
		DoorOpen();
	}
}

//==============================================================================
// Purpose: Fade out function
//==============================================================================
void CBioPropDoor::FadeOutView( CBaseEntity *pEnt )
{
	color32 black = { 0,0,0,255 };
	UTIL_ScreenFade( pEnt, black, 0.6f, 0.0f, (FFADE_OUT|FFADE_STAYOUT) );
}

//==============================================================================
// Purpose: Fade in function
//==============================================================================
void CBioPropDoor::FadeInView( CBaseEntity *pEnt )
{
	color32_s clr = { 0,0,0,255 };
	UTIL_ScreenFade( pEnt, clr, 0.4, 0, FFADE_IN | FFADE_PURGE );
}

//==============================================================================
// Purpose: Transition initialization
//==============================================================================
void CBioPropDoor::Transition()
{
	//Get player
	CBasePlayer *pPlayer = UTIL_GetLocalPlayer();
	if (!pPlayer) return;
	m_hPlayer = pPlayer; // <- Save it!
	//Freeze the player
	pPlayer->AddFlag( FL_FROZEN );
	//Fade out the view!
	SetThink( &CBioPropDoor::FadeOutThink );
	SetNextThink( gpGlobals->curtime ); // <- now!
	//Fade out sets the teleport think!
	//TeleportThink sets the fade in think!
}

void CBioPropDoor::FadeInThink()
{
	color32_s clr = { 0,0,0,255 };
	UTIL_ScreenFade( m_hPlayer, clr, 0.4, 0, FFADE_IN | FFADE_PURGE );
	// Unfreeze player
	m_hPlayer->RemoveFlag( FL_FROZEN );

	// Add one door in players stats
	CBasePlayer *pPlayer = UTIL_GetLocalPlayer();
	if(pPlayer)
	{
		CBio_Player *pBioPlayer = dynamic_cast<CBio_Player*>(pPlayer);
		pBioPlayer->AddStatisticDoorsOpened();
	}
}
void CBioPropDoor::FadeOutThink()
{
	color32 black = { 0,0,0,255 };
	UTIL_ScreenFade( m_hPlayer, black, 0.6f, 0.0f, (FFADE_OUT|FFADE_STAYOUT) );
	//Let's teleport!
	SetThink( &CBioPropDoor::TeleportThink );
	SetNextThink( gpGlobals->curtime + 1);
}

void CBioPropDoor::TeleportThink()
{
	Vector vecTarget = m_hPlayer->GetAbsOrigin() - WorldSpaceCenter();
	VectorNormalize(vecTarget);

	Vector vecFacing;
	AngleVectors(GetLocalAngles(),&vecFacing);

	float result = DotProduct(vecTarget,vecFacing);

	if (result > 0)
	{
		// Player is in front, so teleport him behind me
		Vector vecBackStart, vecBackStop, vecBackDir;
		AngleVectors( this->GetAbsAngles(), &vecBackDir );
			//Negate direction
		vecBackDir.Negate();
		vecBackStart = this->WorldSpaceCenter();
		vecBackStop = vecBackStart + vecBackDir * 24.0f;	//TODO: get closer to the door (maybe 24.0f?)

		//teleport now!
		m_hPlayer->SetAbsOrigin( vecBackStop );
	}
	else
	{
		// Player is behind me, so teleport him in front of me
		Vector vecFrontStart, vecFrontStop, vecFrontDir;
		AngleVectors( this->GetAbsAngles(), &vecFrontDir );
		vecFrontStart = this->WorldSpaceCenter();
		vecFrontStop = vecFrontStart + vecFrontDir * 24.0f;	//TODO: get closer to the door (maybe 24.0f?)

		//teleport now!
		m_hPlayer->SetAbsOrigin( vecFrontStop );
	}
	// Fade in the view again!
	SetThink( &CBioPropDoor::FadeInThink );
	SetNextThink( gpGlobals->curtime + 1);
}

void CBioPropDoor::DoorOpen( void )
{
	Transition();
}


void CBioPropDoor::InputLock(inputdata_t &inputdata)
{
	m_bLocked = true;
}

void CBioPropDoor::InputUnlock(inputdata_t &inputdata)
{
	if (!m_nHardwareType)
	{
		// Doors with no hardware must always be locked.
		DevWarning(1, "Unlocking prop_door '%s' at (%.0f %.0f %.0f) with no hardware. All openable doors must have hardware!\n", GetDebugName(), GetAbsOrigin().x, GetAbsOrigin().y, GetAbsOrigin().z);
	}

	m_bLocked = false;
}

void CBioPropDoor::InputToggleLock(inputdata_t &inputdata)
{
	if (m_bLocked)
	{
		m_bLocked = false;
	}
	else
	{
		m_bLocked = true;
	}
}

void CBioPropDoor::InputEnable(inputdata_t &inputdata)
{
	m_bDisabled = false;
}

void CBioPropDoor::InputDisable(inputdata_t &inputdata)
{
	m_bDisabled = true;
}