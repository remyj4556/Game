#ifndef TEXTUREATLAS_HPP
#define TEXTUREATLAS_HPP

#include <unordered_map>
#include <vector>
#include <filesystem>
#include <fstream>
#include <iostream>

#include "TextureRegion.hpp"
#include "stb_rect_pack.h"
#include "stb_image.h"
#include "stb_image_write.h"
#include "Texture.hpp"

void printRects(std::vector<stbrp_rect>& rects);
std::pair<std::vector<stbrp_rect>, std::vector<std::string>> buildRectangles(const std::string& path);

class TextureAtlas {
	private:
		std::unordered_map<std::string, TextureRegion> regions;

	public:
        // ctor takes a path to a *directory* of textures, and builds them into a single atlas
		TextureAtlas(const char* path);
		TextureAtlas();
		~TextureAtlas();



		TextureRegion& getTextureRegion(const std::string& texture_name);
		Texture* atlas;
};



#endif