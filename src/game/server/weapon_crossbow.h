//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

//-----------------------------------------------------------------------------
// CCrossbowBolt
//-----------------------------------------------------------------------------

class CCrossbowBolt : public CBaseCombatCharacter
{
public:
	DECLARE_CLASS(CCrossbowBolt, CBaseCombatCharacter);

	DECLARE_SERVERCLASS();
	DECLARE_ACTTABLE();

	CCrossbowBolt();

	static CCrossbowBolt *CCrossbowBolt::BoltCreate(const Vector &vecOrigin, const QAngle &angAngles, CBasePlayer *pentOwner);

private:
	// Animation event handlers
};