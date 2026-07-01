inline uint8_t Vertex::getX() const {
	return (data) & 0x3F;
}

inline uint8_t Vertex::getY() const {
	return (data >> 6) & 0x3F;
}

inline uint8_t Vertex::getZ() const {
	return (data >> 12) & 0x3F;
}

inline uint8_t Vertex::getFaceId() const {
	return (data >> 18) & 0x7;
}

inline uint16_t Vertex::getBlockId() const {
	return (data >> 21) & 0x3FFF;
}