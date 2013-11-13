#include "cbase.h"

class CBioCam : public CBaseEntity
{
public:
	DECLARE_CLASS( CBioCam, CBaseEntity );

	void Spawn( void );

	virtual int UpdateTransmitState();

	DECLARE_DATADESC();
};

LINK_ENTITY_TO_CLASS( bio_camera, CBioCam );

BEGIN_DATADESC( CBioCam )
END_DATADESC()

void CBioCam::Spawn( void )
{
	BaseClass::Spawn();

	SetMoveType( MOVETYPE_NOCLIP );
	SetSolid( SOLID_NONE );
	SetRenderColorA( 0 );
	
	DispatchUpdateTransmitState();
}

int CBioCam::UpdateTransmitState()
{
	return SetTransmitState( FL_EDICT_ALWAYS );
}