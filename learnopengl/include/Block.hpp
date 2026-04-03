#ifndef BLOCK_HPP
#define BLOCK_HPP

#include <cstdint>

using block_id_type = uint16_t;

struct Block {
	block_id_type id;
	// metadata like rotation, etc. in future
};

#endif



