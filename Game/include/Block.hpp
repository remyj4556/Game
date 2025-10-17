#ifndef BLOCK_HPP
#define BLOCK_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

enum class BlockType {
	Air,
	Grass,
	Cobblestone
};

class Block {
	private:
		BlockType type;
		glm::vec3 position;

	public:
		Block();
		Block(const glm::vec3 & position);
		Block(const BlockType type, const glm::vec3 & position);

		// getters
		const BlockType & getType() const;
		const glm::vec3 & getPos() const;
};

Block::Block() : type(BlockType::Air), position(glm::vec3(0.0f, 0.0f, 0.0f)) {}

Block::Block(const glm::vec3 & position) : type(BlockType::Air), position(position) {}

Block::Block(const BlockType type, const glm::vec3 & position) : type(type), position(position) {}

const BlockType& Block::getType() const {
	return type;
}

const glm::vec3& Block::getPos() const {
	return position;
}

#endif