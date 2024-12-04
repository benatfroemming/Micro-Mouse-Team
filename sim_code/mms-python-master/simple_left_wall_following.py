import API
import sys

def log(string):
    sys.stderr.write("{}\n".format(string))
    sys.stderr.flush()

def main():
    # width = API.mazeWidth()
    # height = API.mazeHeight()
    p1 = 0
    p2 = 0
    heading = 'n'
    log("Running...")
    #API.setText(p1, p2, "abc")
    while True:
        show_walls(p1, p2, heading)
        if not API.wallLeft():
            API.turnLeft()
            heading = turn_left(heading)
        while API.wallFront():
            API.turnRight()
            heading = turn_right(heading)
        API.moveForward()
        p1, p2 = move_forward(p1, p2, heading)
        log((p1, p2, heading))

def turn_left(heading):
    order = ['n', 'w', 's', 'e']  
    return order[(order.index(heading) + 1) % 4]

def turn_right(heading):
    order = ['n', 'e', 's', 'w'] 
    return order[(order.index(heading) + 1) % 4]

def move_forward(p1, p2, heading):
    if heading == 'n':
        return p1, p2 + 1  
    elif heading == 'e':
        return p1 + 1, p2  
    elif heading == 's':
        return p1, p2 - 1 
    elif heading == 'w':
        return p1 - 1, p2  
    return p1, p2

def show_walls(p1, p2, heading):
    API.setColor(p1, p2, "G")
    if API.wallFront():
        # n,e,s,w
        API.setWall(p1, p2, heading)
    if API.wallLeft():
        h = turn_left(heading)
        API.setWall(p1, p2, h)
    if API.wallRight():
        h = turn_right(heading)
        API.setWall(p1, p2, h)    

if __name__ == "__main__":
    main()
