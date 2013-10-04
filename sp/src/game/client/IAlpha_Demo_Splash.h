// IAlpha_Demo_Splash.h
class IAlphaDemoSplash
{
public:
	virtual void		Create( vgui::VPANEL parent ) = 0;
	virtual void		Destroy( void ) = 0;
};

extern IAlphaDemoSplash* alphademosplash;