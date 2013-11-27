//===== Copyright © 2013, SpotlightEntertainmentStudios, All rights reserved. ========
//
// Purpose: Camera Trigger V0.1
//------------------------------------------------------------------------------------
//			Note:	-
//====================================================================================
#include "cbase.h"
#include "triggers.h"
#include "bio_player.h"

class CCamTrigger : public CBaseTrigger
{
public:
	DECLARE_CLASS( CCamTrigger, CBaseTrigger );
	DECLARE_DATADESC();

	void Spawn( void );

	void StartTouch( CBaseEntity *pOther );
	void EndTouch( CBaseEntity *pOther );
	void SetupCamera( CBaseEntity *pSetup );
	void FindSetupEnt( void );

private:
	bool m_bDisabled;

	string_t	m_CamSetupEntity;
};

BEGIN_DATADESC( CCamTrigger )
	DEFINE_KEYFIELD( m_bDisabled, FIELD_BOOLEAN, "StartDisabled" ),
	DEFINE_KEYFIELD( m_CamSetupEntity, FIELD_STRING, "CamSetupEntity" ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( bio_cam_trigger, CCamTrigger );

//==============================================================================
// Purpose: Called when spawning, after keyvalues have been handled.
//==============================================================================
void CCamTrigger::Spawn( void )
{
	BaseClass::Spawn();
	InitTrigger();
}
//==============================================================================
// Purpose: StartTouch when Player begins to touch the trigger
//==============================================================================
void CCamTrigger::StartTouch( CBaseEntity *pOther )
{
	if( m_bDisabled )
		return;
	if( !pOther->IsPlayer() )
		return;
	FindSetupEnt();

	BaseClass::StartTouch( pOther );
}
//==============================================================================
// Purpose: EndTouch when Player stops touching the trigger
//==============================================================================
void CCamTrigger::EndTouch( CBaseEntity *pOther )
{
	if( m_bDisabled )
		return;
	if( !pOther->IsPlayer() )
		return;

	BaseClass::EndTouch( pOther );
}
//==============================================================================
// Purpose: Setup the camera / set the camera found by the function FindSetupEnt as ViewEntity
//==============================================================================
void CCamTrigger::SetupCamera( CBaseEntity *pSetup )
{
	CBasePlayer *pPlayer = UTIL_GetLocalPlayer();
	pPlayer->SetViewEntity( pSetup );
}
//==============================================================================
// Purpose: Find the Entity which defines the camera location and angles
//==============================================================================
void CCamTrigger::FindSetupEnt( void )
{
	if( m_CamSetupEntity != NULL_STRING )
	{
		CBaseEntity *pEntResult = gEntList.FindEntityByName( NULL, STRING( m_CamSetupEntity ) );
		if( !pEntResult )
		{
			Warning( "failed to find CamSetupEntity %s.\n", GetEntityName().ToCStr(), STRING( m_CamSetupEntity ) );
			return;
		}
		//now setup the camera with our found entity!
		SetupCamera( pEntResult );
	}
}