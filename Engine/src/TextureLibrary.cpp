#include "../include/TextureLibrary.hpp"
#include "../include/stb_image.h"
#include <fstream>
#include <iostream>
#include <cstdint>
#include <filesystem>
#include <stdexcept>

TextureLibrary::TextureLibrary() {}

TextureLibrary::TextureLibrary(const std::filesystem::path& texture_dir_path, GLsizei texture_width, GLsizei texture_height, GLint mipmap_level) 
    : texture_width(texture_width)
    , texture_height(texture_height)
    , mipmap_level(mipmap_level) 
{
    std::vector<uint8_t*> texture_buffers;

    // add image buffer data to vector
    for (auto const& dir_entry : std::filesystem::directory_iterator{ texture_dir_path }) {
        if (!dir_entry.is_regular_file()) {
            std::cerr << "error parsing texture directory: " << texture_dir_path
                << "\n - " << dir_entry << " is not a file of the specified type.\n";
        }

        stbi_set_flip_vertically_on_load(true);

        // load stb image 
        int width, height, channels;
        uint8_t* data = stbi_load(dir_entry.path().string().data(), &width, &height, &channels, 4);

        if (!data) {
            std::cerr << "error loading image data for: " << dir_entry << "\n";
        }
        if (width != texture_width || height != texture_height) {
            std::cerr << "error: " << dir_entry << " does not have the specified dimensions of "
                      << texture_width << " x " << texture_height << "\n";
        }

        texture_name_to_id[dir_entry.path().stem().string()] = texture_buffers.size();
        texture_buffers.push_back(data);
    }

    texture_count = texture_buffers.size();
    glGenTextures(1, &ID);
    bind();

    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, texture_width, texture_height, texture_count, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    
    // upload each buffer into the OpenGL texture array
    for (size_t entry_index = 0; entry_index < texture_buffers.size(); ++entry_index) {
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, entry_index, texture_width, texture_height, 1, GL_RGBA, GL_UNSIGNED_BYTE, texture_buffers[entry_index]);

        // free stb image data from vector
        stbi_image_free(texture_buffers[entry_index]);
    }

    // generate mipmaps
    glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

    // set texture filtering and wrapping parameters
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    unbind();
}

void TextureLibrary::bind() const {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, ID);
}

void TextureLibrary::unbind() const {
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

void TextureLibrary::del() {
    glDeleteTextures(1, &ID);
}

int TextureLibrary::getTextureID(const std::string& texture_name) {
    try {
        return texture_name_to_id.at(texture_name);
    }
    catch (const std::out_of_range& oor) {
        std::cerr << "error accessing TextureID for texture: " << texture_name << "\n"
                  << " - texture: " << texture_name << " does not exist\n";
    }
}
