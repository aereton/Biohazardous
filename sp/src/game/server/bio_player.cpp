#include "cbase.h"
#include "bio_player.h"

#define PLAYER_MDL "models/player.mdl"


BEGIN_DATADESC( CBio_Player )
	DEFINE_FIELD( m_bIsInterested, FIELD_BOOLEAN ),
END_DATADESC()


IMPLEMENT_SERVERCLASS_ST( CBio_Player, DT_CBio_Player )
	SendPropBool( SENDINFO( m_bIsInterested ) ),
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
	Msg( "YOU'RE NOW AN BIOHAZARDOUS PLAYER! MUHAHAHAHAAHARRRRRR!" );
	SetModel( PLAYER_MDL );
	BaseClass::Spawn();
}