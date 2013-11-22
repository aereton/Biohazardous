#include "cbase.h"
#include "bio_player.h"
#include "baseentity.h"
#include "entityoutput.h"
#include "KeyValues.h"
#include "filesystem.h"
#include "Color.h"
#include "gamestats.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

class CBio_Investigation : public CLogicalEntity
{
public:
	DECLARE_CLASS( CBio_Investigation, CLogicalEntity );
	DECLARE_DATADESC();

	CBio_Investigation()
	{
		m_bDisabled = false;
		m_bIsInUse = false;
		m_nSecondsCounter = 0;
	}

	void UseOptionalCamera();

	void TimerThink();

	void InputTrigger(inputdata_t &inputData);
	void InputEnable(inputdata_t &inputData);
	void InputDisable(inputdata_t &inputData);

	inline void SetMessage( string_t iszMessage ) { m_iszMessage = iszMessage; }
	
private:
	string_t		m_iszMessage;	//Message to display.

	bool			m_bDisabled;
	bool			m_bIsInUse;
	string_t		m_OptionalCamera;
	int				m_nSecondsToHold;
	int				m_nSecondsCounter;

	COutputEvent	m_OnTrigger;
	COutputEvent	m_OnNewCameraAngle;
	COutputEvent	m_OnTimerHit;

	EHANDLE			hOldCam;
};

BEGIN_DATADESC( CBio_Investigation )
	DEFINE_THINKFUNC( TimerThink ),

	DEFINE_FIELD( m_nSecondsCounter, FIELD_INTEGER ),
	DEFINE_FIELD( m_bIsInUse, FIELD_BOOLEAN ),
	DEFINE_FIELD( hOldCam, FIELD_EHANDLE ),

	DEFINE_KEYFIELD( m_bDisabled, FIELD_BOOLEAN, "StartDisabled" ),
	DEFINE_KEYFIELD( m_iszMessage, FIELD_STRING, "message" ),
	DEFINE_KEYFIELD( m_OptionalCamera, FIELD_STRING, "OptionalCamera" ),
	DEFINE_KEYFIELD( m_nSecondsToHold, FIELD_INTEGER, "SecondsToHold" ),

	DEFINE_INPUTFUNC( FIELD_VOID, "Trigger", InputTrigger ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Enable", InputEnable ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Disable", InputDisable ),

	DEFINE_OUTPUT( m_OnTrigger, "OnTrigger" ),
	DEFINE_OUTPUT( m_OnNewCameraAngle, "OnNewCameraAngle" ),
	DEFINE_OUTPUT( m_OnTimerHit, "OnTimerHit" ),

END_DATADESC()

LINK_ENTITY_TO_CLASS( bio_investigation, CBio_Investigation );

void CBio_Investigation::InputTrigger(inputdata_t &inputData)
{
	// Not if disabled
	if (m_bDisabled)
		return;
	// Nor if in use
	if (m_bIsInUse)
		return;

	// Mark as in use
	m_bIsInUse = true;
	// Freeze the player
	CBaseEntity *pPlayer = UTIL_GetLocalPlayer();
	pPlayer->AddFlag( FL_FROZEN );
	// Disable AI
		//TODO!!
	// If OptionalCamera was set, activate it!
	if (m_OptionalCamera != NULL_STRING )
	{
		UseOptionalCamera();
	}
	
	// Show the message
	UTIL_ShowMessage( STRING( m_iszMessage ), ToBasePlayer( pPlayer ) );
	// Fire output!
	m_OnTrigger.FireOutput(inputData.pActivator, inputData.pCaller);
	// Hold it <userdefined> seconds
	SetThink( &CBio_Investigation::TimerThink );
	SetNextThink( gpGlobals->curtime ); // <- now!
}

void CBio_Investigation::UseOptionalCamera( void )
{
	// Get our Player
	CBasePlayer *pPlayer = UTIL_GetLocalPlayer();
	// Get our old camera
	CBaseEntity *pOldCam = dynamic_cast<CBasePlayer*>(pPlayer);
	if (!pOldCam) return;
	pOldCam = pPlayer->GetViewEntity();
	hOldCam = pOldCam; // <- Save it!
	// Get our new camera
	CBaseEntity *pNewCam = gEntList.FindEntityByName( NULL, STRING( m_OptionalCamera ) );
	// Setup new camera
	pPlayer->SetViewEntity( pNewCam );
	// Fire output!
	m_OnNewCameraAngle.FireOutput( pPlayer, this );
}

void CBio_Investigation::InputEnable(inputdata_t &inputData)
{
	m_bDisabled = false;
}

void CBio_Investigation::InputDisable(inputdata_t &inputData)
{
	m_bDisabled = true;
}

void CBio_Investigation::TimerThink( void )
{
	// Increment our counter
	m_nSecondsCounter++;

	// See if we've met or corssed our hold value
	if (m_nSecondsCounter >= m_nSecondsToHold)
	{
		// Get the player
		CBasePlayer *pPlayer = UTIL_GetLocalPlayer();
		// Fire output
		m_OnTimerHit.FireOutput( pPlayer, this );
		// Reset our counter
		m_nSecondsCounter = 0;

		// If optional cam was set, reset to old camera
		if (m_OptionalCamera != NULL_STRING)
		{
			// Convert the EHANDLE back to a CBaseEntity pointer
			CBaseEntity *pOldCam;
			pOldCam = dynamic_cast<CBaseEntity*>( hOldCam.Get() );
			// set to previous camera angle
			pPlayer->SetViewEntity(pOldCam);
		}

		// Unfreeze the player
		pPlayer->RemoveFlag( FL_FROZEN );
		// Re-enable AI
			//TODO!!
		// Not in use anymore!
		m_bIsInUse = false;
		// Stop thinking
		SetNextThink( TICK_NEVER_THINK );
		SetThink( NULL );
	}

	SetNextThink( gpGlobals->curtime + 1 );
}