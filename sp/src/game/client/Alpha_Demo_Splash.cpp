// include files necessare to allow compile
#include "cbase.h"
#include "IAlpha_Demo_Splash.h"
using namespace vgui;
#include <vgui/IVGui.h>
#include <vgui_controls/Frame.h>
#include <vgui_controls/ImagePanel.h>

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
	vgui::ImagePanel* m_pBackground; // Panel class declaration
};

ConVar cl_showalphademosplash("cl_showalphademosplash", "1", FCVAR_CLIENTDLL, "Sets the state of AlphaDemoSplash panel <state>");

CON_COMMAND(ToggleAlphaDemoSplash, "Toggles AlphaDemoSplash panel on or off")
{
	cl_showalphademosplash.SetValue(!cl_showalphademosplash.GetBool());
};

void CAlphaDemoSplash::OnTick()
{
	BaseClass::OnTick();
	SetVisible(cl_showalphademosplash.GetBool()); // 1 BY DEFAULT
}

void CAlphaDemoSplash::OnCommand(const char* pcCommand)
{
	if(!Q_stricmp(pcCommand, "turnoff"))
		cl_showalphademosplash.SetValue(0);
}

// Constructor: Initializes the Panel
CAlphaDemoSplash::CAlphaDemoSplash(vgui::VPANEL parent)
	: BaseClass(NULL, "AlphaDemoSplash")
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

	// Don't draw panels background, we'll use an image instead!
	SetPaintBackgroundEnabled( false );

	SetWide( 512 );
	SetTall( 256 );
	SetZPos( 1 );
	SetRoundedCorners( 0 );

	SetScheme( vgui::scheme()->LoadSchemeFromFile("resource/SourceScheme.res", "SourceScheme" ) );

	LoadControlSettings("resource/UI/AlphaDemoSplash.res");

	vgui::ivgui()->AddTickSignal( GetVPanel(), 100 );

	// Content: Other Panels
	//---- Background Image ----
	m_pBackground = new vgui::ImagePanel( this, "BackgroundImage" );
	m_pBackground->SetPos(0, 0);
	m_pBackground->SetZPos( 2 );
	m_pBackground->SetSize(512, 256);
	m_pBackground->SetImage( "alpha_demo_splash/bg" );
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