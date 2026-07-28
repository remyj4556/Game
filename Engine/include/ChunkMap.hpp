#ifndef CHUNKMAP_HPP
#define CHUNKMAP_HPP

#include <unordered_map>
#include <mutex>
#include <utility>
#include <memory>
#include <vector>

#include "Chunk.hpp"
#include "Coordinates.hpp"

// Thread-safe hashmap wrapper
class ChunkMap {
	private:
		mutable std::mutex mtx;
		std::unordered_map<CoordinateSystem::ChunkCoordinates, std::shared_ptr<Chunk>, CoordinateSystem::ChunkCoordinatesHash> raw_map;

	public:
		ChunkMap() = default;

		std::shared_ptr<Chunk> tryGet(CoordinateSystem::ChunkCoordinates chunk_coord) {
			std::unique_lock<std::mutex> l(mtx);
			auto it = raw_map.find(chunk_coord);

			if (it == raw_map.end())
				return nullptr;
			
			return it->second;
		}

		void insert(std::pair<CoordinateSystem::ChunkCoordinates, std::shared_ptr<Chunk>> value) {
			std::unique_lock<std::mutex> l(mtx);
			raw_map [value.first] = std::move(value.second);
		}

		void erase(CoordinateSystem::ChunkCoordinates chunk_coord) {
			std::unique_lock<std::mutex> l(mtx);
			raw_map.erase(chunk_coord);
		}

		bool contains(CoordinateSystem::ChunkCoordinates chunk_coord) const {
			std::unique_lock<std::mutex> l(mtx);
			return raw_map.contains(chunk_coord);
		}

		std::vector<CoordinateSystem::ChunkCoordinates> getAllCoords() const {
			std::vector<CoordinateSystem::ChunkCoordinates> coords;

			std::unique_lock<std::mutex> l(mtx);
			coords.reserve(raw_map.size());

			for (const auto& [chunk_coord, chunk_ptr] : raw_map) {
				coords.push_back(chunk_coord);
			}

			// RVO constructs this vector in place at the call site, negligible performance loss here
			return coords;
		}


};

#endif
