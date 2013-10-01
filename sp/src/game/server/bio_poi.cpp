//===== Copyright © 2013, SpotlightEntertainmentStudios, All rights reserved. ========
//
// Purpose: Point of Interests V0.3
//
//====================================================================================
#include "cbase.h"
#include "triggers.h"
#include "bio_player.h"

class CPointOfInterest : public CBaseTrigger
{
public:
	DECLARE_CLASS( CPointOfInterest, CBaseTrigger );
	DECLARE_DATADESC();

	CPointOfInterest()
	{
		m_bInterested = false;
		m_bInViewcone = false;
		m_bRecentlyTriggered = false;
	}

	void Spawn( void );

	void EnableThink( void );
	void ViewconeThink( void );
	
	void StartTouch( CBaseEntity *pOther );
	void EndTouch( CBaseEntity *pOther );

private:
	bool m_bDisabled;
	bool m_bInterested;
	bool m_bInViewcone;
	bool m_bRecentlyTriggered;

	string_t	m_AffectedEntity;
	string_t	m_EffectedEntity;

};

BEGIN_DATADESC( CPointOfInterest )

	DEFINE_THINKFUNC( EnableThink ),
	DEFINE_THINKFUNC( ViewconeThink ),

	DEFINE_KEYFIELD( m_bDisabled, FIELD_BOOLEAN, "StartDisabled" ),
	DEFINE_KEYFIELD( m_AffectedEntity, FIELD_STRING, "AffectedEntity" ),
	DEFINE_KEYFIELD( m_EffectedEntity, FIELD_STRING, "EffectedEntity" ),

END_DATADESC()

LINK_ENTITY_TO_CLASS( bio_trigger_poi, CPointOfInterest );


//------------------------------------------------------------------------------
// Purpose: Called when spawning, after keyvalues have been handled.
//------------------------------------------------------------------------------
void CPointOfInterest::Spawn( void )
{
	BaseClass::Spawn();

	InitTrigger();

	SetNextThink( TICK_NEVER_THINK );
	SetThink( NULL );
	RegisterThinkContext( "ViewconeContext" );
	SetContextThink( NULL, TICK_NEVER_THINK, "ViewconeContext" );

	// TO DO:
	// Get poi-global point for player
	//--------------
	// Disables the affected entity on spawn
	//--------------
	CBasePlayer *pPlayer = UTIL_GetLocalPlayer();
	CBaseEntity *pEntResult = gEntList.FindEntityByName( NULL, STRING( m_AffectedEntity ) );
	variant_t emptyVariant;
	pEntResult->AcceptInput( "Disable", pPlayer, pEntResult, emptyVariant, 0 );
}
//------------------------------------------------------------------------------
// Purpose: checks viewcone state and triggers players state & entity behaviour
//------------------------------------------------------------------------------
void CPointOfInterest::EnableThink( void )
{
	CBasePlayer* pPlayer = UTIL_GetCommandClient();
	if( pPlayer)
	{
		CBio_Player *pBioPlayer = dynamic_cast<CBio_Player*>(pPlayer);
		if( !m_bInViewcone )
		{
			// ToDo: Entity-Stuff here!
			// If we're interested, get not!
			if( pBioPlayer->IsInterested() )
			{
				pBioPlayer->GetUninterested();
			}
		}
		else
		{
			// ToDo: Entity-Stuff here!
			// If we're not interested already, get interested!
			if( !pBioPlayer->IsInterested() )
			{
				pBioPlayer->GetInterested();
			}
		}
		// Please think again..
		SetNextThink( gpGlobals->curtime + 0.1 );
	}
	
}
//------------------------------------------------------------------------------
// Purpose: Checks if the specified entity is in Players FOV!
//------------------------------------------------------------------------------
void CPointOfInterest::ViewconeThink( void )
{
	CBasePlayer *pPlayer = UTIL_GetLocalPlayer();
	// Only if entity specified!
	if( m_AffectedEntity != NULL_STRING )
	{
		// Find specified entity
		CBaseEntity *pEntResult = gEntList.FindEntityByName( NULL, STRING( m_AffectedEntity ) );
		//Couldn't find entity?
		if( !pEntResult )
		{
			Warning( "failed to find AffectedEntity %s.\n", GetEntityName().ToCStr(), STRING( m_AffectedEntity ) );
			return;
		}
		if( pPlayer->FInViewCone( pEntResult ) )
		{
			m_bInViewcone = true;
		}
		else if( !pPlayer->FInViewCone( pEntResult ) )
		{
			m_bInViewcone = false;
		}
	}
	// No entity specified!
	else
	{
		Warning( "no entity specified as AffectedEntity! returning.. %s.\n", GetEntityName().ToCStr(), STRING( m_AffectedEntity ) );
		return;
	}

	// Please think again
	SetNextThink( gpGlobals->curtime + 0.1, "ViewconeContext" );
}


//------------------------------------------------------------------------------
// Purpose: Starttouch function
//------------------------------------------------------------------------------
void CPointOfInterest::StartTouch( CBaseEntity *pOther )
{
	// Not if we're disabled
	if( m_bDisabled )
		return;
	// Only on clients
	if( !pOther->IsPlayer() )
		return;
	
	// Placeholder - information!
	DevMsg( "Intouch!\n" );

	// Think like Platon
	SetThink( &CPointOfInterest::EnableThink );
	SetNextThink( gpGlobals->curtime ); // <- now!
	SetContextThink( &CPointOfInterest::ViewconeThink, gpGlobals->curtime, "ViewconeContext" );

	BaseClass::StartTouch( pOther );
}
//------------------------------------------------------------------------------
// Purpose: Endtouch function
//------------------------------------------------------------------------------
void CPointOfInterest::EndTouch( CBaseEntity *pOther )
{
	// Not if disabled
	if( m_bDisabled )
		return;

	// Only on clients
	if( !pOther->IsPlayer() )
		return;

	// Placeholder - information!
	DevMsg( "Endtouch!\n" );

	// Headache! Don't think anymore...
	SetNextThink( TICK_NEVER_THINK );
	SetThink( NULL );
	SetContextThink( NULL, TICK_NEVER_THINK, "ViewconeContext" );

	// The player should be now uninterested
	CBasePlayer* pPlayer = UTIL_GetCommandClient();
	if( pPlayer )
	{
		CBio_Player *pBioPlayer = dynamic_cast<CBio_Player*>(pPlayer);
		if( pBioPlayer->IsInterested() )
		{
			pBioPlayer->GetUninterested();
		}
	}
	

	BaseClass::EndTouch( pOther );
}