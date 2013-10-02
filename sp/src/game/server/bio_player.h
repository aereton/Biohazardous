//===== Copyright © 2013, SpotlightEntertainmentStudios, All rights reserved. ========
//
// Purpose: Biohazardous Player Class
//
//====================================================================================
#ifndef BIO_PLAYER_H
#define BIO_PLAYER_H

#include "hl2_player.h"

class CBio_Player : public CHL2_Player
{
	DECLARE_CLASS( CBio_Player, CHL2_Player );
	DECLARE_SERVERCLASS();
	DECLARE_DATADESC();
public:

	CBio_Player();

	void Precache();
	void Spawn();

	// Points of interest
	bool IsInterested() const;
	void GetInterested();
	void GetUninterested();

	// DemoMedals
	int GetMedals();
	int GetMaxMedals();
	void AddMedal();
	void SetMedals( int nAmount );

	CNetworkVar( bool, m_bIsInterested );
	CNetworkVar( int, m_iDemoMedals );
	CNetworkVar( int, m_iMaxDemoMedals );
protected:
	
private:

};

inline CBio_Player *ToBio_Player( CBaseEntity *pPlayer )
{
	return assert_cast< CBio_Player* >(pPlayer );
}

inline CBio_Player *LocalBio_Player()
{
	return ToBio_Player( UTIL_GetLocalPlayer() );
}

#endif