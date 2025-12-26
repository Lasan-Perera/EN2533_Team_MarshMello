#ifndef GRID_H
#define GRID_H

#include <Arduino.h>

#define GRID_SIZE 9
#define TOTAL_CELLS (GRID_SIZE * GRID_SIZE)

struct Point {
  int x;
  int y;

  bool operator==(const Point &other) const {
    return x == other.x && y == other.y;
  }

  bool operator!=(const Point &other) const { return !(*this == other); }
};

class Grid {
public:
  Grid();
  void init();

  // Map Management
  void setObstacle(int x, int y);
  bool isObstacle(int x, int y);
  void setScannedEmpty(int x, int y);
  bool isScannedEmpty(int x, int y);
  bool isValid(int x, int y);

  // Pathfinding (BFS)
  // Pathfinding (BFS)
  // Returns next move direction: 'N', 'S', 'E', 'W', or 'X' (Stay/Error)
  char getNextMoveBFS(Point start, Point target);

private:
  bool obstacles[GRID_SIZE][GRID_SIZE];
  bool scannedEmpty[GRID_SIZE][GRID_SIZE];

  // Helper for BFS
  struct Node {
    Point p;
    Point parent;
  };
};

#endif // GRID_H
