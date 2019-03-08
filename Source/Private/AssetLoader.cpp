#include "AssetLoader.h"

bool FAssetLoader::LoadResources()
{
	bool bResult = true;
	bResult &= LoadFont(RESOURCES_FONT_CHALK);
	bResult &= LoadFont(RESOURCES_FONT_PIXEL);
	bResult &= LoadTexture(RESOURCES_TEXTURE_CHALKBOARD);
	bResult &= LoadTexture(RESOURCES_TEXTURE_BASKETBALL);
	bResult &= LoadTexture(RESOURCES_TEXTURE_BASKETBALLBOARD);
	bResult &= LoadTexture(RESOURCES_TEXTURE_WOOD);
	bResult &= LoadTexture(RESOURCES_TEXTURE_BOXALT);
	bResult &= LoadMusic(RESOURCES_AUDIO_TROLOLO);
	return bResult;
}

void FAssetLoader::UnloadResources()
{
	for (auto i : TextureLibrary)
	{
		if (!i.second) continue;
		delete i.second;
	}

	TextureLibrary.clear();

	for (auto i : FontLibrary)
	{
		if (!i.second) continue;
		delete i.second;
	}

	FontLibrary.clear();

	for (auto i : MusicLibrary)
	{
		if (!i.second) continue;
		i.second->stop();
		delete i.second;
	}

	MusicLibrary.clear();

}

SFML::Texture* FAssetLoader::FindTexture(const FAssetLoader* ContextObject, const std::string Name)
{
	if (!ContextObject) return nullptr;

	auto Iterator = ContextObject->TextureLibrary.find(Name);
	return (Iterator != ContextObject->TextureLibrary.end()) ? Iterator->second : nullptr ;
}

SFML::Font* FAssetLoader::FindFont(const FAssetLoader* ContextObject, const std::string Name)
{
	if (!ContextObject) return nullptr;

	auto Iterator = ContextObject->FontLibrary.find(Name);
	return (Iterator != ContextObject->FontLibrary.end()) ? Iterator->second : nullptr;
}

SFML::Music * FAssetLoader::FindMusic(const FAssetLoader* ContextObject, const std::string Name)
{
	if (!ContextObject) return nullptr;

	auto Iterator = ContextObject->MusicLibrary.find(Name);
	return (Iterator != ContextObject->MusicLibrary.end()) ? Iterator->second : nullptr;
}

bool FAssetLoader::LoadTexture(const std::string& FileName)
{
	SFML::Texture* pNewTexture = new SFML::Texture();
	bool bResult = pNewTexture->loadFromFile(FileName);
	LOAD_CHECK(bResult, FileName);

	if (bResult)
	{
		TextureLibrary.insert(std::pair<const std::string, SFML::Texture*>(FileName, pNewTexture));
	}
	else
	{
		delete pNewTexture;
	}
	return bResult;
}

bool FAssetLoader::LoadFont(const std::string& FileName)
{
	SFML::Font* pNewFont = new SFML::Font();
	bool bResult = pNewFont->loadFromFile(FileName);
	LOAD_CHECK(bResult, FileName);
	
	if (bResult)
	{
		FontLibrary.insert(std::pair<const std::string, SFML::Font*>(FileName, pNewFont));
	}
	else
	{
		delete pNewFont;
	}
	return bResult;
}

bool FAssetLoader::LoadMusic(const std::string& FileName)
{
	SFML::Music* pNewMusic = new SFML::Music();
	bool bResult = pNewMusic->openFromFile(FileName);
	LOAD_CHECK(bResult, FileName);

	if (bResult)
	{
		MusicLibrary.insert(std::pair<const std::string, SFML::Music*>(FileName, pNewMusic));
	}
	else
	{
		delete pNewMusic;
	}
	return bResult;
}
