import API
import sys
from collections import deque

class Cell:
    def __init__(self):
        self.distance = -1  
        self.walls = {'n': False, 'e': False, 's': False, 'w': False}

def log(string):
    sys.stderr.write("{}\n".format(string))
    sys.stderr.flush()

def main():
    log("Running...")
    targets = [(7, 7)]  # target cells
    p1, p2 = 0, 0
    width = int(API.mazeWidth())
    height = int(API.mazeHeight())
    maze = initialize_maze(width, height)  # Initialize maze
    heading = 'n'  # Initial heading
    visited = set()  # Set to track visited cells
    stack = [(p1, p2)]  # DFS stack

    # EXPLORATION RUN
    show_walls(p1, p2, heading, maze) 
    visited.add((p1, p2)) 
    while stack:
        options = [] 

        # Check movement options
        if not API.wallLeft():
            left_p1, left_p2 = move_forward(p1, p2, turn_left(heading))
            if (left_p1, left_p2) not in visited:
                options.append(('left', (left_p1, left_p2)))

        if not API.wallFront():
            front_p1, front_p2 = move_forward(p1, p2, heading)
            if (front_p1, front_p2) not in visited:
                options.append(('front', (front_p1, front_p2)))

        if not API.wallRight():
            right_p1, right_p2 = move_forward(p1, p2, turn_right(heading))
            if (right_p1, right_p2) not in visited:
                options.append(('right', (right_p1, right_p2)))

        if options:
            # Choose the first available direction
            direction, (next_p1, next_p2) = options[0]

            # Perform the action to move
            if direction == 'left':
                API.turnLeft()
                heading = turn_left(heading)
            elif direction == 'right':
                API.turnRight()
                heading = turn_right(heading)

            API.moveForward()
            p1, p2 = move_forward(p1, p2, heading)
            stack.append((p1, p2))  
            visited.add((p1, p2))

        else:
            # if no options go back in stack
            stack.pop()
            if stack:
                g1, g2 = stack[-1]
                d1, d2 = p1 - g1, p2 - g2  

                if d1 == 1 and d2 == 0:  
                    goal = 'w'
                elif d1 == -1 and d2 == 0: 
                    goal = 'e'
                elif d1 == 0 and d2 == 1:  
                    goal = 's'
                elif d1 == 0 and d2 == -1:  
                    goal = 'n'

                if turn_left(heading) == goal:
                    API.turnLeft()  
                    heading = turn_left(heading)
                elif turn_right(heading) == goal:
                    API.turnRight()  
                    heading = turn_right(heading)
                else:
                    while heading != goal:
                        API.turnLeft()  
                        heading = turn_left(heading)
                
                # Move backward to the previous cell)
                API.moveForward()
                p1, p2 = move_forward(p1, p2, heading)
        show_walls(p1, p2, heading, maze)

    # FAST RUN
    floodfill(maze, width, height, targets)  
    while API.wallFront():
        API.turnLeft()
        heading = turn_left(heading)
    
    color_path(p1, p2)

    while (p1, p2) not in targets:
        options = [] 

        # Check movement options
        if not API.wallLeft():
            left_p1, left_p2 = move_forward(p1, p2, turn_left(heading))
            options.append(('left', (left_p1, left_p2), maze[left_p1, left_p2].distance))

        if not API.wallFront():
            front_p1, front_p2 = move_forward(p1, p2, heading)
            options.append(('front', (front_p1, front_p2), maze[front_p1, front_p2].distance))

        if not API.wallRight():
            right_p1, right_p2 = move_forward(p1, p2, turn_right(heading))
            options.append(('right', (right_p1, right_p2), maze[right_p1, right_p2].distance))
        
        # Choose the first available direction
        options = sorted(options, key=lambda x: x[2])
        direction, _, _ = options[0]
        # Perform the action to move
        if direction == 'left':
            API.turnLeft()
            heading = turn_left(heading)
        elif direction == 'right':
            API.turnRight()
            heading = turn_right(heading)

        API.moveForward()
        p1, p2 = move_forward(p1, p2, heading)
        color_path(p1, p2)


def initialize_maze(width, height):
    maze = {(x, y): Cell() for x in range(width) for y in range(height)}
    return maze

def floodfill(maze, width, height, start_positions):
    log("Floodfill!")
    queue = deque([(x, y, 0) for x, y in start_positions])
    visited = set(start_positions)
    
    while queue:
        x, y, distance = queue.popleft()
        maze[(x, y)].distance = distance
        API.clearText(x, y)
        API.setText(x, y, str(distance))
        
        directions = [
            (x, y + 1, 'n'),
            (x, y - 1, 's'),
            (x + 1, y, 'e'),
            (x - 1, y, 'w')
        ]
        
        for nx, ny, wall in directions:
            if 0 <= nx < width and 0 <= ny < height and (nx, ny) not in visited:
                if not maze[(x, y)].walls[wall]:
                    queue.append((nx, ny, distance + 1))
                    visited.add((nx, ny))

def turn_left(heading):
    order = ['n', 'w', 's', 'e']
    return order[(order.index(heading) + 1) % 4]

def turn_right(heading):
    order = ['n', 'e', 's', 'w']
    return order[(order.index(heading) + 1) % 4]

def move_forward(x, y, heading):
    if heading == 'n':
        return x, y + 1
    elif heading == 'e':
        return x + 1, y
    elif heading == 's':
        return x, y - 1
    elif heading == 'w':
        return x - 1, y
    return x, y

def show_walls(x, y, heading, maze):
    API.setColor(x, y, "G")  
    directions = {
        'n': (0, 1, 's'), 
        'e': (1, 0, 'w'),  
        's': (0, -1, 'n'),
        'w': (-1, 0, 'e') 
    }
    
    # front wall
    if API.wallFront():
        API.setWall(x, y, heading)
        maze[(x, y)].walls[heading] = True
        # update adjacent cell as well
        dx, dy, opposite = directions[heading]
        adj_x, adj_y = x + dx, y + dy
        if (adj_x, adj_y) in maze:
            maze[(adj_x, adj_y)].walls[opposite] = True

    # left wall
    left_heading = turn_left(heading)
    if API.wallLeft():
        API.setWall(x, y, left_heading)
        maze[(x, y)].walls[left_heading] = True
        # update adjacent cell as well
        dx, dy, opposite = directions[left_heading]
        adj_x, adj_y = x + dx, y + dy
        if (adj_x, adj_y) in maze:
            maze[(adj_x, adj_y)].walls[opposite] = True

    # Handle right wall
    right_heading = turn_right(heading)
    if API.wallRight():
        API.setWall(x, y, right_heading)
        maze[(x, y)].walls[right_heading] = True
        # update adjacent cell as well
        dx, dy, opposite = directions[right_heading]
        adj_x, adj_y = x + dx, y + dy
        if (adj_x, adj_y) in maze:
            maze[(adj_x, adj_y)].walls[opposite] = True

def color_path(x, y):
    API.setColor(x, y, "B") 


if __name__ == "__main__":
    main()

