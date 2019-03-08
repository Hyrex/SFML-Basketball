#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Defines.h"


class FAssetLoader
{
public:
	FAssetLoader() {};
	~FAssetLoader() {};
	bool LoadResources();
	void UnloadResources();
	static SFML::Texture*	FindTexture(const FAssetLoader* ContextObject, const std::string Name);
	static SFML::Font*		FindFont(const FAssetLoader* ContextObject, const std::string Name);
	static SFML::Music*		FindMusic(const FAssetLoader* ContextObject, const std::string Name);
	
private:
	std::map<const std::string, SFML::Texture*> TextureLibrary;
	std::map<const std::string, SFML::Font*>	FontLibrary;
	std::map<const std::string, SFML::Music*>	MusicLibrary;

	bool LoadTexture(const std::string& FileName);
	bool LoadFont(const std::string& FileName);
	bool LoadMusic(const std::string& FileName);

};

