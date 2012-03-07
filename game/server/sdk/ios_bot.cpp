//========= Copyright � 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Basic BOT handling.
//
// $Workfile:     $
// $Date:         $
//
//-----------------------------------------------------------------------------
// $Log: $
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "player.h"
#include "sdk_player.h"
#include "in_buttons.h"
#include "movehelper_server.h"
#include "gameinterface.h"
#include "game.h"

#include "ios_keeperbot.h"
#include "ios_fieldbot.h"


ConVar bot_forcefireweapon( "bot_forcefireweapon", "", 0, "Force bots with the specified weapon to fire." );
ConVar bot_forceattack2( "bot_forceattack2", "0", 0, "When firing, use attack2." );
ConVar bot_forceattackon( "bot_forceattackon", "0", 0, "When firing, don't tap fire, hold it down." );
ConVar bot_flipout( "bot_flipout", "0", 0, "When on, all bots fire their guns." );
ConVar bot_changeclass( "bot_changeclass", "0", 0, "Force all bots to change to the specified class." );
static ConVar bot_mimic( "bot_mimic", "0", 0, "Bot uses usercmd of player by index." );
static ConVar bot_mimic_yaw_offset( "bot_mimic_yaw_offset", "0", 0, "Offsets the bot yaw." );
ConVar bot_frozen( "bot_frozen", "0", 0, "Don't do anything." );

ConVar bot_sendcmd( "bot_sendcmd", "", 0, "Forces bots to send the specified command." );

ConVar bot_crouch( "bot_crouch", "0", 0, "Bot crouches" );

static int g_CurBotNumber = 1;

LINK_ENTITY_TO_CLASS( ios_bot, CBot );

class CBotManager
{
public:
	static CBasePlayer* ClientPutInServerOverride_KeeperBot( edict_t *pEdict, const char *playername )
	{
		// This tells it which edict to use rather than creating a new one.
		CBasePlayer::s_PlayerEdict = pEdict;

		CKeeperBot *pPlayer = static_cast<CKeeperBot *>( CreateEntityByName( "ios_keeperbot" ) );
		if ( pPlayer )
		{
			//init bot?
			pPlayer->SetPlayerName( playername );
			pPlayer->m_JoinTime = gpGlobals->curtime;
			Q_memset( &pPlayer->m_cmd, 0, sizeof( pPlayer->m_cmd ) );
		}

		return pPlayer;
	}
	static CBasePlayer* ClientPutInServerOverride_FieldBot( edict_t *pEdict, const char *playername )
	{
		// This tells it which edict to use rather than creating a new one.
		CBasePlayer::s_PlayerEdict = pEdict;

		CFieldBot *pPlayer = static_cast<CFieldBot *>( CreateEntityByName( "ios_fieldbot" ) );
		if ( pPlayer )
		{
			//init bot?
			pPlayer->SetPlayerName( playername );
			pPlayer->m_JoinTime = gpGlobals->curtime;
			//pPlayer->m_fMissTime = 0.0f;
			//pPlayer->m_fNextDive = 0.0f;
			Q_memset( &pPlayer->m_cmd, 0, sizeof( pPlayer->m_cmd ) );
		}

		return pPlayer;
	}
};

//-----------------------------------------------------------------------------
// Purpose: Create a new Bot and put it in the game.
// Output : Pointer to the new Bot, or NULL if there's no free clients.
//-----------------------------------------------------------------------------
CBasePlayer *BotPutInServer( bool bFrozen, int keeper )
{
	char botname[ 64 ];
//	Q_snprintf( botname, sizeof( botname ), "Bot%02i", g_CurBotNumber );

	//pick name
	if (keeper==1)
	{
		Q_snprintf( botname, sizeof( botname ), "KEEPER1");
	}
	else if (keeper==2)
	{
		Q_snprintf( botname, sizeof( botname ), "KEEPER2");
	}
	else
	{
		Q_snprintf( botname, sizeof( botname ), "Arthur");
	}

	// This trick lets us create a CBot for this client instead of the CSDKPlayer
	// that we would normally get when ClientPutInServer is called.
	ClientPutInServerOverrideFn overrideFn;
	if (keeper > 0)
		overrideFn = &CBotManager::ClientPutInServerOverride_KeeperBot;
	else
		overrideFn = &CBotManager::ClientPutInServerOverride_FieldBot;

	ClientPutInServerOverride( overrideFn );
	edict_t *pEdict = engine->CreateFakeClient( botname );
	ClientPutInServerOverride( NULL );

	if (!pEdict)
	{
		Msg( "Failed to create Bot.\n");
		return NULL;
	}

	// Allocate a player entity for the bot, and call spawn
	CBot *pPlayer = ((CBot*)CBaseEntity::Instance( pEdict ));

	pPlayer->ClearFlags();
	pPlayer->AddFlag( FL_CLIENT | FL_FAKECLIENT );

	if ( bFrozen )
		pPlayer->AddEFlags( EFL_BOT_FROZEN );

	pPlayer->m_flNextBotJoin = -1;

	//pPlayer->ChangeTeam( TEAM_UNASSIGNED );
	//pPlayer->RemoveAllItems( true );
	//pPlayer->Spawn();	//spawning here then moving to goal caused the extremely strange keeper teleport bug. Im not sure why!

	pPlayer->BotJoinTeam(keeper);

	return pPlayer;
}

void CBot::BotJoinTeam(int keeper)
{
	int playerCount[2] = {};

	for (int i = 1; i <= gpGlobals->maxClients; i++) 
	{
		CSDKPlayer *pPl = ToSDKPlayer(UTIL_PlayerByIndex(i));
		if (!pPl)
			continue;

		if (!(CSDKPlayer::IsOnField(pPl) || pPl->GetTeamToJoin() != TEAM_INVALID))
			continue;

		int team = CSDKPlayer::IsOnField(pPl) ? pPl->GetTeamNumber() : pPl->GetTeamToJoin();

		playerCount[team - TEAM_A] += 1;
	}

	if (keeper > 0)
	{
		m_TeamPos = 1;
		ConvertSpawnToShirt();
		ChooseKeeperSkin();
		//spawn at correct position
		//pPlayer->Spawn();
		//ChangeTeam(keeper == 1 ? TEAM_A : TEAM_B);
		m_nTeamToJoin = keeper == 1 ? TEAM_A : TEAM_B;
		m_flNextJoin = gpGlobals->curtime + mp_joindelay.GetFloat();
		//ChangeTeam(TEAM_SPECTATOR);
		g_CurBotNumber += 1;
	}
	else
	{
		//int team = g_IOSRand.RandomInt(TEAM_A, TEAM_B);
		//int team = g_CurBotNumber % 2 == 0 ? TEAM_A : TEAM_B;
		int team = playerCount[0] < playerCount[1] ? TEAM_A : TEAM_B;

		if (playerCount[team] == 11)
			team = team == TEAM_A ? TEAM_B : TEAM_A;

		if (playerCount[team] == 11)
			ChangeTeam(TEAM_SPECTATOR);
		else
		{
			while (true)
			{
				int posWanted = g_IOSRand.RandomInt(2,11);
				if (TeamPosFree(team, posWanted, true))
				{
					m_TeamPos = posWanted;
					ConvertSpawnToShirt();
					ChoosePlayerSkin();
					//ChangeTeam(team);
					m_nTeamToJoin = team;
					m_flNextJoin = gpGlobals->curtime + mp_joindelay.GetFloat();
					//ChangeTeam(TEAM_SPECTATOR);
					g_CurBotNumber += 1;
					break;
				}
			}
		}
	}
}

// Handler for the "bot" command.
CON_COMMAND_F( bot_add, "Add a bot.", FCVAR_CHEAT )
{
	// Look at -count.
	int count = args.FindArgInt( "-count", 1 );
	count = clamp( count, 1, 16 );

	// Look at -frozen.
	bool bFrozen = !!args.FindArg( "-frozen" );
		
	// Ok, spawn all the bots.
	while ( --count >= 0 )
	{
		BotPutInServer( bFrozen, 0 );
	}
}

void BotAdd_Keeper1()
{
	//ffs!
	if ( !UTIL_IsCommandIssuedByServerAdmin() )
        return;

	//extern int FindEngineArgInt( const char *pName, int defaultVal );
	//extern const char* FindEngineArg( const char *pName );
	BotPutInServer( FALSE, 1 );
}
void BotAdd_Keeper2()
{
	//ffs!
	if ( !UTIL_IsCommandIssuedByServerAdmin() )
        return;

	//extern int FindEngineArgInt( const char *pName, int defaultVal );
	//extern const char* FindEngineArg( const char *pName );
	BotPutInServer( FALSE, 2 );
}


ConCommand cc_Keeper1( "sv_addkeeper1", BotAdd_Keeper1, "Add keeper1" );
ConCommand cc_Keeper2( "sv_addkeeper2", BotAdd_Keeper2, "Add keeper2" );


//-----------------------------------------------------------------------------
// Purpose: Run through all the Bots in the game and let them think.
//-----------------------------------------------------------------------------
void Bot_RunAll( void )
{
	// Add keeper bot if spot is empty
	if (botkeepers.GetBool())
	{
		bool keeperSpotTaken[2] = {};

		for (int i = 1; i <= gpGlobals->maxClients; i++)
		{
			CSDKPlayer *pPlayer = ToSDKPlayer(UTIL_PlayerByIndex(i));

			if (!pPlayer)
				continue;

			int team = TEAM_INVALID;
			if (CSDKPlayer::IsOnField(pPlayer))
				team = pPlayer->GetTeamNumber();
			else if (pPlayer->GetTeamToJoin() != TEAM_INVALID)
				team = pPlayer->GetTeamToJoin();

			if (pPlayer->IsBot() && pPlayer->GetTeamPosition() == 1)
				team = Q_strcmp(pPlayer->GetPlayerName(), "KEEPER1") == 0 ? TEAM_A : TEAM_B;

			if (team != TEAM_INVALID && pPlayer->GetTeamPosition() == 1)
				keeperSpotTaken[team - TEAM_A] = true;
		}

		for (int i = 0; i < 2; i++)
		{
			if (!keeperSpotTaken[i])
				BotPutInServer(false, i + 1);
		}
	}

	for ( int i = 1; i <= gpGlobals->maxClients; i++ )
	{
		CSDKPlayer *pPlayer = ToSDKPlayer( UTIL_PlayerByIndex( i ) );

		// Ignore plugin bots
		if ( pPlayer && (pPlayer->GetFlags() & FL_FAKECLIENT) && !pPlayer->IsEFlagSet( EFL_PLUGIN_BASED_BOT ) )
		{
			CBot *pBot = dynamic_cast< CBot* >( pPlayer );
			if ( pBot )
				pBot->BotFrame();
		}
	}
}

bool CBot::RunMimicCommand( CUserCmd& cmd )
{
	if ( bot_mimic.GetInt() > gpGlobals->maxClients )
		return false;
	
	CBasePlayer *pPlayer = UTIL_PlayerByIndex( bot_mimic.GetInt()  );
	if ( !pPlayer )
		return false;

	if ( !pPlayer->GetLastUserCommand() )
		return false;

	cmd = *pPlayer->GetLastUserCommand();
	cmd.viewangles[YAW] += bot_mimic_yaw_offset.GetFloat();

	if( bot_crouch.GetInt() )
		cmd.buttons |= IN_DUCK;

	return true;
}

//-----------------------------------------------------------------------------
// Purpose: Simulates a single frame of movement for a player
// Input  : *fakeclient - 
//			*viewangles - 
//			forwardmove - 
//			m_flSideMove - 
//			upmove - 
//			buttons - 
//			impulse - 
//			msec - 
// Output : 	virtual void
//-----------------------------------------------------------------------------
void CBot::RunPlayerMove( CUserCmd &cmd, float frametime )
{
	// Store off the globals.. they're gonna get whacked
	float flOldFrametime = gpGlobals->frametime;
	float flOldCurtime = gpGlobals->curtime;

	float flTimeBase = gpGlobals->curtime + gpGlobals->frametime - frametime;
	SetTimeBase( flTimeBase );

	MoveHelperServer()->SetHost( this );
	PlayerRunCommand( &cmd, MoveHelperServer() );

	// save off the last good usercmd
	SetLastUserCommand( cmd );

	// Clear out any fixangle that has been set
	pl.fixangle = FIXANGLE_NONE;

	// Restore the globals..
	gpGlobals->frametime = flOldFrametime;
	gpGlobals->curtime = flOldCurtime;
}



//-----------------------------------------------------------------------------
// Run this Bot's AI for one frame.
//-----------------------------------------------------------------------------
void CBot::BotFrame()
{
	// Make sure we stay being a bot
	AddFlag( FL_FAKECLIENT );

	Q_memcpy(&m_oldcmd, &m_cmd, sizeof(m_oldcmd));
	Q_memset(&m_cmd, 0, sizeof(m_cmd));

	if (GetTeamNumber() == TEAM_SPECTATOR && m_nTeamToJoin == TEAM_INVALID)
	{
		if (m_flNextBotJoin == -1)
			m_flNextBotJoin = gpGlobals->curtime + 3;
		else if (gpGlobals->curtime >= m_flNextBotJoin)
		{
			BotJoinTeam(0);
			m_flNextBotJoin = -1;
		}
	}

	if (CSDKPlayer::IsOnField(this))
	{
		if (bot_frozen.GetBool())
		{
		}
		else if (bot_mimic.GetInt() > 0)
			RunMimicCommand(m_cmd);
		else
		{
			GetBall()->VPhysicsGetObject()->GetPosition(&m_vBallPos, &m_aBallAng);
			GetBall()->VPhysicsGetObject()->GetVelocity(&m_vBallVel, &m_vBallAngImp);

			m_vDirToBall = m_vBallPos - GetLocalOrigin();
			if (m_vBallVel.Length2D() == 0)
			{
				m_flAngToBallVel = 0;
			}
			else
			{
				Vector vel = m_vBallVel;
				vel.z = 0;
				vel.NormalizeInPlace();
				Vector dir = -m_vDirToBall;
				dir.z = 0;
				dir.NormalizeInPlace();
				m_flAngToBallVel = RAD2DEG(acos(dir.Dot(vel)));
			}

			BotThink();

			if (m_cmd.viewangles[YAW] > 180.0f )
				m_cmd.viewangles[YAW] -= 360.0f;
			else if ( m_cmd.viewangles[YAW] < -180.0f )
				m_cmd.viewangles[YAW] += 360.0f;

			if (m_cmd.viewangles[PITCH] > 180.0f )
				m_cmd.viewangles[PITCH] -= 360.0f;
			else if ( m_cmd.viewangles[PITCH] < -180.0f )
				m_cmd.viewangles[PITCH] += 360.0f;

			m_LastAngles = m_cmd.viewangles;
			SetLocalAngles(m_cmd.viewangles);
			SnapEyeAngles(m_cmd.viewangles);
		}
	}

	// Fix up the m_fEffects flags
	PostClientMessagesSent();

	float frametime = gpGlobals->frametime;
	//ios RunPlayerMove( pBot, cmd, frametime );
	RunPlayerMove( m_cmd, frametime );
}

void CBot::BotThink()
{
}