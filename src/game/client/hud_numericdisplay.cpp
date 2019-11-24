//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "hud_numericdisplay.h"
#include "iclientmode.h"

#include <Color.h>
#include <KeyValues.h>
#include <vgui/ISurface.h>
#include <vgui/ISystem.h>
#include <vgui/IVGui.h>

#include <string>
#include <chrono>

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CHudNumericDisplay::CHudNumericDisplay(vgui::Panel *parent, const char *name) : BaseClass(parent, name)
{
	vgui::Panel *pParent = g_pClientMode->GetViewport();
	SetParent( pParent );

	m_iValue = 0;
	m_LabelText[0] = 0;
	m_iSecondaryValue = 0;
	m_bDisplayValue = true;
	m_bDisplaySecondaryValue = false;
	m_bIndent = false;
	m_bIsTime = false;
	m_animate = false;
}

//-----------------------------------------------------------------------------
// Purpose: Resets values on restore/new map
//-----------------------------------------------------------------------------
void CHudNumericDisplay::Reset()
{
	m_flBlur = 0.0f;
}

//-----------------------------------------------------------------------------
// Purpose: data accessor
//-----------------------------------------------------------------------------
void CHudNumericDisplay::SetDisplayValue(int value)
{
	m_iValue = value;
}

//-----------------------------------------------------------------------------
// Purpose: data accessor
//-----------------------------------------------------------------------------
void CHudNumericDisplay::SetSecondaryValue(int value)
{
	m_iSecondaryValue = value;
}

//-----------------------------------------------------------------------------
// Purpose: data accessor
//-----------------------------------------------------------------------------
void CHudNumericDisplay::SetShouldDisplayValue(bool state)
{
	m_bDisplayValue = state;
}

//-----------------------------------------------------------------------------
// Purpose: data accessor
//-----------------------------------------------------------------------------
void CHudNumericDisplay::SetShouldDisplaySecondaryValue(bool state)
{
	m_bDisplaySecondaryValue = state;
}

//-----------------------------------------------------------------------------
// Purpose: data accessor
//-----------------------------------------------------------------------------
void CHudNumericDisplay::SetLabelText(const wchar_t *text)
{
	wcsncpy(m_LabelText, text, sizeof(m_LabelText) / sizeof(wchar_t));
	m_LabelText[(sizeof(m_LabelText) / sizeof(wchar_t)) - 1] = 0;
}

//-----------------------------------------------------------------------------
// Purpose: data accessor
//-----------------------------------------------------------------------------
void CHudNumericDisplay::SetIndent(bool state)
{
	m_bIndent = state;
}

//-----------------------------------------------------------------------------
// Purpose: data accessor
//-----------------------------------------------------------------------------
void CHudNumericDisplay::SetIsTime(bool state)
{
	m_bIsTime = state;
}

//-----------------------------------------------------------------------------
// Purpose: data accessor
//-----------------------------------------------------------------------------
void CHudNumericDisplay::SetAnimate(bool state)
{
	m_animate = state;
}

//-----------------------------------------------------------------------------
// Purpose: paints a number at the specified position
//-----------------------------------------------------------------------------
void CHudNumericDisplay::PaintNumbers(HFont font, int xpos, int ypos, int value)
{
	if (!m_animate)
	{
		surface()->DrawSetTextFont(font);
		wchar_t unicode[6];

		if (!m_bIsTime)
		{
			V_snwprintf(unicode, ARRAYSIZE(unicode), L"%d", value);
		}
		else
		{
			int iMinutes = value / 60;
			int iSeconds = value - iMinutes * 60;
#ifdef PORTAL
			// portal uses a normal font for numbers so we need the seperate to be a renderable ':' char
			if ( iSeconds < 10 )
				V_snwprintf( unicode, ARRAYSIZE(unicode), L"%d:0%d", iMinutes, iSeconds );
			else
				V_snwprintf(unicode, ARRAYSIZE(unicode), L"%d:%d", iMinutes, iSeconds);
#else
			if (iSeconds < 10)
				V_snwprintf(unicode, ARRAYSIZE(unicode), L"%d`0%d", iMinutes, iSeconds);
			else
				V_snwprintf(unicode, ARRAYSIZE(unicode), L"%d`%d", iMinutes, iSeconds);
#endif
		}

		// adjust the position to take into account 3 characters
		int charWidth = surface()->GetCharacterWidth(font, '0');
		if (value < 100 && m_bIndent)
		{
			xpos += charWidth;
		}
		if (value < 10 && m_bIndent)
		{
			xpos += charWidth;
		}

		surface()->DrawSetTextPos(xpos, ypos);
		surface()->DrawUnicodeString(unicode);
	}
	if (m_animate)
	{
		// Animate the 3 digits moving up - down
		surface()->DrawSetTextFont(font);
		wchar_t unicode[6];

		std::string value_str = std::to_string(value);

		int first_int = 0,
			second_int = 0,
			third_int = 0;

		wchar_t first[6], second[6], third[6];

		int size = value_str.size();

		V_snwprintf(unicode, ARRAYSIZE(unicode), L"%d", value);

		if (size > 0) first_int = std::stoi(value_str.substr(0, 1));			// first digit
		if (size > 1) second_int = std::stoi(value_str.substr(1, 1));			// second digit
		if (size > 2) third_int = std::stoi(value_str.substr(2, 1));			// third digit

		if (size > 0) V_snwprintf(first, ARRAYSIZE(first), L"%d", first_int);
		if (size > 1) V_snwprintf(second, ARRAYSIZE(second), L"%d", second_int);
		if (size > 2) V_snwprintf(third, ARRAYSIZE(third), L"%d", third_int);

		int ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

		// adjust the position to take into account 3 characters
		int charWidth = surface()->GetCharacterWidth(font, '0');

		if (size > 0)
		{
			ypos += sin(ms / 100) * 10;
			surface()->DrawSetTextPos(xpos, ypos);
			surface()->DrawUnicodeString(first);
			xpos += charWidth;
		}

		if (size > 1)
		{
			ypos += sin(ms / 100 + 5) * 10;
			surface()->DrawSetTextPos(xpos, ypos);
			surface()->DrawUnicodeString(second);
			xpos += charWidth;
		}

		if (size > 2)
		{
			ypos += sin(ms / 100 + 10) * 10;
			surface()->DrawSetTextPos(xpos, ypos);
			surface()->DrawUnicodeString(third);
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: draws the text
//-----------------------------------------------------------------------------
void CHudNumericDisplay::PaintLabel( void )
{
	surface()->DrawSetTextFont(m_hTextFont);
	surface()->DrawSetTextColor(GetFgColor());
	surface()->DrawSetTextPos(text_xpos, text_ypos);
	surface()->DrawUnicodeString( m_LabelText );
}

//-----------------------------------------------------------------------------
// Purpose: renders the vgui panel
//-----------------------------------------------------------------------------
void CHudNumericDisplay::Paint()
{
	if (m_bDisplayValue)
	{
		// draw our numbers
		surface()->DrawSetTextColor(GetFgColor());
		PaintNumbers(m_hNumberFont, digit_xpos, digit_ypos, m_iValue);

		// draw the overbright blur
		for (float fl = m_flBlur; fl > 0.0f; fl -= 1.0f)
		{
			if (fl >= 1.0f)
			{
				PaintNumbers(m_hNumberGlowFont, digit_xpos, digit_ypos, m_iValue);
			}
			else
			{
				// draw a percentage of the last one
				Color col = GetFgColor();
				col[3] *= fl;
				surface()->DrawSetTextColor(col);
				PaintNumbers(m_hNumberGlowFont, digit_xpos, digit_ypos, m_iValue);
			}
		}
	}

	// total ammo
	if (m_bDisplaySecondaryValue)
	{
		surface()->DrawSetTextColor(GetFgColor());
		PaintNumbers(m_hSmallNumberFont, digit2_xpos, digit2_ypos, m_iSecondaryValue);
	}

	PaintLabel();
}



