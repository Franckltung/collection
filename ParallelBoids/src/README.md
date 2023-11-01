## Parallel Boids

A parallel boid simulator. Uses a vector based movement system. Developed using Intel Threading Blocks, Dear ImGui and SDL2.

### Requirements to run
- Compile

```
meson setup build
cd build
ninja
```

- Run

```
./ParallelBoids
```

To run parallel version:

```
./ParallelBoids --doParallel
```
