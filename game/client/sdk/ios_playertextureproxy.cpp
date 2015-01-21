#include "cbase.h"
#include "ProxyEntity.h"
#include "materialsystem/IMaterial.h"
#include "materialsystem/IMaterialVar.h"
#include "materialsystem/ITexture.h"
#include <KeyValues.h>

#include "c_sdk_player.h"
#include "c_team.h"
#include "sdk_gamerules.h"
#include "c_playerresource.h"
#include "c_ios_replaymanager.h"
#include "iosoptions.h"
#include "clientmode_sdk.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#include "Filesystem.h"

class CProceduralRegenerator : public ITextureRegenerator
{
public:
	CProceduralRegenerator( void );
	virtual void RegenerateTextureBits( ITexture *pTexture, IVTFTexture *pVTFTexture, Rect_t *pSubRect );
	virtual void Release( void );
	bool SetPlayerInfo(const char *name, int number, bool isKeeper, CFontAtlas *pFontAtlas,
					   const Color &shirtNameFillColor, const Color &shirtNameOutlineColor, int shirtNameVerticalOffset,
					   const Color &shirtBackNumberFillColor, const Color &shirtBackNumberOutlineColor, int shirtBackNumberVerticalOffset,
					   const Color &shortsNumberFillColor, const Color &shortsNumberOutlineColor, int shortsNumberHorizontalOffset, int shortsNumberVerticalOffset,
					   bool hasShirtFrontNumber, const Color &shirtFrontNumberFillColor, const Color &shirtFrontNumberOutlineColor, int shirtFrontNumberHorizontalOffset, int shirtFrontNumberVerticalOffset);
private:
	virtual void WriteText(unsigned char *imageData, const char *text, glyphWithOutline_t **pixels, const chr_t *chars, const int &width, const int &height, int offsetX, int offsetY, const Color &color, int rotation, bool isOutline);
	char m_szShirtName[MAX_PLAYER_NAME_LENGTH];
	char m_szShirtNumber[4];
	bool m_bIsKeeper;
	CFontAtlas *m_pFontAtlas;

	Color m_ShirtNameFillColor;
	Color m_ShirtNameOutlineColor;
	int m_nShirtNameHorizontalOffset;
	int m_nShirtNameVerticalOffset;

	Color m_ShirtBackNumberFillColor;
	Color m_ShirtBackNumberOutlineColor;
	int m_nShirtBackNumberHorizontalOffset;
	int m_nShirtBackNumberVerticalOffset;

	Color m_ShortsNumberFillColor;
	Color m_ShortsNumberOutlineColor;
	int m_nShortsNumberHorizontalOffset;
	int m_nShortsNumberVerticalOffset;

	bool m_bHasShirtFrontNumber;
	Color m_ShirtFrontNumberFillColor;
	Color m_ShirtFrontNumberOutlineColor;
	int m_nShirtFrontNumberHorizontalOffset;
	int m_nShirtFrontNumberVerticalOffset;
};

CProceduralRegenerator::CProceduralRegenerator()
{
	m_szShirtName[0] = '\0';
	m_szShirtNumber[0] = '\0';
	m_bIsKeeper = false;
	m_pFontAtlas = NULL;

	m_ShirtNameFillColor = Color(0, 255, 0, 255);
	m_ShirtNameOutlineColor = Color(0, 255, 0, 255);
	m_nShirtNameHorizontalOffset = 0;
	m_nShirtNameVerticalOffset = 0;

	m_ShirtBackNumberFillColor = Color(0, 255, 0, 255);
	m_ShirtBackNumberOutlineColor = Color(0, 255, 0, 255);
	m_nShirtBackNumberHorizontalOffset = 0;
	m_nShirtBackNumberVerticalOffset = 0;

	m_ShortsNumberFillColor = Color(0, 255, 0, 255);
	m_ShortsNumberOutlineColor = Color(0, 255, 0, 255);
	m_nShortsNumberHorizontalOffset = 0;
	m_nShortsNumberVerticalOffset = 0;

	m_bHasShirtFrontNumber = false;
	m_ShirtFrontNumberFillColor = Color(0, 255, 0, 255);
	m_ShirtFrontNumberOutlineColor = Color(0, 255, 0, 255);
	m_nShirtFrontNumberHorizontalOffset = 0;
	m_nShirtFrontNumberVerticalOffset = 0;
}

bool CProceduralRegenerator::SetPlayerInfo(const char *name, int number, bool isKeeper, CFontAtlas *pFontAtlas,
										   const Color &shirtNameFillColor, const Color &shirtNameOutlineColor, int shirtNameVerticalOffset,
										   const Color &shirtBackNumberFillColor, const Color &shirtBackNumberOutlineColor, int shirtBackNumberVerticalOffset,
										   const Color &shortsNumberFillColor, const Color &shortsNumberOutlineColor, int shortsNumberHorizontalOffset, int shortsNumberVerticalOffset,
										   bool hasShirtFrontNumber, const Color &shirtFrontNumberFillColor, const Color &shirtFrontNumberOutlineColor, int shirtFrontNumberHorizontalOffset, int shirtFrontNumberVerticalOffset)
{
	bool hasChanged = false;

	if (Q_strcmp(name, m_szShirtName))
	{
		Q_strncpy(m_szShirtName, name, sizeof(m_szShirtName));
		hasChanged = true;
	}

	if (number != atoi(m_szShirtNumber))
	{
		Q_snprintf(m_szShirtNumber, sizeof(m_szShirtNumber), "%d", number);
		hasChanged = true;
	}

	if (isKeeper != m_bIsKeeper)
	{
		m_bIsKeeper = isKeeper;
		hasChanged = true;
	}

	if (pFontAtlas != m_pFontAtlas)
	{
		m_pFontAtlas = pFontAtlas;
		hasChanged = true;
	}


	if (shirtNameFillColor != m_ShirtNameFillColor)
	{
		m_ShirtNameFillColor = shirtNameFillColor;
		hasChanged = true;
	}

	if (shirtNameOutlineColor != m_ShirtNameOutlineColor)
	{
		m_ShirtNameOutlineColor = shirtNameOutlineColor;
		hasChanged = true;
	}

	if (shirtNameVerticalOffset != m_nShirtNameVerticalOffset)
	{
		m_nShirtNameVerticalOffset = shirtNameVerticalOffset;
		hasChanged = true;
	}


	if (shirtBackNumberFillColor != m_ShirtBackNumberFillColor)
	{
		m_ShirtBackNumberFillColor = shirtBackNumberFillColor;
		hasChanged = true;
	}

	if (shirtBackNumberOutlineColor != m_ShirtBackNumberOutlineColor)
	{
		m_ShirtBackNumberOutlineColor = shirtBackNumberOutlineColor;
		hasChanged = true;
	}

	if (shirtBackNumberVerticalOffset != m_nShirtBackNumberVerticalOffset)
	{
		m_nShirtBackNumberVerticalOffset = shirtBackNumberVerticalOffset;
		hasChanged = true;
	}


	if (shortsNumberFillColor != m_ShortsNumberFillColor)
	{
		m_ShortsNumberFillColor = shortsNumberFillColor;
		hasChanged = true;
	}

	if (shortsNumberOutlineColor != m_ShortsNumberOutlineColor)
	{
		m_ShortsNumberOutlineColor = shortsNumberOutlineColor;
		hasChanged = true;
	}

	if (shortsNumberHorizontalOffset != m_nShortsNumberHorizontalOffset)
	{
		m_nShortsNumberHorizontalOffset = shortsNumberHorizontalOffset;
		hasChanged = true;
	}

	if (shortsNumberVerticalOffset != m_nShortsNumberVerticalOffset)
	{
		m_nShortsNumberVerticalOffset = shortsNumberVerticalOffset;
		hasChanged = true;
	}


	if (hasShirtFrontNumber != m_bHasShirtFrontNumber)
	{
		m_bHasShirtFrontNumber = hasShirtFrontNumber;
		hasChanged = true;
	}

	if (shirtFrontNumberFillColor != m_ShirtFrontNumberFillColor)
	{
		m_ShirtFrontNumberFillColor = shirtFrontNumberFillColor;
		hasChanged = true;
	}

	if (shirtFrontNumberOutlineColor != m_ShirtFrontNumberOutlineColor)
	{
		m_ShirtFrontNumberOutlineColor = shirtFrontNumberOutlineColor;
		hasChanged = true;
	}

	if (shirtFrontNumberHorizontalOffset != m_nShirtFrontNumberHorizontalOffset)
	{
		m_nShirtFrontNumberHorizontalOffset = shirtFrontNumberHorizontalOffset;
		hasChanged = true;
	}

	if (shirtFrontNumberVerticalOffset != m_nShirtFrontNumberVerticalOffset)
	{
		m_nShirtFrontNumberVerticalOffset = shirtFrontNumberVerticalOffset;
		hasChanged = true;
	}

	return hasChanged;
}

void CProceduralRegenerator::RegenerateTextureBits( ITexture *pTexture, IVTFTexture *pVTFTexture, Rect_t *pSubRect )
{
	int width = pVTFTexture->Width();
	int height = pVTFTexture->Height();

	unsigned char *imageData = pVTFTexture->ImageData(0, 0, 0);
	ImageFormat imageFormat = pVTFTexture->Format();
	if( imageFormat != IMAGE_FORMAT_BGRA8888 )
		return;

	for (int y = 0; y < height; y++ )
	{
		for (int x = 0; x < width; x++ )
		{
			int index = 4 * (y * width + x);
			imageData[index + 0] = 0;
			imageData[index + 1] = 0;
			imageData[index + 2] = 0;
			imageData[index + 3] = 0;
		}
	}

	static int keeperHorizontalOffset = 600;
	static int outfieldHorizontalOffset = 350;


	// Write outline pixels
	
	// Shirt back name
	WriteText(imageData, m_szShirtName, m_pFontAtlas->m_ShirtNamePixels, m_pFontAtlas->m_NameChars, m_pFontAtlas->m_nNamePixelsWidth, m_pFontAtlas->m_nNamePixelsHeight, m_bIsKeeper ? keeperHorizontalOffset : outfieldHorizontalOffset, m_nShirtNameVerticalOffset, m_ShirtNameOutlineColor, 180, true);
	
	// Shirt back number
	WriteText(imageData, m_szShirtNumber, m_pFontAtlas->m_ShirtBackNumberPixels, m_pFontAtlas->m_ShirtBackNumberChars, m_pFontAtlas->m_nShirtBackNumberPixelsWidth, m_pFontAtlas->m_nShirtBackNumberPixelsHeight, m_bIsKeeper ? keeperHorizontalOffset : outfieldHorizontalOffset, m_nShirtBackNumberVerticalOffset, m_ShirtBackNumberOutlineColor, 180, true);	
	
	// Shorts front number
	WriteText(imageData, m_szShirtNumber, m_pFontAtlas->m_ShirtAndShortsNumberPixels, m_pFontAtlas->m_ShirtAndShortsNumberChars, m_pFontAtlas->m_nShirtAndShortsNumberPixelsWidth, m_pFontAtlas->m_nShirtAndShortsNumberPixelsHeight, m_nShortsNumberHorizontalOffset, m_nShortsNumberVerticalOffset, m_ShortsNumberOutlineColor, m_bIsKeeper ? 0 : 270, true);	

	// Shirt front number
	if (m_bHasShirtFrontNumber)
		WriteText(imageData, m_szShirtNumber, m_pFontAtlas->m_ShirtAndShortsNumberPixels, m_pFontAtlas->m_ShirtAndShortsNumberChars, m_pFontAtlas->m_nShirtAndShortsNumberPixelsWidth, m_pFontAtlas->m_nShirtAndShortsNumberPixelsHeight, m_nShirtFrontNumberHorizontalOffset, m_nShirtFrontNumberVerticalOffset, m_ShirtFrontNumberOutlineColor, 0, true);	
	

	// Write glyph pixels
	
	// Shirt back name
	WriteText(imageData, m_szShirtName, m_pFontAtlas->m_ShirtNamePixels, m_pFontAtlas->m_NameChars, m_pFontAtlas->m_nNamePixelsWidth, m_pFontAtlas->m_nNamePixelsHeight, m_bIsKeeper ? keeperHorizontalOffset : outfieldHorizontalOffset, m_nShirtNameVerticalOffset, m_ShirtNameFillColor, 180, false);
	
	// Shirt back number
	WriteText(imageData, m_szShirtNumber, m_pFontAtlas->m_ShirtBackNumberPixels, m_pFontAtlas->m_ShirtBackNumberChars, m_pFontAtlas->m_nShirtBackNumberPixelsWidth, m_pFontAtlas->m_nShirtBackNumberPixelsHeight, m_bIsKeeper ? keeperHorizontalOffset : outfieldHorizontalOffset, m_nShirtBackNumberVerticalOffset, m_ShirtBackNumberFillColor, 180, false);	
	
	// Shorts front number
	WriteText(imageData, m_szShirtNumber, m_pFontAtlas->m_ShirtAndShortsNumberPixels, m_pFontAtlas->m_ShirtAndShortsNumberChars, m_pFontAtlas->m_nShirtAndShortsNumberPixelsWidth, m_pFontAtlas->m_nShirtAndShortsNumberPixelsHeight, m_nShortsNumberHorizontalOffset, m_nShortsNumberVerticalOffset, m_ShortsNumberFillColor, m_bIsKeeper ? 0 : 270, false);	

	// Shirt front number
	if (m_bHasShirtFrontNumber)	
		WriteText(imageData, m_szShirtNumber, m_pFontAtlas->m_ShirtAndShortsNumberPixels, m_pFontAtlas->m_ShirtAndShortsNumberChars, m_pFontAtlas->m_nShirtAndShortsNumberPixelsWidth, m_pFontAtlas->m_nShirtAndShortsNumberPixelsHeight, m_nShirtFrontNumberHorizontalOffset, m_nShirtFrontNumberVerticalOffset, m_ShirtFrontNumberFillColor, 0, false);	
	
}

struct compColor_t
{
	int r;
	int g;
	int b;
	int a;

	compColor_t() : r(0), g(0), b(0), a(0) {}
	compColor_t(const Color &col) : r(col.r()), g(col.g()), b(col.b()), a(col.a()) {}
};

// Use Porter-Duff "over" mode for alpha compositing
void GetCompositeColor(const compColor_t &colorA, const compColor_t &colorB, compColor_t &compColor)
{
	float aR = colorA.r / 255.0f;
	float aG = colorA.g / 255.0f;
	float aB = colorA.b / 255.0f;
	float aA = colorA.a / 255.0f;

	float bR = colorB.r / 255.0f;
	float bG = colorB.g / 255.0f;
	float bB = colorB.b / 255.0f;
	float bA = colorB.a / 255.0f;

	float alpha = aA + bA * (1 - aA);

	compColor.r = (aR * aA + bR * bA * (1 - aA)) / alpha * 255;
	compColor.g = (aG * aA + bG * bA * (1 - aA)) / alpha * 255;
	compColor.b = (aB * aA + bB * bA * (1 - aA)) / alpha * 255;
	compColor.a = alpha * 255;
}

void CProceduralRegenerator::WriteText(unsigned char *imageData, const char *text, glyphWithOutline_t **pixels, const chr_t *chars, const int &width, const int &height, int offsetX, int offsetY, const Color &color, int rotation, bool isOutline)
{
	// Name and Number are upside down on the texture

	int totalWidth = 0;
	int maxHeight = 0;

	// Calculate the total width and max height of the text with kerning
	for (size_t i = 0; i < strlen(text); i++)
	{
		const chr_t &chr = chars[text[i]];
		totalWidth += chr.advanceX;

		if (i > 0)
			totalWidth += chr.GetKerning(text[i - 1]);

		maxHeight = max(maxHeight, chr.height + chr.offsetY);
	}

	int posX;
	int posY;

	if (rotation == 90)
	{
		posX = offsetX + maxHeight / 2;
		posY = offsetY - totalWidth / 2;
	}
	else if (rotation == 180)
	{
		posX = offsetX + totalWidth / 2;
		posY = offsetY + maxHeight / 2;
	}
	else if (rotation == 270)
	{
		posX = offsetX - maxHeight / 2;
		posY = offsetY + totalWidth / 2;
	}
	else
	{
		posX = offsetX - totalWidth / 2;
		posY = offsetY - maxHeight / 2;
	}

	const int textureWidth = 1024;
	const int textureHeight = 1024;

	for (size_t i = 0; i < strlen(text); i++)
	{
		const chr_t &chr = chars[text[i]];

		if (i > 0)
		{
			int kerning = chr.GetKerning(text[i - 1]);

			if (rotation == 90)
				posY += kerning;
			if (rotation == 180)
				posX -= kerning;
			if (rotation == 270)
				posY -= kerning;
			else
				posX += kerning;
		}

		for (int y = 0; y < chr.height; y++)
		{
			for (int x = 0; x < chr.width; x++)
			{
				int destX;
				int destY;

				if (rotation == 90)
				{
					destX = posX - y - chr.offsetY;
					destY = posY + x + chr.offsetX;
				}
				else if (rotation == 180)
				{
					destX = posX - x - chr.offsetX;
					destY = posY - y - chr.offsetY;
				}
				else if (rotation == 270)
				{
					destX = posX + y + chr.offsetY;
					destY = posY - x - chr.offsetX;
				}
				else
				{
					destX = posX + x + chr.offsetX;
					destY = posY + y + chr.offsetY;
				}

				compColor_t curCol;
				int index = clamp(4 * (destY * textureWidth + destX), 0, 4 * (textureWidth * textureHeight) - 4);
				curCol.b = imageData[index + 0];
				curCol.g = imageData[index + 1];
				curCol.r = imageData[index + 2];
				curCol.a = imageData[index + 3];

				int srcX = chr.x + x;
				int srcY = chr.y + y;

				compColor_t col(color);
				col.a = isOutline ? pixels[srcY][srcX].outline : pixels[srcY][srcX].glyph;

				compColor_t compCol;
				GetCompositeColor(col, curCol, compCol);

				imageData[index + 0] = compCol.b;
				imageData[index + 1] = compCol.g;
				imageData[index + 2] = compCol.r;
				imageData[index + 3] = compCol.a;
			}
		}

		if (rotation == 90)
		{
			posY += chr.advanceX;
		}
		else if (rotation == 180)
		{
			posX -= chr.advanceX;
		}
		else if (rotation == 270)
		{
			posY -= chr.advanceX;
		}
		else
		{
			posX += chr.advanceX;
		}
	}
}

void CProceduralRegenerator::Release()
{
	//delete stuff
}

#define TEAMKITS_PATH "models/player/teamkits"

class CPlayerTextureProxy : public CEntityMaterialProxy
{
public:
	CPlayerTextureProxy();
	~CPlayerTextureProxy();
	virtual bool Init( IMaterial *pMaterial, KeyValues *pKeyValues );
	virtual void OnBind( C_BaseEntity *pEnt );
	virtual void Release( void );
	virtual IMaterial *GetMaterial() {return m_pBaseTextureVar->GetOwningMaterial();}

private:
	IMaterialVar	*m_pBaseTextureVar;		// variable for our base texture
	IMaterialVar	*m_pDetailTextureVar;
	ITexture		*m_pTexture;		// default texture
	char m_szTextureType[64];
	CProceduralRegenerator	*m_pTextureRegen[2][11]; // The regenerator
	CProceduralRegenerator *m_pPreviewTextureRegen;
	float m_flLastPreviewTextureUpdate;
	float m_flLastTextureUpdate[2][11];
};

CPlayerTextureProxy::CPlayerTextureProxy()
{
	m_pBaseTextureVar = NULL;
	m_pDetailTextureVar = NULL;
	m_pTexture = NULL;

	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 11; j++)
		{
			m_pTextureRegen[i][j] = NULL;
			m_flLastTextureUpdate[2][11] = -1;
		}
	}

	m_pPreviewTextureRegen = NULL;
	m_flLastPreviewTextureUpdate = -1;
}

CPlayerTextureProxy::~CPlayerTextureProxy()
{
}

void CPlayerTextureProxy::Release()
{
//	for (int i = 0; i < 2; i++)
//		for (int j = 0; j < 11; j++)
//			delete m_pTextureRegen[i][j];
//
//	delete m_pPreviewTextureRegen;
//
	delete this;
}

bool CPlayerTextureProxy::Init( IMaterial *pMaterial, KeyValues *pKeyValues )
{
	#ifdef ALLPROXIESFAIL
	return false;
	#endif

	// Check for $basetexture variable
	m_pBaseTextureVar = pMaterial->FindVar( "$basetexture", NULL );

	if ( !m_pBaseTextureVar )
		return false;

	// Set default texture and make sure its not an error texture
	m_pTexture = m_pBaseTextureVar->GetTextureValue();

	if ( IsErrorTexture( m_pTexture ) )
		return false;
	
	Q_strncpy(m_szTextureType, pKeyValues->GetString("type"), sizeof(m_szTextureType));

	if (!Q_strcmp(m_szTextureType, "shirt") || !Q_strcmp(m_szTextureType, "keepershirt"))
	{
		m_pDetailTextureVar = pMaterial->FindVar("$detail", NULL);
	}

	return true;
}

void CPlayerTextureProxy::OnBind( C_BaseEntity *pEnt )
{
	// Bail if no base variable
	if ( !m_pBaseTextureVar )
		return;

	CTeamKitInfo *pKitInfo;
	const char *teamFolder;
	const char *kitFolder;
	int skinIndex;
	int shirtNumber;
	const char *shirtName;
	bool isKeeper;

	ITexture *pDetailTexture;
	CProceduralRegenerator **pProcReg;
	float *pLastTextureUpdate;

	if (dynamic_cast<C_SDKPlayer *>(pEnt))
	{
		C_SDKPlayer *pPl = dynamic_cast<C_SDKPlayer *>(pEnt);

		C_Team *pTeam = GetGlobalTeam(g_PR->GetTeam(pPl->index));

		teamFolder = pTeam->GetFolderName();
		kitFolder = pTeam->GetKitFolderName();
		pKitInfo = pTeam->GetKitInfo();
		skinIndex = g_PR->GetSkinIndex(pPl->index);
		shirtNumber = g_PR->GetShirtNumber(pPl->index);
		shirtName = g_PR->GetShirtName(pPl->index);
		isKeeper = g_PR->GetTeamPosType(pPl->index) == POS_GK;

		int teamIndex = pTeam->GetTeamNumber() - TEAM_HOME;
		int posIndex = g_PR->GetTeamPosIndex(pPl->index);

		pDetailTexture = materials->FindTexture(VarArgs("models/player/default/detail_%d_%d", teamIndex, posIndex), NULL, true);
		pProcReg = &m_pTextureRegen[teamIndex][posIndex];
		pLastTextureUpdate = &m_flLastTextureUpdate[teamIndex][posIndex];
	}
	else if (dynamic_cast<C_ReplayPlayer *>(pEnt))
	{
		C_ReplayPlayer *pReplayPl = dynamic_cast<C_ReplayPlayer *>(pEnt);

		C_Team *pTeam = GetGlobalTeam(pReplayPl->m_nTeamNumber);

		teamFolder = pTeam->GetFolderName();
		kitFolder = pTeam->GetKitFolderName();
		pKitInfo = pTeam->GetKitInfo();
		skinIndex = pReplayPl->m_nSkinIndex;
		shirtNumber = pReplayPl->m_nShirtNumber;
		shirtName = pReplayPl->m_szShirtName;
		isKeeper = pReplayPl->m_bIsKeeper;

		int teamIndex = pReplayPl->m_nTeamNumber - TEAM_HOME;
		int posIndex = pReplayPl->m_nTeamPosIndex;

		pDetailTexture = materials->FindTexture(VarArgs("models/player/default/detail_%d_%d", teamIndex, posIndex), NULL, true);
		pProcReg = &m_pTextureRegen[teamIndex][posIndex];
		pLastTextureUpdate = &m_flLastTextureUpdate[teamIndex][posIndex];
	}
	else if (dynamic_cast<C_BaseAnimatingOverlay *>(pEnt))
	{
		CAppearanceSettingPanel *pPanel = (CAppearanceSettingPanel *)iosOptionsMenu->GetPanel()->GetSettingPanel(SETTING_PANEL_APPEARANCE);

		pPanel->GetPlayerTeamInfo(&teamFolder, &kitFolder);
		pKitInfo = CTeamInfo::FindTeamByKitName(VarArgs("%s/%s", teamFolder, kitFolder));
		skinIndex = pPanel->GetPlayerSkinIndex();
		shirtNumber = pPanel->GetPlayerOutfieldShirtNumber();
		shirtName = pPanel->GetPlayerShirtName();
		isKeeper = false;

		pDetailTexture = materials->FindTexture("models/player/default/detail_preview", NULL, true);
		pProcReg = &m_pPreviewTextureRegen;
		pLastTextureUpdate = &m_flLastPreviewTextureUpdate;
	}
	else
	{
		return;
	}

	if (!Q_strcmp(m_szTextureType, "shirt") || !Q_strcmp(m_szTextureType, "keepershirt"))
	{
		if (!(*pProcReg))
			*pProcReg = new CProceduralRegenerator();

		pDetailTexture->SetTextureRegenerator(*pProcReg);
		m_pDetailTextureVar->SetTextureValue(pDetailTexture);

		bool needsUpdate;

		if (isKeeper)
		{
			needsUpdate = (*pProcReg)->SetPlayerInfo(shirtName, shirtNumber, true, pKitInfo->m_pFontAtlas,
													 pKitInfo->m_KeeperShirtNameFillColor, pKitInfo->m_KeeperShirtNameOutlineColor, pKitInfo->m_nKeeperShirtNameVerticalOffset,
													 pKitInfo->m_KeeperShirtBackNumberFillColor, pKitInfo->m_KeeperShirtBackNumberOutlineColor, pKitInfo->m_nKeeperShirtBackNumberVerticalOffset,
													 pKitInfo->m_KeeperShortsNumberFillColor, pKitInfo->m_KeeperShortsNumberOutlineColor, pKitInfo->m_nKeeperShortsNumberHorizontalOffset, pKitInfo->m_nKeeperShortsNumberVerticalOffset,
													 pKitInfo->m_bHasKeeperShirtFrontNumber, pKitInfo->m_KeeperShirtFrontNumberFillColor, pKitInfo->m_KeeperShirtFrontNumberOutlineColor, pKitInfo->m_nKeeperShirtFrontNumberHorizontalOffset, pKitInfo->m_nKeeperShirtFrontNumberVerticalOffset);
		}
		else
		{
			needsUpdate = (*pProcReg)->SetPlayerInfo(shirtName, shirtNumber, false, pKitInfo->m_pFontAtlas,
													 pKitInfo->m_OutfieldShirtNameFillColor, pKitInfo->m_OutfieldShirtNameOutlineColor, pKitInfo->m_nOutfieldShirtNameVerticalOffset,
													 pKitInfo->m_OutfieldShirtBackNumberFillColor, pKitInfo->m_OutfieldShirtBackNumberOutlineColor, pKitInfo->m_nOutfieldShirtBackNumberVerticalOffset,
													 pKitInfo->m_OutfieldShortsNumberFillColor, pKitInfo->m_OutfieldShortsNumberOutlineColor, pKitInfo->m_nOutfieldShortsNumberHorizontalOffset, pKitInfo->m_nOutfieldShortsNumberVerticalOffset,
													 pKitInfo->m_bHasOutfieldShirtFrontNumber, pKitInfo->m_OutfieldShirtFrontNumberFillColor, pKitInfo->m_OutfieldShirtFrontNumberOutlineColor, pKitInfo->m_nOutfieldShirtFrontNumberHorizontalOffset, pKitInfo->m_nOutfieldShirtFrontNumberVerticalOffset);
		}
				
		if (needsUpdate || *pLastTextureUpdate <= ClientModeSDKNormal::m_flLastMapChange)
		{
			pDetailTexture->Download();
			*pLastTextureUpdate = gpGlobals->curtime;
		}
	}

	char texture[128];

	if (Q_stricmp(m_szTextureType, "shirt") == 0)
		Q_snprintf(texture, sizeof(texture), "%s/%s/%s/outfield", TEAMKITS_PATH, teamFolder, kitFolder);
	else if (Q_stricmp(m_szTextureType, "keepershirt") == 0)
		Q_snprintf(texture, sizeof(texture), "%s/%s/%s/keeper", TEAMKITS_PATH, teamFolder, kitFolder);
	else if (Q_stricmp(m_szTextureType, "socks") == 0)
		Q_snprintf(texture, sizeof(texture), "%s/%s/%s/socks", TEAMKITS_PATH, teamFolder, kitFolder);
	else if (Q_stricmp(m_szTextureType, "gksocks") == 0)
		Q_snprintf(texture, sizeof(texture), "%s/%s/%s/gksocks", TEAMKITS_PATH, teamFolder, kitFolder);
	else if (Q_stricmp(m_szTextureType, "skin") == 0)
		Q_snprintf(texture, sizeof(texture), "models/player/skins/skin%d", skinIndex + 1);
	else
		Q_snprintf(texture, sizeof(texture), "%s", m_pTexture->GetName());

	ITexture *pNewTex = materials->FindTexture(texture, NULL, false);

	if (!pNewTex->IsError())
		m_pTexture = pNewTex;

	m_pBaseTextureVar->SetTextureValue(m_pTexture);
		
	GetMaterial()->RecomputeStateSnapshots();
}

EXPOSE_INTERFACE( CPlayerTextureProxy, IMaterialProxy, "PlayerTexture" IMATERIAL_PROXY_INTERFACE_VERSION );


class CTextureProxy : public IMaterialProxy
{
public:
	CTextureProxy();
	~CTextureProxy();
	virtual bool Init( IMaterial *pMaterial, KeyValues *pKeyValues );
	virtual void OnBind( void *pEntity );
	virtual void Release( void );
	virtual IMaterial *GetMaterial() {return m_pBaseTextureVar->GetOwningMaterial();}

private:
	IMaterialVar	*m_pBaseTextureVar;		// variable for our base texture
	ITexture		*m_pDefaultTexture;		// default texture
	ITexture		*m_pNewTexture;		// default texture
	char m_szTextureType[64];
};

CTextureProxy::CTextureProxy()
{
	m_pBaseTextureVar = NULL;
	m_pDefaultTexture = NULL;
	m_pNewTexture = NULL;
}

CTextureProxy::~CTextureProxy()
{
	// Do nothing
}
void CTextureProxy::Release()
{
	m_pBaseTextureVar = NULL;
	m_pDefaultTexture = NULL;
	m_pNewTexture = NULL;
	delete this;
}

bool CTextureProxy::Init( IMaterial *pMaterial, KeyValues *pKeyValues )
{
	#ifdef ALLPROXIESFAIL
	return false;
	#endif
	
	// Check for $basetexture variable
	m_pBaseTextureVar = pMaterial->FindVar( "$basetexture", NULL );

	if ( !m_pBaseTextureVar )
		return false;

	// Set default texture and make sure its not an error texture
	m_pDefaultTexture = m_pBaseTextureVar->GetTextureValue();

	if ( IsErrorTexture( m_pDefaultTexture ) )
		return false;
	
	Q_strncpy(m_szTextureType, pKeyValues->GetString("type"), sizeof(m_szTextureType));

	return true;
}

void CTextureProxy::OnBind( void *pEntity )
{
	// Bail if no base variable
	if ( !m_pBaseTextureVar )
		return;

	char texture[128];

	if (Q_stricmp(m_szTextureType, "hometeamcrest") == 0 && GetGlobalTeam(TEAM_HOME)->HasCrest())
		Q_snprintf(texture, sizeof(texture), "%s/%s/teamcrest", TEAMKITS_PATH, GetGlobalTeam(TEAM_HOME)->GetFolderName());
	else if (Q_stricmp(m_szTextureType, "awayteamcrest") == 0 && GetGlobalTeam(TEAM_AWAY)->HasCrest())
		Q_snprintf(texture, sizeof(texture), "%s/%s/teamcrest", TEAMKITS_PATH, GetGlobalTeam(TEAM_AWAY)->GetFolderName());
	else
		Q_snprintf(texture, sizeof(texture), "%s", m_pDefaultTexture->GetName());

	m_pNewTexture = materials->FindTexture(texture, NULL, true);
		
	m_pBaseTextureVar->SetTextureValue(m_pNewTexture);

	GetMaterial()->RecomputeStateSnapshots();
}

EXPOSE_INTERFACE( CTextureProxy, IMaterialProxy, "Texture" IMATERIAL_PROXY_INTERFACE_VERSION );
