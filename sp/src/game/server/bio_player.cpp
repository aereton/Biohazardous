//===== Copyright © 2013, SpotlightEntertainmentStudios, All rights reserved. ========
//
// Purpose: Biohazardous Player Class
//
//====================================================================================
#include "cbase.h"
#include "hl2_player.h"
#include "bio_player.h"

#define PLAYER_MDL "models/Humans/Group03m/female_01.mdl"


BEGIN_DATADESC( CBio_Player )
	DEFINE_FIELD( m_bIsInterested, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_nStatisticDoorsOpened, FIELD_INTEGER ),
END_DATADESC()


IMPLEMENT_SERVERCLASS_ST( CBio_Player, DT_CBio_Player )
	SendPropBool( SENDINFO( m_bIsInterested ) ),
	SendPropInt( SENDINFO( m_nStatisticDoorsOpened ) ),
END_SEND_TABLE()


LINK_ENTITY_TO_CLASS( player, CBio_Player );

//Constructor?
CBio_Player::CBio_Player()
{
	m_bIsInterested = false;
}

void CBio_Player::Precache()
{
	PrecacheModel( PLAYER_MDL );
	BaseClass::Precache();
}

void CBio_Player::Spawn()
{
	BaseClass::Spawn();
	SetModel( PLAYER_MDL );
	EquipSuit( false );
}

//------------------------------------------------------------------
// Point of Interests Interaction!!!!
//------------------------------------------------------------------
bool CBio_Player::IsInterested() const
{
	return m_bIsInterested;
}

void CBio_Player::GetInterested()
{
	m_bIsInterested = true;
}

void CBio_Player::GetUninterested()
{
	m_bIsInterested = false;
}

//------------------------------------------------------------------
// Statistics counter
//------------------------------------------------------------------
void CBio_Player::AddStatisticDoorsOpened( void )
{
	// Add one
	m_nStatisticDoorsOpened++;

	//For Debug: get the value!
	//DevMsg("You currently have %i doors opened!", m_nStatisticDoorsOpened);
}