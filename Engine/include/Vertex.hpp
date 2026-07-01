#ifndef VERTEX_HPP
#define VERTEX_HPP

#include <cstdint>

/*
	Vertex is bit packed into a uint32_t. 
	0-5:   local X
	6-11:  local Y
	12-17: local Z
	18-20: face ID
	21-31: block ID

	NOTE: 
	- block ID elsewhere uses block_id_type (alias of uint16_t), here we necessarily use 11 bits. 
	  Will not be a problem unless we exceed 2048 block definitions :D

	- Though 2^5 = 32, we must have at least 6 bits for X,Y,Z since Vertices are in [0,32] (33 positions), while blocks are in [0,31].
	  Thus, using 6 bits gives us possible chunk sizes of [1, 63].
*/

class Vertex {
	public:
		uint32_t data;
	
		Vertex(uint8_t x, uint8_t y, uint8_t z, uint8_t face_id, uint16_t block_id);

		uint8_t getX() const;
		uint8_t getY() const;
		uint8_t getZ() const;
		uint8_t getFaceId() const;
		uint16_t getBlockId() const;
};

#include "Vertex.inl"

#endif