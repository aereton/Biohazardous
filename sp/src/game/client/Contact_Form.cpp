// necessary to allow to compile
#include "cbase.h"
#include "IContact_Form.h"
#include "filesystem.h"
using namespace vgui;
#include <vgui/IVGui.h>
#include <vgui_controls/Frame.h>
#include <vgui_controls/HTML.h>
#include <vgui_controls/ImagePanel.h>

#define HTML_LOCATION "ingameuse/contact_form.html"

class CContactForm : public vgui::Frame
{
	DECLARE_CLASS_SIMPLE( CContactForm, vgui::Frame );

	CContactForm(vgui::VPANEL parent);	//Constructor
	~CContactForm(){};					//Destructor
protected:
	//VGUI overrides:
	virtual void OnTick();
	virtual void OnCommand(const char* pcCommand);

			void InitBackground();
			void InitHTML();
private:
	//Other used VGUI control Elements:
	vgui::HTML* m_pHTML;
	vgui::ImagePanel* m_pBackground;
};

ConVar cl_showcontactform("cl_showcontactform", "0", FCVAR_CLIENTDLL, "Sets the state of the contact form <state>");

CON_COMMAND(ToggleContactForm, "Toggles the contact form on or off" )
{
	cl_showcontactform.SetValue(!cl_showcontactform.GetBool());
	contactform->Activate();
};

void CContactForm::OnTick()
{
	BaseClass::OnTick();
	SetVisible(cl_showcontactform.GetBool());
}
void CContactForm::OnCommand(const char* pcCommand)
{
	if(!Q_stricmp(pcCommand, "turnoff"))
		cl_showcontactform.SetValue(0);
}

void CContactForm::InitBackground()
{
	m_pBackground = new vgui::ImagePanel( this, "Background" );
	m_pBackground->SetPos(0, 0);
	m_pBackground->SetZPos(2);
	m_pBackground->SetSize(440, 400);
	m_pBackground->SetImage("contact_form/bg");
}

void CContactForm::InitHTML()
{
	m_pHTML = new vgui::HTML( this, "HTML", true, false );
	m_pHTML->SetPos(10, 10);
	m_pHTML->SetZPos(3);
	m_pHTML->SetSize(420, 380);
	
	char temp[1024];
	strcpy(temp, "http://biohazardous.tipido.net/");
	strcat(temp, HTML_LOCATION);

	m_pHTML->OpenURL(temp, false);
}

CContactForm::CContactForm(vgui::VPANEL parent)
	: BaseClass( NULL, "ContactForm" )
{
	SetParent( parent );

	SetKeyBoardInputEnabled( true );
	SetMouseInputEnabled( true );

	SetProportional( false );
	SetTitleBarVisible( false );
	SetMinimizeButtonVisible( false );
	SetMaximizeButtonVisible( false );
	SetCloseButtonVisible( false );
	SetSizeable( false );
	SetMoveable( false );
	SetVisible( true );

	// Don't draw panels background, we'll use an image instead!
	//TODO!!!!!

	SetWide( 440 );
	SetTall( 400 );
	SetZPos( 1 );
	SetRoundedCorners( 0 );

	SetScheme(vgui::scheme()->LoadSchemeFromFile("resource/SourceScheme.res", "SourceScheme"));

	LoadControlSettings("resource/UI/ContactForm.res");
	vgui::ivgui()->AddTickSignal( GetVPanel(), 100 );

	InitBackground();
	InitHTML();
}

class CContactFormInterface : public IContactForm
{
private:
	CContactForm *ContactForm;
public:
	CContactFormInterface()
	{
		ContactForm = NULL;
	}
	void Create(vgui::VPANEL parent)
	{
		ContactForm = new CContactForm(parent);
	}
	void Destroy()
	{
		if (ContactForm)
		{
			ContactForm->SetParent( (vgui::Panel *)NULL);
			delete ContactForm;
		}
	}
	void Activate( void )
	{
		if( ContactForm )
		{
			ContactForm->Activate();
		}
	}
};
static CContactFormInterface g_ContactForm;
IContactForm* contactform = (IContactForm*)&g_ContactForm;