#include "../include/ChunkMesher.hpp"
#include "../include/Chunk.hpp"
#include "../include/Vertex.hpp"
#include "../include/BlockDefinition.hpp"
#include "../include/glm/fwd.hpp"
#include "../include/glm/glm.hpp"

#include <vector>
#include <utility>
#include <cstdint>
#include <array>
#include <glad/glad.h>

std::pair<std::vector<Vertex>, std::vector<GLuint>> ChunkMesher::buildNaiveMesh(ChunkGroup chunks) {
	std::vector<Vertex> mesh_vertices;
	std::vector<GLuint> mesh_indices;

	int chunk_size = Chunk::CHUNK_SIZE;

	for (int i = 0; i < (chunk_size * chunk_size * chunk_size); ++i) {
		int x = i % chunk_size;
		int y = (i / chunk_size) % chunk_size;
		int z = ((i / chunk_size) / chunk_size) % chunk_size;

		// get the id of the current block
		block_id_type current_block = chunks.blockAtLocalPos(x, y, z);

		// skip if air
		if (current_block == 0) {
			continue;
		}

		// iterate over each face
		Direction face_dir = Direction::Back;
		for (const auto & dir : directions) {
			block_id_type face_neighbor = chunks.blockAtLocalPos(x + dir.x, y + dir.y, z + dir.z); 

			if (face_neighbor == 0) {
				addBlockFace(x, y, z, face_dir, current_block, mesh_vertices, mesh_indices);
			}

			face_dir = static_cast<Direction>(static_cast<int>(face_dir) + 1);
		}
	}

	return { mesh_vertices, mesh_indices };
}

std::pair<std::vector<Vertex>, std::vector<GLuint>> ChunkMesher::buildGreedyMesh(ChunkGroup chunks) {
    std::vector<Vertex> mesh_vertices;
    std::vector<GLuint> mesh_indices;

    struct FaceDesc {
        Direction dir;
        int       d, u, v;
        int       normalSign;
    };

    const FaceDesc faces[6] = {
        { Direction::Back,   2, 0, 1, -1 },
        { Direction::Front,  2, 0, 1, +1 },
        { Direction::Left,   0, 2, 1, -1 },
        { Direction::Right,  0, 2, 1, +1 },
        { Direction::Bottom, 1, 0, 2, -1 },
        { Direction::Top,    1, 0, 2, +1 },
    };

    constexpr int N = Chunk::CHUNK_SIZE;
    constexpr int N2 = N * N;
    std::array<uint16_t, N2> mask;
    std::array<uint8_t, N2> visited;
    std::array<uint16_t, N2> sliceCurrent;
    std::array<uint16_t, N2> sliceNeighbor;

    for (const FaceDesc& fd : faces) {
        for (int depth = 0; depth < N; ++depth) {
            std::fill(visited.begin(), visited.end(), 0);

            // flatten current and neighbor slices into local arrays
            for (int j = 0; j < N; ++j) {
                for (int i = 0; i < N; ++i) {
                    int pos[3], npos[3];
                    pos[fd.d] = depth;
                    pos[fd.u] = i;
                    pos[fd.v] = j;

                    npos[fd.d] = depth + fd.normalSign;
                    npos[fd.u] = i;
                    npos[fd.v] = j;

                    sliceCurrent[j * N + i] = chunks.blockAtLocalPos(pos[0], pos[1], pos[2]);
                    sliceNeighbor[j * N + i] = chunks.blockAtLocalPos(npos[0], npos[1], npos[2]);
                }
            }

            // build mask from flattened slices
            for (int j = 0; j < N; ++j) {
                for (int i = 0; i < N; ++i) {
                    uint16_t cur = sliceCurrent[j * N + i];
                    uint16_t nbr = sliceNeighbor[j * N + i];
                    mask[j * N + i] = (cur != 0 && nbr == 0) ? cur : 0;
                }
            }

            // greedy merge 
            for (int j = 0; j < N; ++j) {
                for (int i = 0; i < N; ++i) {
                    if (visited[j * N + i]) continue;

                    uint16_t bid = mask[j * N + i];
                    if (bid == 0) continue;

                    // expand width along i
                    int width = 1;
                    while (i + width < N
                        && !visited[j * N + (i + width)]
                        && mask[j * N + (i + width)] == bid)
                    {
                        ++width;
                    }

                    // expand height along j
                    int height = 1;
                    while (j + height < N) {
                        bool rowOk = true;
                        for (int k = 0; k < width; ++k) {
                            int idx = (j + height) * N + (i + k);
                            if (visited[idx] || mask[idx] != bid) {
                                rowOk = false;
                                break;
                            }
                        }
                        if (!rowOk) break;
                        ++height;
                    }

                    // mark as consumed
                    for (int dj = 0; dj < height; ++dj)
                        for (int di = 0; di < width; ++di)
                            visited[(j + dj) * N + (i + di)] = 1;

                    // build quad
                    int corner[3];
                    corner[fd.d] = depth + (fd.normalSign == +1 ? 1 : 0);
                    corner[fd.u] = i;
                    corner[fd.v] = j;

                    glm::vec3 origin(corner[0], corner[1], corner[2]);
                    glm::vec3 uStep(0.0f), vStep(0.0f);
                    uStep[fd.u] = 1.0f;
                    vStep[fd.v] = 1.0f;

                    float uw = static_cast<float>(width);
                    float vh = static_cast<float>(height);

                    glm::vec3 verts[4];
                    switch (fd.dir) {
                    case Direction::Back:
                        verts[0] = origin;
                        verts[1] = origin + uw * uStep;
                        verts[2] = origin + uw * uStep + vh * vStep;
                        verts[3] = origin + vh * vStep;
                        break;
                    case Direction::Front:
                        verts[0] = origin + uw * uStep;
                        verts[1] = origin;
                        verts[2] = origin + vh * vStep;
                        verts[3] = origin + uw * uStep + vh * vStep;
                        break;
                    case Direction::Left:
                        verts[0] = origin + uw * uStep;
                        verts[1] = origin;
                        verts[2] = origin + vh * vStep;
                        verts[3] = origin + uw * uStep + vh * vStep;
                        break;
                    case Direction::Right:
                        verts[0] = origin;
                        verts[1] = origin + uw * uStep;
                        verts[2] = origin + uw * uStep + vh * vStep;
                        verts[3] = origin + vh * vStep;
                        break;
                    case Direction::Bottom:
                        verts[0] = origin + vh * vStep;
                        verts[1] = origin + uw * uStep + vh * vStep;
                        verts[2] = origin + uw * uStep;
                        verts[3] = origin;
                        break;
                    case Direction::Top:
                        verts[0] = origin;
                        verts[1] = origin + uw * uStep;
                        verts[2] = origin + uw * uStep + vh * vStep;
                        verts[3] = origin + vh * vStep;
                        break;
                    }

                    glm::vec2 uvs[4];

                    switch (fd.dir) {
                    case Direction::Back:
                    case Direction::Right:
                    case Direction::Top:
                        // verts: LL, LR, TR, TL
                        uvs[0] = { 0,  0 };
                        uvs[1] = { uw, 0 };
                        uvs[2] = { uw, vh };
                        uvs[3] = { 0,  vh };
                        break;
                    case Direction::Front:
                    case Direction::Left:
                        // verts are flipped on u-axis
                        uvs[0] = { uw, 0 };
                        uvs[1] = { 0,  0 };
                        uvs[2] = { 0,  vh };
                        uvs[3] = { uw, vh };
                        break;
                    case Direction::Bottom:
                        // verts: TL, TR, LR, LL (v is flipped)
                        uvs[0] = { 0,  vh };
                        uvs[1] = { uw, vh };
                        uvs[2] = { uw, 0 };
                        uvs[3] = { 0,  0 };
                        break;
                    }

                    GLuint offset = static_cast<GLuint>(mesh_vertices.size());
                    for (int idx : indices)
                        mesh_indices.push_back(idx + offset);

                    for (int k = 0; k < 4; ++k) {
                        Vertex vert(verts[k].x, verts[k].y, verts[k].z, static_cast<uint8_t>(fd.dir), bid);
                        mesh_vertices.push_back(vert);
                    }
                }
            }
        }
    }

    return { mesh_vertices, mesh_indices };
}

void ChunkMesher::addBlockFace(int x, int y, int z, Direction direction, block_id_type current_block, std::vector<Vertex> &mesh_vertices, std::vector<GLuint> &mesh_indices) const {
	const glm::ivec3* face = face_vertices[static_cast<size_t>(direction)];
	
	// add indices
	GLuint offset = static_cast<GLuint>(mesh_vertices.size());
	for (int i = 0; i < 6; ++i) {
		mesh_indices.push_back(indices[i] + offset);
	}

	// LL - LR - TR - TL vertex order
	for (int i = 0; i < 4; ++i) {
        float vx = face[i].x + x;
        float vy = face[i].y + y;
        float vz = face[i].z + z;
		Vertex vert(vx, vy, vz, static_cast<uint8_t>(direction), current_block);
      
		// add vertex to mesh
		mesh_vertices.push_back(vert);
	}
}

