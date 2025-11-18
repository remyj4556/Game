#include "../include/TextureAtlas.hpp"

TextureAtlas::TextureAtlas(const char* path) {

    // can eventually define these dynamically
    const int atlas_width = 32;
    const int atlas_height = 32;
    const int atlas_channels = 4;  // RGBA

    auto output = buildRectangles(path);

    std::vector<stbrp_rect> rects = output.first;
    std::vector<std::string> ids = output.second;

    std::vector<stbrp_node> nodes(atlas_width);

    stbrp_context context;
    stbrp_init_target(&context, atlas_width, atlas_height, nodes.data(), nodes.size());

    // pack the rectangles
    stbrp_pack_rects(&context, rects.data(), rects.size());

    // want to combine all image data in a pixel buffer with their offsets, then write once to the texture atlas
    std::vector<uint8_t> pixels;
    pixels.assign(static_cast<size_t>(atlas_width * atlas_height * atlas_channels), 0);

    for (stbrp_rect& rect : rects) {
        if (!rect.was_packed) {
            std::cerr << "Rect id: " << rect.id << " was not packed, skipping\n";
            continue;
        }

        // get image file for current rect object with id
        std::string current_path = ids[rect.id];

        int width, height, channels;
        unsigned char* data = stbi_load(current_path.c_str(), &width, &height, &channels, atlas_channels);
        if (!data) {
            std::cerr << "Failed to load " << current_path << "\n";
            continue;
        }
        else {
            std::cout << "Successfully loaded: " << current_path << "\n";
        }


        for (size_t y = 0; y < height; ++y) {
            for (size_t x = 0; x < width; ++x) {
                size_t data_index = (y * width + x) * atlas_channels;
                size_t atlas_index = (static_cast<size_t>(y + rect.y) * atlas_width + (x + rect.x)) * atlas_channels;
                for (int c = 0; c < atlas_channels; ++c) {
                    pixels[atlas_index + c] = data[data_index + c];
                }
            }
        }

        stbi_image_free(data);


        // add x,y as uv coords in map for access
        std::filesystem::path image_path(current_path);
        std::string texture_name = image_path.stem().string();
        
        TextureRegion region;
        region.uv_min.x = rect.x / float(atlas_width);
        region.uv_min.y = (atlas_height - (rect.y + rect.h)) / float(atlas_height);
        region.uv_max.x = (rect.x + rect.w) / float(atlas_width);
        region.uv_max.y = (atlas_height - rect.y) / float(atlas_height);

        std::cout << texture_name << ", u min: " << region.uv_min.x << ", v min: " << region.uv_min.y
            << ", u max: " << region.uv_max.x << ", v max: " << region.uv_max.y << "\n";

        regions[texture_name] = region;

    }

    // write image into atlas
    int result = stbi_write_png("atlas.png", atlas_width, atlas_height, atlas_channels, pixels.data(), atlas_width * atlas_channels);

    // assign member texture to be this atlas
    atlas = new Texture("atlas.png", GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE);
}

TextureAtlas::~TextureAtlas() {
    delete atlas;
}

TextureRegion& TextureAtlas::getTextureRegion(const std::string& texture_name) {
    return regions.at(texture_name);
}

// returns a pair of sbrp_rect objects that contain the data for each texture file in an directory,
// and a vector of the filenames, such that the i_th element in the vector contains the file with id i
std::pair<std::vector<stbrp_rect>, std::vector<std::string>> buildRectangles(const std::string& path) {
    const std::filesystem::path directory{ path };
    std::vector<stbrp_rect> rects;
    std::vector<std::string> ids;

    // iterate over directory
    size_t id = 0;
    for (auto const& dir_entry : std::filesystem::directory_iterator{ directory }) {
        //std::cout << dir_entry.path() << std::endl;

        // get dimensions of image file
        int width, height, channels;
        unsigned char* data = stbi_load(dir_entry.path().string().data(), &width, &height, &channels, 4);
        if (!data) {
            std::cerr << "Failed to load (rect): " << dir_entry.path().string() << "\n";
            continue;
        }
        else {
            std::cout << "Successfully loaded (rect): " << dir_entry.path().string() << "\n";
        }

        // push new stbrp_rect object to output vector with this data
        stbrp_rect rect = {};
        rect.id = id;
        rect.w = width;
        rect.h = height;

        rects.push_back(rect);

        // add filename to index id in the vector
        ids.push_back(dir_entry.path().string());

        stbi_image_free(data);

        ++id;
    }

    std::pair<std::vector<stbrp_rect>, std::vector<std::string>> output = { rects, ids };
    return output;
}

void printRects(std::vector<stbrp_rect>& rects) {
    for (const stbrp_rect& rect : rects) {
        std::cout << "id: " << rect.id << ", width: " << rect.w << ", height: " << rect.h
            << ", x: " << rect.x << ", y: " << rect.y << ", was packed: " << rect.was_packed << "\n";
    }
}