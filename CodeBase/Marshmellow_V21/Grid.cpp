#include "Grid.h"

Grid::Grid() { init(); }

void Grid::init() {
  for (int x = 0; x < GRID_SIZE; x++) {
    for (int y = 0; y < GRID_SIZE; y++) {
      obstacles[x][y] = false;
      scannedEmpty[x][y] = false;
    }
  }
}

void Grid::setScannedEmpty(int x, int y) {
  if (isValid(x, y)) {
    scannedEmpty[x][y] = true;
  }
}

bool Grid::isScannedEmpty(int x, int y) {
  if (!isValid(x, y))
    return false;
  return scannedEmpty[x][y];
}

void Grid::setObstacle(int x, int y) {
  if (isValid(x, y)) {
    obstacles[x][y] = true;
  }
}

bool Grid::isObstacle(int x, int y) {
  if (!isValid(x, y))
    return true; // Treat out of bounds as obstacle
  return obstacles[x][y];
}

bool Grid::isValid(int x, int y) {
  return x >= 0 && x < GRID_SIZE && y >= 0 && y < GRID_SIZE;
}

char Grid::getNextMoveBFS(Point start, Point target) {
  if (start == target)
    return 'X';

  // BFS Queue
  Point queue[GRID_SIZE * GRID_SIZE];
  int head = 0;
  int tail = 0;

  // Visited array and Parent map to reconstruct path
  bool visited[GRID_SIZE][GRID_SIZE];
  Point parents[GRID_SIZE][GRID_SIZE];

  for (int i = 0; i < GRID_SIZE; i++)
    for (int j = 0; j < GRID_SIZE; j++)
      visited[i][j] = false;

  // Start BFS
  queue[tail++] = start;
  visited[start.x][start.y] = true;
  parents[start.x][start.y] = {-1, -1};

  bool found = false;

  while (head < tail) {
    Point current = queue[head++];

    if (current == target) {
      found = true;
      break;
    }

    // Neighbors: N, E, S, W
    int dx[] = {0, 1, 0, -1};
    int dy[] = {1, 0, -1, 0};

    for (int i = 0; i < 4; i++) {
      int nx = current.x + dx[i];
      int ny = current.y + dy[i];

      if (isValid(nx, ny) && !obstacles[nx][ny] && !visited[nx][ny]) {
        visited[nx][ny] = true;
        parents[nx][ny] = current;
        queue[tail++] = {nx, ny};
      }
    }
  }

  if (!found)
    return 'X'; // No path

  // Backtrack to find the first move
  Point curr = target;
  while (parents[curr.x][curr.y] != start) {
    curr = parents[curr.x][curr.y];
    if (curr.x == -1)
      return 'X'; // Should not happen if found is true
  }

  // Determine direction from start to curr (the first step)
  if (curr.x > start.x)
    return 'E';
  if (curr.x < start.x)
    return 'W';
  if (curr.y > start.y)
    return 'N';
  if (curr.y < start.y)
    return 'S';

  return 'X';
}
