// include files necessare to allow compile
#include "cbase.h"
#include "IAlpha_Demo_Splash.h"
using namespace vgui;
#include <vgui/IVGui.h>
#include <vgui_controls/Frame.h>

class CAlphaDemoSplash : public vgui::Frame
{
	DECLARE_CLASS_SIMPLE( CAlphaDemoSplash, vgui::Frame );

	CAlphaDemoSplash( vgui::VPANEL parent );	// Constructor
	~CAlphaDemoSplash(){};						// Destructor

protected:
	// VGUI overrides:
	virtual void OnTick();
	virtual void OnCommand( const char* pcCommand );

private:
	// Other used VGUI control Elements:

};

void CAlphaDemoSplash::OnTick()
{
	BaseClass::OnTick();
}

void CAlphaDemoSplash::OnCommand( const char* pcCommand )
{
	BaseClass::OnCommand( pcCommand );
}

// Constructor: Initializes the Panel
CAlphaDemoSplash::CAlphaDemoSplash( vgui::VPANEL parent )
	: BaseClass( NULL, "AlphaDemoSplash" )
{
	SetParent( parent );

	SetKeyBoardInputEnabled( true );
	SetMouseInputEnabled( true );

	SetProportional( false );
	SetTitleBarVisible( true );
	SetMinimizeButtonVisible( false );
	SetMaximizeButtonVisible( false );
	SetCloseButtonVisible( false );
	SetSizeable( false );
	SetMoveable( false );
	SetVisible( true );

	SetScheme( vgui::scheme()->LoadSchemeFromFile("resource/SourceScheme.res", "SourceScheme" ) );

	LoadControlSettings("resource/UI/AlphaDemo.res");

	vgui::ivgui()->AddTickSignal( GetVPanel(), 100 );
}

// Interface class used for construction.
class CAlphaDemoSplashInterface : public IAlphaDemoSplash
{
private:
	CAlphaDemoSplash *AlphaDemoSplash;
public:
	CAlphaDemoSplashInterface()
	{
		AlphaDemoSplash = NULL;
	}
	void Create( vgui::VPANEL parent )
	{
		AlphaDemoSplash = new CAlphaDemoSplash( parent );
	}
	void Destroy()
	{
		if( AlphaDemoSplash )
		{
			AlphaDemoSplash->SetParent( (vgui::Panel *) NULL );
			delete AlphaDemoSplash;
		}
	}
};

static CAlphaDemoSplashInterface g_AlphaDemoSplash;
IAlphaDemoSplash* alphademosplash = (IAlphaDemoSplash*)&g_AlphaDemoSplash;