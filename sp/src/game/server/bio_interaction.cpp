#include "cbase.h"
#include "bio_player.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

class CBio_Interaction : public CBaseEntity
{
public:
	DECLARE_CLASS( CBio_Interaction, CBaseEntity );
	DECLARE_DATADESC();

	CBio_Interaction()
	{
		m_bDisabled = true;
	}

	void Spawn();
	void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );

	virtual int ObjectCaps( void );

	void InputEnable(inputdata_t &inputdata);
	void InputDisable(inputdata_t &inputdata);
private:
	bool m_bDisabled;

	COutputEvent	m_OnUsed;
};

LINK_ENTITY_TO_CLASS( bio_interaction, CBio_Interaction );

BEGIN_DATADESC( CBio_Interaction )
	DEFINE_USEFUNC( Use ),
	DEFINE_KEYFIELD( m_bDisabled, FIELD_BOOLEAN, "StartDisabled" ),

	DEFINE_INPUTFUNC( FIELD_VOID, "Enable", InputEnable ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Disable", InputDisable ),

	DEFINE_OUTPUT( m_OnUsed, "OnUsed" ),
END_DATADESC()

void CBio_Interaction::Spawn( void )
{
	BaseClass::Spawn();
	
	SetSolid( SOLID_BSP );
	SetModel( STRING( GetModelName() ) );
	SetMoveType( MOVETYPE_NONE );
}

void CBio_Interaction::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if( m_bDisabled )
		return;

	m_OnUsed.FireOutput(pActivator, pCaller);
}

int CBio_Interaction::ObjectCaps( void )
{
	int flags = BaseClass::ObjectCaps();
	return ( flags | FCAP_IMPULSE_USE | FCAP_USE_IN_RADIUS );
}

void CBio_Interaction::InputEnable(inputdata_t &inputdata)
{
	m_bDisabled = false;
}

void CBio_Interaction::InputDisable(inputdata_t &inputdata)
{
	m_bDisabled = true;
}