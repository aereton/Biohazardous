//===== Copyright © 2013, SpotlightEntertainmentStudios, All rights reserved. ========
//
// Purpose: Biohazardous Player Class
//
//====================================================================================
#include "cbase.h"
#include "c_bio_player.h"

IMPLEMENT_CLIENTCLASS_DT( C_Bio_Player, DT_CBio_Player, CBio_Player )
	RecvPropBool( RECVINFO( m_bIsInterested ) ),
END_RECV_TABLE()

C_Bio_Player::C_Bio_Player()
{
}

C_Bio_Player::~C_Bio_Player()
{
}

void C_Bio_Player::OnDataChanged( DataUpdateType_t updateType )
{
	BaseClass::OnDataChanged( updateType );

	if( updateType == DATA_UPDATE_CREATED )
	{
		SetNextClientThink( CLIENT_THINK_ALWAYS );
	}
}

surfacedata_t* C_Bio_Player::GetGroundSurface()
{
	return BaseClass::GetGroundSurface();
}

void C_Bio_Player::UpdateStepSound( surfacedata_t *psurface, const Vector &vecOrigin, const Vector &vecVelocity )
{
}

void C_Bio_Player::UpdateStepSoundOverride( surfacedata_t *psurface, const Vector &vecOrigin, const Vector &vecVelocity )
{
	BaseClass::UpdateStepSound( psurface, vecOrigin, vecVelocity );
}