#ifndef c_IOS_REPLAYMANAGER_H
#define c_IOS_REPLAYMANAGER_H

#include "cbase.h"
#include "c_physicsprop.h"
#include "ios_teamkit_parse.h"

class C_ReplayBall : public C_PhysicsProp
{
public:
	DECLARE_CLASS(C_ReplayBall, C_PhysicsProp);
	DECLARE_CLIENTCLASS();

	C_ReplayBall();
	~C_ReplayBall();

	char m_szSkinName[MAX_KITNAME_LENGTH];
};

class C_ReplayPlayer : public CBaseAnimatingOverlay
{
public:
	DECLARE_CLASS(C_ReplayPlayer, CBaseAnimatingOverlay);
	DECLARE_CLIENTCLASS();

	C_ReplayPlayer();
	~C_ReplayPlayer();

	int m_nTeamNumber;
	int m_nTeamPosIndex;
	bool m_bIsKeeper;
	int m_nShirtNumber;
	int m_nSkinIndex;
	int m_nHairIndex;
	char m_szPlayerName[MAX_PLAYER_NAME_LENGTH];
	char m_szShirtName[MAX_SHIRT_NAME_LENGTH];
	char m_szShoeName[MAX_KITNAME_LENGTH];
	char m_szKeeperGloveName[MAX_KITNAME_LENGTH];
};

class C_ReplayManager : public CBaseEntity
{
public:
	DECLARE_CLASS(C_ReplayManager, CBaseEntity);
	DECLARE_CLIENTCLASS();

	C_ReplayManager();
	~C_ReplayManager();
	bool IsReplaying();
	bool m_bIsReplaying;
	int	m_nReplayRunIndex;
	bool m_bAtMinGoalPos;
};

extern C_ReplayManager *GetReplayManager();

extern C_ReplayBall *GetReplayBall();

#endif