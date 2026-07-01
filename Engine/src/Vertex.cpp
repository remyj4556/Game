#include "../include/Vertex.hpp"
#include <cstdint>
#include <cassert>

Vertex::Vertex(uint8_t x, uint8_t y, uint8_t z, uint8_t face_id, uint16_t block_id) {
	data = (x & 0x3F) |
		   ((y & 0x3F) << 6) |
		   ((z & 0x3F) << 12) |
		   ((face_id & 0x7) << 18) |
		   ((block_id & 0x3FFF) << 21);
}  