## dansweeper-ml

beta state experimental minesweeper "gym" intended for testing various solving and generation implementations and a learning ground for machine learning. A fully playable parallel version can be found at [here](https://github.com/denialpan/dansweeper).

### features

![minedemov2](https://github.com/user-attachments/assets/df4595a0-c630-4190-8a13-af42d6a776fe)

texture credit: https://github.com/Minesweeper-World/MS-Texture

---

#### core game mechanics, generation and rendering

- deterministic board generation from width, height, number of mines, and prng.
- safe first click
- chording
- millisecond timer
- panning and pixel perfect zooming (no texture blurring)
- satisfying tile highlighting

#### solvers
- multithreaded away from rendering and game logic to prevent long step calculation
- pause/resume and advance by steps
- display solver stats and averages
- algorithmic
  - BFS
  - DFS (planned)
  - ...
- machine learning
  - reinforcement learning
  - ...
---
