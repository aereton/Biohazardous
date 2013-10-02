//===== Copyright © 2013, SpotlightEntertainmentStudios, All rights reserved. ========
//
// Purpose: Biohazardous Player Class
//
//====================================================================================
#include "cbase.h"
#include "bio_player.h"

//========================================================
// Set the playermodel here:
#define PLAYER_MDL "models/player.mdl"
//========================================================

//========================================================
// Save our data:
BEGIN_DATADESC( CBio_Player )
	DEFINE_FIELD( m_bIsInterested, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_iDemoMedals, FIELD_INTEGER ),
END_DATADESC()
//========================================================

//========================================================
// Send our data to the client:
IMPLEMENT_SERVERCLASS_ST( CBio_Player, DT_CBio_Player )
	SendPropBool( SENDINFO( m_bIsInterested ) ),
	SendPropInt( SENDINFO( m_iDemoMedals ) ),
END_SEND_TABLE()
//========================================================

LINK_ENTITY_TO_CLASS( player, CBio_Player );

//=============
// Constructor
//=============
CBio_Player::CBio_Player()
{
	m_bIsInterested = false;
	// Define max medals here
	m_iMaxDemoMedals = 3;
}

//========================================================
// Precache function
//========================================================
void CBio_Player::Precache()
{
	PrecacheModel( PLAYER_MDL );
	BaseClass::Precache();
}
//========================================================
// Spawn function
//========================================================
void CBio_Player::Spawn()
{
	SetModel( PLAYER_MDL );
	EquipSuit( false );
	BaseClass::Spawn();
}

//==================================================================
// Points of interest
//==================================================================
// Return our interested-state
//-------------------------------------------
bool CBio_Player::IsInterested() const
{
	return m_bIsInterested;
}
//-------------------------------------------
// Become interested
//-------------------------------------------
void CBio_Player::GetInterested()
{
	m_bIsInterested = true;
	// Placeholder - information!
	DevMsg( "The Player is now interested.\n" );
}
//-------------------------------------------
// Don't be interested anymore
//-------------------------------------------
void CBio_Player::GetUninterested()
{
	m_bIsInterested = false;
	// Placeholder - information!
	DevMsg( "The Player is no longer interested.\n" );
}

//==================================================================
// DemoMedals
//==================================================================
// Return our value of founded medals.
//-------------------------------------------
int CBio_Player::GetMedals()
{
	return m_iDemoMedals;
}
//-------------------------------------------
// Return our value of max medals.
//-------------------------------------------
int CBio_Player::GetMaxMedals()
{
	return m_iMaxDemoMedals;
}
//-------------------------------------------
// We found a medal, so add one.
//-------------------------------------------
void CBio_Player::AddMedal()
{
	// Not if we have maximum already
	if( m_iDemoMedals < m_iMaxDemoMedals )
	{
	m_iDemoMedals++;
	}
}
//-------------------------------------------
// Set our founded-medals value to specific ammount
//-------------------------------------------
void CBio_Player::SetMedals( int nAmount )
{
	if( nAmount > GetMaxMedals() )
	{
		m_iDemoMedals.Set( GetMaxMedals() );
	}
	else if( nAmount <= GetMaxMedals() )
	{
		m_iDemoMedals.Set( nAmount );
	}
}