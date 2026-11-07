import pygame
from queue import PriorityQueue

# Uniform-Cost/Dijkstra (Graph Search), evaluation_value = path_cost
def search(redraw, grid, start, end, parent_dict):
    # count needed for 2nd-level prioritization (FIFO) of queue elements that 
    #   have the same evaluation value
    count = 0
    evaluation_values = {cell: float("inf") for row in grid for cell in row}
    evaluation_values[start] = 0
    # Priority Queue sorting next cells to be explored ascending by their 
    #   evaluation_values (first element of tuple), initialized with start cell.
    #   Note that evaluation_value = path_cost.
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
            if not explored_set.__contains__(neighbor):
                neighbor_temp_path_cost = evaluation_values[current] + 1
                # Note that path_costs dictionary is initialized with infinity 
                #   values.
                # No need to replace an element (tuple) in the frontier queue 
                #   that has the same cell but higher evaluation value. Can 
                #   just leave the element with higher evaluation value in the 
                #   queue, because the element with the same cell but lowest 
                #   evaluation value will be popped/explored first and 
                #   elements with same cell but higher evaluation values will 
                #   be ignored when popped because the explored_set will 
                #   already contain the cell.
                if neighbor_temp_path_cost < evaluation_values[neighbor]:
                    count += 1
                    parent_dict[neighbor] = current
                    evaluation_values[neighbor] = neighbor_temp_path_cost
                    frontier_queue.put((evaluation_values[neighbor], count, neighbor))
                    neighbor.make_open()  
        redraw()
        if current != start:
            current.make_closed()
    # no more cells in frontier, but haven't found the goal yet => goal can't 
    #   be found
    return False


