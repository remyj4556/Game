# Strata: C++/OpenGL Voxel Engine
A blazingly fast multithreaded C++ voxel engine featuring dynamic chunk streaming, greedy chunk meshing, and a custom built thread pool enabling concurrent mesh creation and terrain generation.

<img width="800" height="421" alt="ezgif com-optimize" src="https://github.com/user-attachments/assets/49b3e8fd-a5f2-4d82-a536-0945905775ef" />

## Overview
This project is a from-scratch voxel terrain engine built around a concurrent chunk streaming pipeline. Chunks are generated using configurable noise parameters, meshed via a custom greedy meshing algorithm, and uploaded to the GPU asynchronously as the camera moves throughout the world. This allows for thousands of chunks to be loaded simultaneously while keeping a stable (and reasonable) frame rate.

## Performance
Measured with a 32^3 chunk size, single-octave heightmap noise, NVIDIA 3050 and AMD Ryzen 7 5800H.
- Concurrently loaded chunks: 6,000+
- Frame rate: stable 70 FPS
- Avg. mesh time / chunk: ~5.5ms
- Avg. generation time / chunk: ~0.1ms
- Avg. vertices / chunk ~150

## Features
- Custom thread pool: std::packaged_task/std::future based task queue built from scratch, used to parallelize chunk generation and meshing tasks across worker threads. This keeps the main thread available for OpenGL upload and render calls.
- Asynchronous chunk streaming architecture: designed a producer/consumer architecture for chunk "tasks", built around an explicit per-chunk state machine (unloaded -> queued_to_load -> loaded -> queued_to_mesh -> meshed -> queued_to_unload) which ensures chunk data correctness across multiple threads. 
- Thread-safe data structures: Designed thread-safe queue and hashmap implementations. Chunk data is stored as a shared_ptr, keyed by chunk coordinate in a hashmap. Producer/consumer queues are thread-safe and store lightweight coordinates rather than heavy chunk data. Chunk state is atomic, allowing quick reads in hot paths.
- Greedy meshing: Custom texture agnostic meshing algorithm merges adjacent block faces together, greatly reducing vertex count by over 75%. Block textures are accessed by their IDs directly in the fragment shader via an uploaded texture array, rather than passing texture UV's inside of vertex structs, which would reduce greedy meshing efficiency. 
- Bitpacked vertex format: Vertex attributes (local chunk coordinates, block ID, face texture IDs) are packed into a single lightweight uint32_t. This allows for an 80% reduction of GPU memory bandwidth from a naive approach consisting of a vector3 and two integers, storing the same data. 

## Building
### Dependencies
- C++20 or later
- OpenGL 3.3+
- -pthread compiler flag (depending on OS), for thread support

## Controls
- WASD: movement
- Mouse: camera look
- C: enable/disable cursor
- L: enable/disable dynamic handheld light
- Space/Shift: fly up/down
