import pygame
from queue import Queue

# Breadth-first Search (Graph Search), frontier is a FIFO queue
def search(redraw, grid, start, end, parent_dict):
    frontier_queue = Queue()
    frontier_queue.put(start)
    explored_set = set()
    
    while not frontier_queue.empty():
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                        
        current = frontier_queue.get()
        
        explored_set.add(current)
        for neighbor in current.neighbors:
            if not explored_set.__contains__(neighbor) and not neighbor in frontier_queue.queue: 
                parent_dict[neighbor] = current
                if neighbor == end:
                    return True
                frontier_queue.put(neighbor)
                neighbor.make_open()  
        redraw()
        if current != start:
            current.make_closed()
    # no more cells in frontier, but haven't found the goal yet => goal can't 
    #   be found
    return False