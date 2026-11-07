import pygame
from queue import PriorityQueue

# heuristic for Greedy-Best-First
def h(p1, p2):
    r1, c1 = p1
    r2, c2 = p2
    return abs(r1 - r2) + abs(c1 - c2)

# A*Search (Graph Search), evaluation_value = heuristic_value
def search(redraw, grid, start, end, parent_dict):
    # count needed for 2nd-level prioritization (FIFO) of queue elements that 
    #   have the same evaluation value
    count = 0
    evaluation_values = {cell: float("inf") for row in grid for cell in row}
    evaluation_values[start] = h(start.get_pos(), end.get_pos())
    # Priority Queue sorting next cells to be explored ascending by their 
    #   evaluation_values (first element of tuple), initialized with start cell.
    #   Note that evaluation_value = heuristic value.
    frontier_queue = PriorityQueue()
    frontier_queue.put((evaluation_values[start], count, start))
    explored_set = set()
    
    while not frontier_queue.empty():
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                        
        current = frontier_queue.get()[2]
        
        # found the goal
        if current == end:
            return True
        
        explored_set.add(current)
        for neighbor in current.neighbors:
            # The evaluation value/heuristic value does not change for a cell.
            #   Thus, if an element/tuple in the queue already contains a cell, 
            #   it won't be replaced by another tuple with the same cell. 
            if not explored_set.__contains__(neighbor) and not neighbor in frontier_queue.queue:
                count += 1
                parent_dict[neighbor] = current
                evaluation_values[neighbor] = h(neighbor.get_pos(), end.get_pos())
                frontier_queue.put((evaluation_values[neighbor], count, neighbor))
                neighbor.make_open()  
                
        redraw()
        if current != start:
            current.make_closed()
    # no more cells in frontier, but haven't found the goal yet => goal can't 
    #   be found
    return False

