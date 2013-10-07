// IContact_Form.h
class IContactForm
{
public:
	virtual void		Create( vgui::VPANEL parent ) = 0;
	virtual void		Destroy( void ) = 0;
	virtual void		Activate( void ) = 0;
};

extern IContactForm* contactform;