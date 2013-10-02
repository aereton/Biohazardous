//===== Copyright © 2013, SpotlightEntertainmentStudios, All rights reserved. ========
//
// Purpose: Hidden medals
//
//====================================================================================

#include "cbase.h"
#include "props.h"
#include "bio_player.h"

class CBaseMedal : public CDynamicProp
{
public:
	DECLARE_CLASS( CBaseMedal, CDynamicProp );
	DECLARE_DATADESC();

	void Precache( void );
	void Spawn( void );
	void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE, float value );
	void FoundMedal( void );
	void AllMedalsFound( void );
	void TooManyMedals( void );

	virtual int ObjectCaps( void );

	void InputKill( inputdata_t &data );
	void InputEnable( inputdata_t &inputdata )
	{
		m_bDisabledUse = false;
	}
	void InputDisable( inputdata_t &inputdata )
	{
		m_bDisabledUse = true;
	}
private:
	bool m_bDisabledUse;

	COutputEvent m_OnMedalFound;
};