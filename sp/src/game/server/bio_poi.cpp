//===== Copyright © 2013, SpotlightEntertainmentStudios, All rights reserved. ========
//
// Purpose: Point of Interests V0.3
//------------------------------------------------------------------------------------
//			Note:	No-Biohazardous-Entities won't be visible if Disabled!
//					Use an interactable entity for AffectedEntity instead!
//			Note2:	If you're using an Biohazardous specific entity, you want to
//					use this as Affected- AND EffectedEntity.
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

	void Interested( void );
	void NotInterested( void );

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


//==============================================================================
// Purpose: Called when spawning, after keyvalues have been handled.
//==============================================================================
void CPointOfInterest::Spawn( void )
{
	BaseClass::Spawn();

	InitTrigger();

	SetNextThink( TICK_NEVER_THINK );
	SetThink( NULL );
	RegisterThinkContext( "ViewconeContext" );
	SetContextThink( NULL, TICK_NEVER_THINK, "ViewconeContext" );
//--------------------------------------
// Disables the affected entity on spawn
//--------------------------------------
	NotInterested();
}
//==============================================================================
// Purpose: checks viewcone state and triggers players state & entity behaviour
//==============================================================================
void CPointOfInterest::EnableThink( void )
{
	// Get a pointer to our own Biohazardous-Playerclass
	CBasePlayer* pPlayer = UTIL_GetLocalPlayer();
	if( pPlayer)
	{
		CBio_Player *pBioPlayer = dynamic_cast<CBio_Player*>(pPlayer);
		if( !m_bInViewcone )
		{
			// If we're interested, don't be it anymore!
			if( pBioPlayer->IsInterested() )
			{
				pBioPlayer->GetUninterested();
				NotInterested();
			}
		}
		else
		{
			// If we're not interested already, get interested!
			if( !pBioPlayer->IsInterested() )
			{
				pBioPlayer->GetInterested();
				Interested();
			}
		}
		// Please think again..
		SetNextThink( gpGlobals->curtime + 0.1 );
	}
	
}
//==============================================================================
// Purpose: Checks if the specified entity is in Players FOV!
//==============================================================================
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


//==============================================================================
// Purpose: Starttouch function
//==============================================================================
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
//==============================================================================
// Purpose: Endtouch function
//==============================================================================
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

	// We better should set the InViewcone bool to false
	// Otherwhise we get interested if we're touching the poi but not looking at the AffectedEntity!
	m_bInViewcone = false;

	// The player should be now uninterested
	CBasePlayer* pPlayer = UTIL_GetLocalPlayer();
	if( pPlayer )
	{
		CBio_Player *pBioPlayer = dynamic_cast<CBio_Player*>(pPlayer);
		if( pBioPlayer->IsInterested() )
		{
			pBioPlayer->GetUninterested();
			NotInterested();
		}
	}
	BaseClass::EndTouch( pOther );
}
//==============================================================================
// Purpose: Executes the BlinkEffect as well as activates the AffectedEntity
//------------------------------------------------------------------------------
// Note:	No-Biohazardous-Entities won't be visible if Disabled!
//			Use an interactable entity for AffectedEntity instead!
// Note2:	If you're using an Biohazardous specific entity, you want to
//			use this as Affected- AND EffectedEntity.
//==============================================================================
void CPointOfInterest::Interested( void )
{
	// Get Our BasePlayer
	CBasePlayer *pPlayer = UTIL_GetLocalPlayer();
	// Get Our AffectedEntity(pEntResult) as well as our EffectedEntity(pEntResultFX)
	CBaseEntity *pEntResult = gEntList.FindEntityByName( NULL, STRING( m_AffectedEntity ) );
	CBaseEntity *pEntResultFX = gEntList.FindEntityByName( NULL, STRING( m_EffectedEntity ) );
	// ToDo
	variant_t emptyVariant;

	if( pEntResult )
	{
		// Enable our AffectedEntity so it can be interacted with
		pEntResult->AcceptInput( "Enable", pPlayer, pEntResult, emptyVariant, 0 );
	}
	if( pEntResultFX )
	{
		// Enable ItemBlink on our EffectedEntity
		pEntResultFX->AddEffects( EF_ITEM_BLINK );
	}
}
//==============================================================================
// Purpose: Removes the BlinkEffect as well as deactivates the AffectedEntity
//------------------------------------------------------------------------------
// Note:	No-Biohazardous-Entities won't be visible if Disabled!
//			Use an interactable entity for AffectedEntity instead!
// Note2:	If you're using an Biohazardous specific entity, you want to
//			use this as Affected- AND EffectedEntity.
//==============================================================================
void CPointOfInterest::NotInterested( void )
{
	// Get Our BasePlayer
	CBasePlayer *pPlayer = UTIL_GetLocalPlayer();
	// Get Our AffectedEntity(pEntResult) as well as our EffectedEntity(pEntResultFX)
	CBaseEntity *pEntResult = gEntList.FindEntityByName( NULL, STRING( m_AffectedEntity ) );
	CBaseEntity *pEntResultFX = gEntList.FindEntityByName( NULL, STRING( m_EffectedEntity ) );
	// ToDo
	variant_t emptyVariant;

	if( pEntResult )
	{
		// Disable our AffectedEntity so it can't be interacted with
		pEntResult->AcceptInput( "Disable", pPlayer, pEntResult, emptyVariant, 0 );
	}
	if( pEntResultFX )
	{
		// Disable ItemBlink on our EffectedEntity
		pEntResultFX->RemoveEffects( EF_ITEM_BLINK );
	}
}