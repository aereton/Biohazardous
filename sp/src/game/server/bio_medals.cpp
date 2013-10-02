//===== Copyright © 2013, SpotlightEntertainmentStudios, All rights reserved. ========
//
// Purpose: Hidden medals
//
//====================================================================================
#include "cbase.h"
#include "bio_medals.h"

BEGIN_DATADESC( CBaseMedal )
	DEFINE_USEFUNC( Use ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Kill", InputKill ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Enable", InputEnable ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Disable", InputDisable ),
	DEFINE_OUTPUT( m_OnMedalFound, "OnMedalFound" ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( bio_base_medal, CBaseMedal );

#define BASE_MODEL		"models/combine_scanner.mdl"

void CBaseMedal::Precache( void )
{
	PrecacheModel( BASE_MODEL );
	SetModel( BASE_MODEL );

	BaseClass::Precache();
}

void CBaseMedal::Spawn( void )
{
	DisableAutoFade();
	SetModelName( AllocPooledString( BASE_MODEL ) );

	Precache();
	// We want to capture uses from entities
	SetUse( &CBaseMedal::Use );
	
	SetModel( BASE_MODEL );
	SetSolid( SOLID_BBOX );

	// We don't want to move! It's nice here
	SetMoveType( MOVETYPE_NONE );
	BaseClass::Spawn();
}

void CBaseMedal::InputKill( inputdata_t &data )
{
	UTIL_Remove( this );
}

void CBaseMedal::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	//Not if we're disabled
	if( m_bDisabledUse )
		return;

	// Only on clients
	if( !pActivator->IsPlayer() )
		return;
	//Gotcha!
	FoundMedal();
	// After that, kill me plx!
	UTIL_Remove( this );
}

void CBaseMedal::FoundMedal( void )
{
	//Not if we're disabled
	if( m_bDisabledUse )
		return;

	//Get our BasePlayer
	CBasePlayer* pPlayer = UTIL_GetLocalPlayer();
	if( pPlayer )
	{
		CBio_Player *pBioPlayer = dynamic_cast<CBio_Player*>(pPlayer);
		
		// We better add a medal to the player
		pBioPlayer->AddMedal();
		// Already found ALL medals?
		if( pBioPlayer->GetMedals() == pBioPlayer->GetMaxMedals() )
		{
			AllMedalsFound();
		}
		// Check if the amount of player's medals is valid
		else if( pBioPlayer->GetMedals() > pBioPlayer->GetMaxMedals() )
		{
			TooManyMedals();
			return;
		}
	}
	// Fire an output so everyone on the map knows it
	m_OnMedalFound.FireOutput( pPlayer, this );
}

void CBaseMedal::AllMedalsFound( void )
{
	// ToDo:
	// Call here AllMedalsFoundEvent!( Client? )
	Msg( "Wow, you just have found all medals in the game! Congratulations!" );
}

void CBaseMedal::TooManyMedals( void )
{
	Warning( "The player has more medals stored than allowed! Returning to max value!" );
	CBasePlayer* pPlayer = UTIL_GetLocalPlayer();
	if( pPlayer )
	{
		CBio_Player *pBioPlayer = dynamic_cast<CBio_Player*>(pPlayer);

		pBioPlayer->SetMedals( pBioPlayer->GetMaxMedals() );
	}
}

int CBaseMedal::ObjectCaps( void )
{
	int flags = BaseClass::ObjectCaps();
	return ( flags | FCAP_IMPULSE_USE | FCAP_USE_ONGROUND | FCAP_USE_IN_RADIUS );
}