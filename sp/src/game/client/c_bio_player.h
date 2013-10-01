//===== Copyright © 2013, SpotlightEntertainmentStudios, All rights reserved. ========
//
// Purpose: Biohazardous Player Class
//
//====================================================================================
#ifndef C_BIO_PLAYER_H
#define C_BIO_PLAYER_H

#include "c_basehlplayer.h"

class C_Bio_Player : public C_BaseHLPlayer
{
	DECLARE_CLASS( C_Bio_Player, C_BaseHLPlayer );
	DECLARE_CLIENTCLASS();
public:
	C_Bio_Player();
	~C_Bio_Player();

	virtual void OnDataChanged( DataUpdateType_t updateType );

	virtual void UpdateStepSound( surfacedata_t *psurface, const Vector &vecOrigin, const Vector &vecVelocity );
	virtual void UpdateStepSoundOverride( surfacedata_t *psurface, const Vector &vecOrigin, const Vector &vecVelocity );

	surfacedata_t* GetGroundSurface();


	CNetworkVar( bool, m_bIsInterested );

protected:

private:

};

inline C_Bio_Player *ToBio_Player( C_BaseEntity *pPlayer )
{
	return assert_cast< C_Bio_Player* >( pPlayer );
}

inline C_Bio_Player *LocalBio_Player()
{
	return ToBio_Player( C_BasePlayer::GetLocalPlayer() );
}


#endif