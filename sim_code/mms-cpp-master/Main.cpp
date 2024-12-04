#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <queue>
#include <unordered_set>
#include <map>
#include <set>
#include <algorithm>
#include "API.h"

using namespace std;

void log(const std::string& text) {
    std::cerr << text << std::endl;
}

struct Cell {
    int distance = -1;
    map<char, bool> walls = {{'n', false}, {'e', false}, {'s', false}, {'w', false}};
};

char turnLeft(char heading) {
    string order = "nwse";
    return order[(order.find(heading) + 1) % 4];
}

char turnRight(char heading) {
    string order = "nesw";
    return order[(order.find(heading) + 1) % 4];
}

pair<int, int> moveForward(int x, int y, char heading) {
    if (heading == 'n') return {x, y + 1};
    if (heading == 'e') return {x + 1, y};
    if (heading == 's') return {x, y - 1};
    if (heading == 'w') return {x - 1, y};
    return {x, y};
}

map<pair<int, int>, Cell> initializeMaze(int width, int height) {
    map<pair<int, int>, Cell> maze;  
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            maze[{x, y}] = Cell();  
        }
    }
    return maze;  
}

void floodfill(map<pair<int, int>, Cell>& maze, int width, int height, pair<int, int> start) {
    log("Floodfill!");
    // Queue for BFS
    queue<tuple<int, int, int>> queue;
    queue.push({start.first, start.second, 0}); 
    set<pair<int, int>> visited;
    visited.insert(start);

    vector<tuple<int, int, char>> directions = {
        {0, 1, 'n'},   
        {0, -1, 's'},  
        {1, 0, 'e'},   
        {-1, 0, 'w'}   
    };

    while (!queue.empty()) {
        int x, y, distance;
        tie(x, y, distance) = queue.front();
        queue.pop();
        maze[{x, y}].distance = distance;
        API::clearText(x, y);  
        API::setText(x, y, to_string(distance)); 

        // Check all 4 possible directions
        for (const auto& direction : directions) {
            int dx = std::get<0>(direction);
            int dy = std::get<1>(direction);
            char wall = std::get<2>(direction);
            int nx = x + dx, ny = y + dy;

            // Check if the new position is within bounds and not visited
            if (nx >= 0 && nx < width && ny >= 0 && ny < height && visited.find({nx, ny}) == visited.end()) {
                // Check if there is no wall in the current direction
                if (!maze[{x, y}].walls[wall]) {
                    queue.push({nx, ny, distance + 1});
                    visited.insert({nx, ny});
                }
            }
        }
    }
}

void colorPath(int x, int y){
    API::setColor(x, y, 'B');
}

void showWalls(int x, int y, char heading, map<pair<int, int>, Cell>& maze) {
    API::setColor(x, y, 'G');

    // Directions map
    map<char, tuple<int, int, char>> directions = {
        {'n', {0, 1, 's'}}, 
        {'e', {1, 0, 'w'}}, 
        {'s', {0, -1, 'n'}}, 
        {'w', {-1, 0, 'e'}}
    };

    // Front wall
    if (API::wallFront()) {
        API::setWall(x, y, heading);
        maze[{x, y}].walls[heading] = true;
        // Update adjacent cells wall status
        int dx, dy;
        char opposite;
        tie(dx, dy, opposite) = directions[heading];
        int adj_x = x + dx, adj_y = y + dy;
        if (maze.find({adj_x, adj_y}) != maze.end()) {  
            maze[{adj_x, adj_y}].walls[opposite] = true;
        }
    }

    // Left wall
    char left_heading = turnLeft(heading);
    if (API::wallLeft()) {
        API::setWall(x, y, left_heading);
        maze[{x, y}].walls[left_heading] = true;
        // Update adjacent cells wall status
        int dx, dy;
        char opposite;
        tie(dx, dy, opposite) = directions[left_heading]; 
        int adj_x = x + dx, adj_y = y + dy;
        if (maze.find({adj_x, adj_y}) != maze.end()) {  
            maze[{adj_x, adj_y}].walls[opposite] = true;
        }
    }

    // Right wall
    char right_heading = turnRight(heading);
    if (API::wallRight()) {
        API::setWall(x, y, right_heading);
        maze[{x, y}].walls[right_heading] = true;
        // Update adjacent cells wall status
        int dx, dy;
        char opposite;
        tie(dx, dy, opposite) = directions[right_heading]; 
        int adj_x = x + dx, adj_y = y + dy;
        if (maze.find({adj_x, adj_y}) != maze.end()) {  
            maze[{adj_x, adj_y}].walls[opposite] = true;
        }
    }
}

int main(int argc, char* argv[]) {
    log("Running...");

    // Initialization
    pair<int, int> target = {7, 7};  // Target cells
    int p1 = 0, p2 = 0;
    int width = API::mazeWidth();
    int height = API::mazeHeight();
    map<pair<int, int>, Cell> maze = initializeMaze(width, height);
    char heading = 'n';
    set<pair<int, int>> visited;
    stack<pair<int, int>> dfsStack;
    dfsStack.push({p1, p2});

    // EXPLORATION PHASE
    showWalls(p1, p2, heading, maze);
    visited.insert({p1, p2});

    while (!dfsStack.empty()) {
        vector<pair<string, pair<int, int>>> options;

        // Check movement options non-visited cells
        // left
        if (!API::wallLeft()) {
            int leftX, leftY;
            tie(leftX, leftY) = moveForward(p1, p2, turnLeft(heading));
            if (visited.find({leftX, leftY}) == visited.end()) {
                options.push_back({"left", {leftX, leftY}});
            }
        }
        // front
        if (!API::wallFront()) {
            int frontX, frontY;
            tie(frontX, frontY) = moveForward(p1, p2, heading);
            if (visited.find({frontX, frontY}) == visited.end()) {
                options.push_back({"front", {frontX, frontY}});
            }
        }
        // right
        if (!API::wallRight()) {
            int rightX, rightY;
            tie(rightX, rightY) = moveForward(p1, p2, turnRight(heading));
            if (visited.find({rightX, rightY}) == visited.end()) {
                options.push_back({"right", {rightX, rightY}});
            }
        }

        if (!options.empty()) {
            // Choose the first available direction
            string direction;
            pair<int, int> nextCell;
            tie(direction, nextCell) = options.front();

            // Perform the action to move
            if (direction == "left") {
                API::turnLeft();
                heading = turnLeft(heading);
            } else if (direction == "right") {
                API::turnRight();
                heading = turnRight(heading);
            }

            API::moveForward();
            p1 = nextCell.first;
            p2 = nextCell.second;
            dfsStack.push(nextCell);
            visited.insert(nextCell);
        } else {
            // If no options, backtrack in stack
            dfsStack.pop();
            if (!dfsStack.empty()) {
                pair<int, int> prevCell = dfsStack.top();
                int dx = p1 - prevCell.first, dy = p2 - prevCell.second;
                char goal;

                if (dx == 1 && dy == 0) goal = 'w';
                else if (dx == -1 && dy == 0) goal = 'e';
                else if (dx == 0 && dy == 1) goal = 's';
                else if (dx == 0 && dy == -1) goal = 'n';

                while (heading != goal) {
                    API::turnLeft();
                    heading = turnLeft(heading);
                }

                API::moveForward();
                p1 = prevCell.first;
                p2 = prevCell.second;
            }
        }
        showWalls(p1, p2, heading, maze);
    }

    // FAST RUN
    // compute shortest distance from each cell to target 
    floodfill(maze, width, height, target);

    // turn 180 to start fast run
    while (API::wallFront()) {
        API::turnLeft();
        heading = turnLeft(heading);
    }
    colorPath(p1, p2);

    // choose neighboor with smallest distance to target until reaching the goal
    while (p1 != target.first || p2 != target.second) {
        vector<tuple<string, pair<int, int>, int>> options;
        // Check movement options
        if (!API::wallLeft()) {
            int leftX, leftY;
            tie(leftX, leftY) = moveForward(p1, p2, turnLeft(heading));
            options.push_back({"left", {leftX, leftY}, maze[{leftX, leftY}].distance});
        }

        if (!API::wallFront()) {
            int frontX, frontY;
            tie(frontX, frontY) = moveForward(p1, p2, heading);
            options.push_back({"front", {frontX, frontY}, maze[{frontX, frontY}].distance});
        }

        if (!API::wallRight()) {
            int rightX, rightY;
            tie(rightX, rightY) = moveForward(p1, p2, turnRight(heading));
            options.push_back({"right", {rightX, rightY}, maze[{rightX, rightY}].distance});
        }

        // Choose the best option based on distance
        sort(options.begin(), options.end(), [](auto& a, auto& b) {
            return get<2>(a) < get<2>(b);
        });
        string direction;
        pair<int, int> nextCell;
        int d;  
        tie(direction, nextCell, d) = options.front();

        if (direction == "left") {
            API::turnLeft();
            heading = turnLeft(heading);
        } else if (direction == "right") {
            API::turnRight();
            heading = turnRight(heading);
        }

        API::moveForward();
        p1 = nextCell.first;
        p2 = nextCell.second;
        colorPath(p1, p2);
    }
    log("done");
}