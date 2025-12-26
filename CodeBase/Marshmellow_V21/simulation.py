import pygame
import time
import random

# Constants
CELL_SIZE = 60
GRID_SIZE = 9
WINDOW_SIZE = CELL_SIZE * GRID_SIZE
WHITE = (255, 255, 255)
BLACK = (0, 0, 0)
RED = (255, 0, 0)
GREEN = (0, 255, 0)
BLUE = (0, 0, 255)
GRAY = (200, 200, 200)
LIGHT_GRAY = (240, 240, 240)
ROBOT_COLOR = (100, 100, 255)
UNKNOWN_COLOR = (50, 50, 50) # Color for undiscovered obstacles in visualization

# States
STATE_START = 0
STATE_NAVIGATE_TO_BOX = 1
STATE_SCAN_BOX = 2
STATE_PICKUP_BOX = 3
STATE_NAVIGATE_TO_SANCTUARY = 4
STATE_DROP_BOX = 5
STATE_NAVIGATE_TO_EXIT = 6
STATE_FINISH = 7

class Grid:
    def __init__(self):
        # The "Truth" - actual obstacles
        self.obstacles = [[False for _ in range(GRID_SIZE)] for _ in range(GRID_SIZE)]

    def set_obstacle(self, x, y):
        if 0 <= x < GRID_SIZE and 0 <= y < GRID_SIZE:
            self.obstacles[x][y] = True

    def is_obstacle(self, x, y):
        if 0 <= x < GRID_SIZE and 0 <= y < GRID_SIZE:
            return self.obstacles[x][y]
        return False # Out of bounds is not an obstacle for this check, handled by bounds check

class RobotMemory:
    def __init__(self):
        # What the robot knows
        self.known_obstacles = [[False for _ in range(GRID_SIZE)] for _ in range(GRID_SIZE)]
        self.scanned_empty = [[False for _ in range(GRID_SIZE)] for _ in range(GRID_SIZE)]

    def add_obstacle(self, x, y):
        if 0 <= x < GRID_SIZE and 0 <= y < GRID_SIZE:
            self.known_obstacles[x][y] = True

    def add_empty(self, x, y):
        if 0 <= x < GRID_SIZE and 0 <= y < GRID_SIZE:
            self.scanned_empty[x][y] = True

    def is_scanned_empty(self, x, y):
        if 0 <= x < GRID_SIZE and 0 <= y < GRID_SIZE:
            return self.scanned_empty[x][y]
        return False

    def is_valid(self, x, y):
        return 0 <= x < GRID_SIZE and 0 <= y < GRID_SIZE and not self.known_obstacles[x][y]

    def get_next_move_bfs(self, start, target):
        if start == target:
            return None
        queue = [(start, [start])]
        visited = set()
        visited.add(start)
        while queue:
            (current, path) = queue.pop(0)
            if current == target:
                if len(path) > 1:
                    return self._get_direction(start, path[1])
                return None
            for nx, ny in self._get_neighbors(current):
                if self.is_valid(nx, ny) and (nx, ny) not in visited:
                    visited.add((nx, ny))
                    queue.append(((nx, ny), path + [(nx, ny)]))
        return None

    def _get_neighbors(self, pos):
        x, y = pos
        return [(x, y + 1), (x + 1, y), (x, y - 1), (x - 1, y)]

    def _get_direction(self, start, next_step):
        if next_step[0] > start[0]: return 'E'
        if next_step[0] < start[0]: return 'W'
        if next_step[1] > start[1]: return 'N'
        if next_step[1] < start[1]: return 'S'
        return None

class Robot:
    def __init__(self, true_grid):
        self.true_grid = true_grid
        self.memory = RobotMemory()
        self.pos = (0, 0)
        self.heading = 'N'
        self.state = STATE_START
        self.boxes_collected = 0
        self.total_boxes = 2 # Updated to 2
        
        # Snake Pattern - Granular (Every Cell)
        # This ensures we don't bypass parts of a column if an obstacle is in the middle.
        self.waypoints = []
        for x in range(GRID_SIZE):
            if x % 2 == 0:
                # Even columns: Go Up (0 -> 8)
                for y in range(GRID_SIZE):
                    self.waypoints.append((x, y))
            else:
                # Odd columns: Go Down (8 -> 0)
                for y in range(GRID_SIZE - 1, -1, -1):
                    self.waypoints.append((x, y))
        
        # Remove the padding loop since we now have 64 points
        # while len(self.waypoints) < 32: self.waypoints.append((7,7))
        
        self.search_index = 0
        self.target = self.waypoints[0]
        
        # Randomize Real Boxes (2 Boxes, DIFFERENT Colors)
        self.real_boxes = []
        # Select 2 distinct colors from R, G, B
        self.target_colors = random.sample(['R', 'G', 'B'], 2)
        
        for col in self.target_colors:
            while True:
                rx = random.randint(0, GRID_SIZE - 1)
                ry = random.randint(0, GRID_SIZE - 1)
                if (rx, ry) == (0, 0): continue
                if self.true_grid.obstacles[rx][ry]: continue
                if any((bx, by) == (rx, ry) for bx, by, _ in self.real_boxes): continue
                
                self.real_boxes.append((rx, ry, col))
                print(f"Hidden Box {col} placed at ({rx}, {ry})")
                break

        self.carried_box = None
        self.collected_colors = []

    def _is_occupied(self, x, y):
        # Check wall
        if self.true_grid.is_obstacle(x, y):
            return True
        # Check boxes
        for bx, by, col in self.real_boxes:
            if (bx, by) == (x, y):
                return True
        return False

    def sense_obstacles(self):
        # Simulate sensors detecting obstacles in adjacent cells
        # Front, Left, Right
        x, y = self.pos
        # Define neighbors as (dx, dy)
        front = None
        sides = []
        
        if self.heading == 'N':
            front = (x, y+1)
            sides = [(x-1, y), (x+1, y)]
        elif self.heading == 'S':
            front = (x, y-1)
            sides = [(x-1, y), (x+1, y)]
        elif self.heading == 'E':
            front = (x+1, y)
            sides = [(x, y+1), (x, y-1)]
        elif self.heading == 'W':
            front = (x-1, y)
            sides = [(x, y-1), (x, y+1)]
        
        found_new = False
        
        # 1. Check Front (Updates Obstacles OR Empty)
        if front:
            nx, ny = front
            if 0 <= nx < GRID_SIZE and 0 <= ny < GRID_SIZE:
                if self.true_grid.is_obstacle(nx, ny):
                    if not self.memory.known_obstacles[nx][ny]:
                        print(f"Sensor detected obstacle detected FRONT at ({nx}, {ny})")
                        self.memory.add_obstacle(nx, ny)
                        found_new = True
                elif not self._is_occupied(nx, ny):
                    # Only mark empty if NO box and NO wall
                    self.memory.add_empty(nx, ny)

        # 2. Check Sides (Updates Empty ONLY)
        # User confirmed actual robot does not check sides and skip.
        # for nx, ny in sides:
        #     if 0 <= nx < GRID_SIZE and 0 <= ny < GRID_SIZE:
        #         if not self._is_occupied(nx, ny):
        #             self.memory.add_empty(nx, ny)
                # If obstacle/box is on the side, WE DO NOT MAP IT 
                # (matching C++ 'isObstacleLeft' which does not call setObstacle)

        return found_new

    def update(self):
        # 1. Sense Environment
        if self.sense_obstacles():
            # If we found a new obstacle, we might need to re-plan immediately
            # In this simple sim, move_to re-plans every step anyway.
            pass

        if self.state == STATE_START:
            print("State: START")
            self.state = STATE_NAVIGATE_TO_BOX
            
        elif self.state == STATE_NAVIGATE_TO_BOX:
            if self.pos == self.target:
                self.search_index += 1
                 # Skip obstacles AND scanned empty
                while self.search_index < len(self.waypoints):
                     tx, ty = self.waypoints[self.search_index]
                     if self.memory.known_obstacles[tx][ty] or self.memory.is_scanned_empty(tx, ty):
                         print(f"Skipping known/scanned waypoint ({tx}, {ty})")
                         self.search_index += 1
                     else:
                         break

                if self.search_index < len(self.waypoints):
                    self.target = self.waypoints[self.search_index]
                    print(f"Reached waypoint. Next: {self.target}")
                else:
                    self.state = STATE_NAVIGATE_TO_EXIT
            else:
                # Check if CURRENT target has become a known obstacle
                tx, ty = self.target
                if self.memory.known_obstacles[tx][ty]:
                    print(f"Target ({tx}, {ty}) discovered to be obstacle! Skipping...")
                    self.search_index += 1
                    # Find next valid waypoint
                    while self.search_index < len(self.waypoints):
                         ntx, nty = self.waypoints[self.search_index]
                         if self.memory.known_obstacles[ntx][nty] or self.memory.is_scanned_empty(ntx, nty):
                             print(f"Skipping known/scanned waypoint ({ntx}, {nty})")
                             self.search_index += 1
                         else:
                             break
                    
                    if self.search_index < len(self.waypoints):
                        self.target = self.waypoints[self.search_index]
                        print(f"New target: {self.target}")
                    else:
                        self.state = STATE_NAVIGATE_TO_EXIT
                    return

                # Check for box in FRONT
                x, y = self.pos
                front = None
                if self.heading == 'N': front = (x, y+1)
                elif self.heading == 'S': front = (x, y-1)
                elif self.heading == 'E': front = (x+1, y)
                elif self.heading == 'W': front = (x-1, y)

                if front:
                    fx, fy = front
                    for bx, by, col in self.real_boxes:
                        if (bx, by) == (fx, fy):
                             print(f"Box {col} detected FRONT at {front}!")
                             self.state = STATE_SCAN_BOX
                             self.box_in_front = (bx, by, col) # Store detected box
                             return

                self.move_to(self.target)

        elif self.state == STATE_SCAN_BOX:
            print("State: SCAN_BOX")
            if hasattr(self, 'box_in_front'):
                bx, by, col = self.box_in_front
                self.carried_box = col
                self.collected_colors.append(col)
                # Remove from real_boxes
                self.real_boxes = [b for b in self.real_boxes if (b[0], b[1]) != (bx, by)]
                self.state = STATE_PICKUP_BOX
                # Mark as empty in memory after pickup (it's gone now)
                self.memory.add_empty(bx, by)
                del self.box_in_front
                return

        elif self.state == STATE_PICKUP_BOX:
            print(f"State: PICKUP_BOX ({self.carried_box})")
            # Determine sanctuary (Green -> Green, Blue -> Blue, Red -> Red)
            if self.carried_box == 'G': self.target = (4, 8) # Green Sanct
            elif self.carried_box == 'B': self.target = (2, 8) # Blue Sanct
            elif self.carried_box == 'R': self.target = (6, 8) # Red Sanct
            print(f"Sanctuary Target: {self.target}")
            self.state = STATE_NAVIGATE_TO_SANCTUARY

        elif self.state == STATE_NAVIGATE_TO_SANCTUARY:
            if self.pos == self.target:
                self.state = STATE_DROP_BOX
            else:
                self.move_to(self.target)

        elif self.state == STATE_DROP_BOX:
            print("State: DROP_BOX")
            self.carried_box = None
            self.boxes_collected += 1
            if self.boxes_collected < self.total_boxes:
                # Ensure we don't target a known obstacle or scanned empty
                while self.search_index < len(self.waypoints):
                     tx, ty = self.waypoints[self.search_index]
                     if self.memory.known_obstacles[tx][ty] or self.memory.is_scanned_empty(tx, ty):
                         print(f"Skipping known/scanned waypoint ({tx}, {ty})")
                         self.search_index += 1
                     else:
                         break
                
                if self.search_index < len(self.waypoints):
                    self.target = self.waypoints[self.search_index]
                    self.state = STATE_NAVIGATE_TO_BOX
                    print(f"Resuming search at {self.target}")
                else:
                    self.target = (GRID_SIZE - 1, GRID_SIZE - 1)
                    self.state = STATE_NAVIGATE_TO_EXIT
                    print("No more waypoints. Heading to Exit.")
            else:
                self.target = (GRID_SIZE - 1, GRID_SIZE - 1)
                self.state = STATE_NAVIGATE_TO_EXIT
                print("All boxes collected. Heading to Exit.")

        elif self.state == STATE_NAVIGATE_TO_EXIT:
            if self.pos == self.target:
                self.state = STATE_FINISH
            else:
                self.move_to(self.target)
        
        elif self.state == STATE_FINISH:
            print("State: FINISH. Mission Complete.")
            # Logic to infer the MISSING color
            all_colors = {'R', 'G', 'B'}
            found_colors = set(self.collected_colors)
            missing = all_colors - found_colors
            
            if len(missing) == 1:
                print(f">>> OUTPUT: The missing color is {list(missing)[0]} <<<")
            else:
                print(f">>> OUTPUT: Unexpected result. Found: {self.collected_colors} <<<")
            return True
            
        return False

    def move_to(self, target):
        # Use Memory for pathfinding!
        move_dir = self.memory.get_next_move_bfs(self.pos, target)
        
        if move_dir:
            self.heading = move_dir
            # Check if move is valid in TRUE grid (did we crash?)
            nx, ny = self.pos
            if move_dir == 'N': ny += 1
            elif move_dir == 'S': ny -= 1
            elif move_dir == 'E': nx += 1
            elif move_dir == 'W': nx -= 1
            
            if self.true_grid.is_obstacle(nx, ny):
                print(f"CRASH! Hit unknown obstacle at ({nx}, {ny})!")
                self.memory.add_obstacle(nx, ny) # Learn from mistake
                # Stay in place
            else:
                self.pos = (nx, ny)
            
            time.sleep(0.05)

def main():
    pygame.init()
    screen = pygame.display.set_mode((WINDOW_SIZE, WINDOW_SIZE + 50))
    pygame.display.set_caption("Robot Logic Simulation - Unknown Environment")
    clock = pygame.time.Clock()

    true_grid = Grid()
    # Randomize 4 Obstacles
    obstacles_placed = 0
    forbidden_zones = [
        (0, 0), # Start
        (8, 8), # Exit
        (2, 8), # Blue Sanct
        (4, 8), # Green Sanct
        (6, 8)  # Red Sanct
    ]
    
    while obstacles_placed < 4:
        ox = random.randint(0, GRID_SIZE - 1)
        oy = random.randint(0, GRID_SIZE - 1)
        
        if (ox, oy) in forbidden_zones: continue
        
        if not true_grid.obstacles[ox][oy]:
            true_grid.set_obstacle(ox, oy)
            obstacles_placed += 1
            print(f"Hidden Obstacle placed at ({ox}, {oy})")

    robot = Robot(true_grid)

    running = True
    step = 0
    while running:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False

        finished = robot.update()
        if finished:
            print("Simulation finished successfully.")
            running = False
            time.sleep(2)

        # Drawing
        screen.fill(WHITE)
        
        # Draw Grid
        for x in range(GRID_SIZE):
            for y in range(GRID_SIZE):
                rect = pygame.Rect(x * CELL_SIZE, (GRID_SIZE - 1 - y) * CELL_SIZE, CELL_SIZE, CELL_SIZE)
                pygame.draw.rect(screen, GRAY, rect, 1)
                
                # Draw True Obstacles (Faintly/Debug)
                if true_grid.obstacles[x][y]:
                    pygame.draw.rect(screen, LIGHT_GRAY, rect) # Hidden ones are light gray
                
                # Draw Known Obstacles (Black)
                if robot.memory.known_obstacles[x][y]:
                    pygame.draw.rect(screen, BLACK, rect)

        # Draw Boxes
        for bx, by, col in robot.real_boxes:
            color = RED if col == 'R' else GREEN if col == 'G' else BLUE
            rect = pygame.Rect(bx * CELL_SIZE + 10, (GRID_SIZE - 1 - by) * CELL_SIZE + 10, CELL_SIZE - 20, CELL_SIZE - 20)
            pygame.draw.rect(screen, color, rect)

        # Draw Robot
        rx, ry = robot.pos
        rect = pygame.Rect(rx * CELL_SIZE + 5, (GRID_SIZE - 1 - ry) * CELL_SIZE + 5, CELL_SIZE - 10, CELL_SIZE - 10)
        pygame.draw.rect(screen, ROBOT_COLOR, rect)
        
        # Draw Info
        font = pygame.font.SysFont(None, 24)
        status_text = f"Boxes: {robot.boxes_collected}/2 | Pos: {robot.pos}"
        img = font.render(status_text, True, BLACK)
        screen.blit(img, (10, WINDOW_SIZE + 10))

        pygame.display.flip()
        
        step += 1
        if step > 3000:
            print("Simulation timed out.")
            running = False

    pygame.quit()

if __name__ == "__main__":
    main()
