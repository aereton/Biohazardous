#include "cbase.h"
#include "props.h"
#include "BasePropDoor.h"
#include "doors.h"

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
	void Transition(CBaseEntity *pEnt);

	void TeleportThink();
	void FadeInThink();
	void FadeOutThink();

	// Input handlers
	void InputLock(inputdata_t &inputdata);
	void InputUnlock(inputdata_t &inputdata);
	void InputToggleLock(inputdata_t &inputdata);
	void InputEnable(inputdata_t &inputdata);
	void InputDisable(inputdata_t &inputdata);
	// For testing purposes
	void InputTestFadeIN(inputdata_t &inputdata);
	void InputTestFadeOUT(inputdata_t &inputdata);

	int		m_nHardwareType;

	EHANDLE m_hActivator;

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
	
	DEFINE_INPUTFUNC( FIELD_VOID, "Lock", InputLock ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Unlock", InputUnlock ),
	DEFINE_INPUTFUNC( FIELD_VOID, "ToggleLock", InputToggleLock ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Enable", InputEnable ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Disable", InputDisable ),
	// For testing purposes
	DEFINE_INPUTFUNC( FIELD_VOID, "TestFadeIn", InputTestFadeIN ),
	DEFINE_INPUTFUNC( FIELD_VOID, "TestFadeOut", InputTestFadeOUT ),

	DEFINE_OUTPUT( m_OnOpen, "OnOpen" ),
	DEFINE_OUTPUT( m_OnLockedUse, "OnLockedUse" ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( bio_door, CBioPropDoor );


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

	SetNextThink( TICK_NEVER_THINK );
	SetThink( NULL );
}

int CBioPropDoor::ObjectCaps()
{
	return BaseClass::ObjectCaps() | ( HasSpawnFlags( SF_DOOR_IGNORE_USE ) ? 0 : (FCAP_IMPULSE_USE|FCAP_USE_IN_RADIUS) );
}

void CBioPropDoor::Precache( void )
{
	BaseClass::Precache();
}

void CBioPropDoor::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	// Not if we're disabled by PointOfInterest!
	if( m_bDisabled )
		return;

	if( IsDoorLocked() )
	{
		DevMsg( "This door is locked! Can't use!" );
		m_OnLockedUse.FireOutput( pActivator, pCaller );
	}
	else
	{
		DevMsg( "Successfully used! Opening..." );
		DoorOpen();
	}
}

void CBioPropDoor::FadeOutView( CBaseEntity *pEnt )
{
	color32 black = { 0,0,0,255 };
	UTIL_ScreenFade( pEnt, black, 0.6f, 0.0f, (FFADE_OUT|FFADE_STAYOUT) );
}

void CBioPropDoor::FadeInView( CBaseEntity *pEnt )
{
	color32_s clr = { 0,0,0,255 };
	UTIL_ScreenFade( pEnt, clr, 0.4, 0, FFADE_IN | FFADE_PURGE );
}

void CBioPropDoor::Transition( CBaseEntity *pEnt )
{
	//Fade out the view!
	SetThink( &CBioPropDoor::FadeOutThink );
	SetNextThink( gpGlobals->curtime ); // <- now!
	//Fade out sets the teleport think!
	//TeleportThink sets the fade in think!
}

void CBioPropDoor::FadeInThink()
{
	CBasePlayer *pPlayer = UTIL_GetLocalPlayer();
	color32_s clr = { 0,0,0,255 };
	UTIL_ScreenFade( pPlayer, clr, 0.4, 0, FFADE_IN | FFADE_PURGE );
}
void CBioPropDoor::FadeOutThink()
{
	CBasePlayer *pPlayer = UTIL_GetLocalPlayer();
	color32 black = { 0,0,0,255 };
	UTIL_ScreenFade( pPlayer, black, 0.6f, 0.0f, (FFADE_OUT|FFADE_STAYOUT) );
	//Let's teleport!
	SetThink( &CBioPropDoor::TeleportThink );
	SetNextThink( gpGlobals->curtime + 1);
}

void CBioPropDoor::TeleportThink()
{
	CBasePlayer *pPlayer = UTIL_GetLocalPlayer();
	Vector vecTarget = pPlayer->GetAbsOrigin() - WorldSpaceCenter();
	VectorNormalize(vecTarget);

	Vector vecFacing;
	AngleVectors(GetLocalAngles(),&vecFacing);

	float result = DotProduct(vecTarget,vecFacing);

	if (result > 0)
	{
		Msg("Player is in front of me!\n");
		// Player is in front, so teleport him behind me
		Vector vecBackStart, vecBackStop, vecBackDir;
		AngleVectors( this->GetAbsAngles(), &vecBackDir );
			//Negate direction
		vecBackDir.Negate();
		vecBackStart = this->WorldSpaceCenter();
		vecBackStop = vecBackStart + vecBackDir * 24.0f;	//TODO: get closer to the door (maybe 24.0f?)

		//teleport now!
		pPlayer->SetAbsOrigin( vecBackStop );
		Msg("Successfully teleported the player behind the door!\n");
	}
	else
	{
		Msg("Player is behind me!\n");
		// Player is behind me, so teleport him in front of me
		Vector vecFrontStart, vecFrontStop, vecFrontDir;
		AngleVectors( this->GetAbsAngles(), &vecFrontDir );
		vecFrontStart = this->WorldSpaceCenter();
		vecFrontStop = vecFrontStart + vecFrontDir * 24.0f;	//TODO: get closer to the door (maybe 24.0f?)

		//teleport now!
		pPlayer->SetAbsOrigin( vecFrontStop );
		Msg("Successfully teleported the player in front of the door!\n");
	}
	// Fade in the view again!
	SetThink( &CBioPropDoor::FadeInThink );
	SetNextThink( gpGlobals->curtime + 1);
}

// For testing purposes
void CBioPropDoor::InputTestFadeOUT(inputdata_t &inputdata)
{
	
	CBasePlayer *pPlayer = UTIL_GetLocalPlayer();
	if (pPlayer)
	{
		DevMsg( "Fading out the view..." );
		FadeOutView(pPlayer);
	}
}
// For testing purposes
void CBioPropDoor::InputTestFadeIN(inputdata_t &inputdata)
{
	CBasePlayer *pPlayer = UTIL_GetLocalPlayer();
	if (pPlayer)
	{
		DevMsg( "Fading in in the view..." );
		FadeInView(pPlayer);
	}
}

void CBioPropDoor::DoorOpen( void )
{
	DevMsg( "You opened the door!" );
	CBasePlayer *pPlayer = UTIL_GetLocalPlayer();
	if (pPlayer)
	{
		Transition(pPlayer);
	}
}


void CBioPropDoor::InputLock(inputdata_t &inputdata)
{
	m_bLocked = true;
	DevMsg( "The door was locked!" );
}

void CBioPropDoor::InputUnlock(inputdata_t &inputdata)
{
	if (!m_nHardwareType)
	{
		// Doors with no hardware must always be locked.
		DevWarning(1, "Unlocking prop_door '%s' at (%.0f %.0f %.0f) with no hardware. All openable doors must have hardware!\n", GetDebugName(), GetAbsOrigin().x, GetAbsOrigin().y, GetAbsOrigin().z);
	}

	m_bLocked = false;
	DevMsg( "The door was UNlocked!" );
}

void CBioPropDoor::InputToggleLock(inputdata_t &inputdata)
{
	if (m_bLocked)
	{
		m_bLocked = false;
		DevMsg( "The door was UNlocked!" );
	}
	else
	{
		m_bLocked = true;
		DevMsg( "The door was locked!" );
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