import pygame 
from sys import exit

import astar_graphsearch
import breadth_first_graphsearch
import depth_first_graphsearch
import greedy_best_first_graphsearch
import uniform_cost_graphsearch

# initialize window and (default) parameters
pygame.font.init()
WIDTH = 1500
ROWS = 50
CELL_WIDTH = WIDTH // ROWS
WINDOW = pygame.display.set_mode(size=(WIDTH + 300, 900))
pygame.display.set_caption('Pathfinding-Algorithm Visualizer')
is_dark_mode = False
current_cell_type = None
previously_clicked_cell = None
algorithms = ['A*-Search', 'Breadth-First-Search', 'Depth-First-Search', \
               'Greedy-Best-First', 'Uniform-Cost/Dijkstra']
current_algorithm_index = 0
current_algorithm = algorithms[current_algorithm_index]
solution_path_length_count = 0

# colours (for bright mode)
BRIGHT_BLUE = (51, 153, 255)
VERY_BRIGHT_BLUE = (153, 204, 255)
VERY_BRIGHT_GREY = (225, 225, 224)
BLUE = (102, 178, 255)
BLACK = (0, 0, 0)
WHITE = (251, 252, 252)
PURPLE = (153, 51, 255)
DARK_BLUE = (0, 0, 255)
YELLOW = (237, 255, 0)
STRONG_YELLOW = (255, 255, 0)
MID_BLUE = (52, 152, 219)
BLUE_B = (51, 51, 255)
DARK_BLUE_B = (0, 0, 102)
STRONG_TURQUOISE = (0, 255, 255)

# additional colours (for dark mode)
RED = (255, 0, 0)
DARK_NAVY = (21, 32, 43)
NAVY = (25, 39, 52)
PINK = (255, 51, 255)
GREEN = (0, 255, 0)#
GREY_A = (136, 153, 166)
GREY_B = (128, 128, 128)
LIGHT_GREY = (160, 160, 160)
DARK_TURQUOISE = (0, 204, 204)
TURQUOISE = (51, 255, 255)

# default colour settings (bright mode) 
START_COLOUR = DARK_BLUE
END_COLOUR = RED  
EXPLORED_COLOUR = BRIGHT_BLUE
EXPLORING_COLOUR = DARK_BLUE_B
SOLUTION_PATH_COLOUR = STRONG_YELLOW
BACKGROUND_COLOUR = WHITE
BARRIER_COLOUR = VERY_BRIGHT_BLUE
GRID_LINES_COLOUR = VERY_BRIGHT_BLUE
START_END_BARRIER_BUTTON_FONT_COLOUR = MID_BLUE
START_END_BARRIER_BUTTON_FONT_HOVER_COLOUR = WHITE
OTHER_BUTTON_COLOUR = WHITE
OTHER_BUTTON_OUTLINE_COLOUR = MID_BLUE
OTHER_BUTTON_HOVER_COLOUR = MID_BLUE
OTHER_BUTTON_FONT_COLOUR = MID_BLUE
OTHER_BUTTON_FONT_HOVER_COLOUR = WHITE

# Dictionary for parent of each cell. Will be filled by specific 
#   search algorithm.
parent_dict = {}

class Button():
    font = pygame.font.SysFont('arial', 25)
    
    def __init__(self, colour_background, colour_outline, colour_font, \
        colour_hover, colour_font_hover, colour_clicked, xPos, yPos, width, \
        height, text=''):
        
        self.colour_background = colour_background
        self.colour_unhover = colour_background
        self.colour_outline = colour_outline
        self.colour_font = colour_font
        self.colour_font_unhover = colour_font
        self.colour_hover = colour_hover
        self.colour_font_hover = colour_font_hover
        self.colour_clicked = colour_clicked
        self.xPos = xPos
        self.yPos = yPos
        # The actual width and height will be larger due to the outline, 
        #   see draw() function below.
        self.width = width
        self.height = height
        self.text = text
        self.isHover = False
        self.isClicked = False
    
    def is_clicked(self):
        return self.isClicked
    
    def get_yPosEnd(self):
        return self.yPos + self.height
    
    def set_colour_background(self, colour):
        self.colour_background = colour
    
    def set_colour_outline(self, colour):
        self.colour_outline = colour
    
    def set_colour_hover(self, colour):
        self.colour_hover = colour
        
    def set_colour_unhover(self, colour):
        self.colour_unhover = colour
        
    def set_colour_clicked(self, colour):
        self.colour_clicked = colour
    
    def set_colour_font(self, colour):
        self.colour_font = colour
        
    def set_colour_font_hover(self, colour):
        self.colour_font_hover = colour
        
    def set_colour_font_unhover(self, colour):
        self.colour_font_unhover = colour
    
    def set_text(self, text):
        self.text = text
    
    def draw(self):
        # filling the button with the outline colour first
        if self.colour_outline != self.colour_background:
            pygame.draw.rect(WINDOW, self.colour_outline, (self.xPos-2, \
                                self.yPos-2, self.width+4, self.height+4), 0)
        else:
            pygame.draw.rect(WINDOW, self.colour_background, (self.xPos-2, \
                                self.yPos-2, self.width+4, self.height+4), 0)    
        # filling the button and leaving the outline colours at the borders
        pygame.draw.rect(WINDOW, self.colour_background, (self.xPos, \
                                      self.yPos, self.width, self.height), 0)
        #rendering the button's text    
        if self.text != '':
            text = Button.font.render(self.text, True, self.colour_font)
            WINDOW.blit(text, (self.xPos + (self.width/2 - text.get_width()/2),\
                               self.yPos + (self.height/2 - text.get_height()/2)))
    
    # pos is a tuple of x and y coordinates indicating the position of the mouse
    def mouse_is_over(self, pos):
        if pos[0] > self.xPos and pos[0] < self.xPos + self.width and \
            pos[1] > self.yPos and pos[1] < self.yPos + self.height:
                return True
        return False
    
    def hover(self):     
        self.colour_background = self.colour_hover
        self.colour_font = self.colour_font_hover   
        self.isHover = True
    
    def unhover(self):
        self.colour_background = self.colour_unhover
        self.colour_font = self.colour_font_unhover
        self.isHover = False
    
    def click(self):
        self.colour_background = self.colour_clicked
        self.colour_font = self.colour_font_hover 
        self.isClicked = True
        
    def unclick(self):
        self.unhover()
        self.isClicked = False

class Cell:
    # quadratic cells, i.e., width = height
    # qaudratic window, i.e., rows = columns
    def __init__(self, row, col, width, total_rows):
        self.row = row
        self.col = col
        self.y = row * width 
        self.x = col * width
        self.colour = BACKGROUND_COLOUR
        self.neighbors = []
        self.width = width
        self.total_rows = total_rows 
        self.default = True
        self.start = False
        self.end = False
        self.barrier = False
        self.closed = False
        self.open = False
        self.part_of_solution = False
        
    def get_pos(self):
        return self.row, self.col

    def is_default(self):
        return self.default
    
    # already visited/explored
    def is_closed(self):
        	return self.closed

    # currently being visited/explored
    def is_open(self):
        	return self.open

    def is_barrier(self):
        return self.barrier

    def is_start(self):
        	return self.start

    def is_end(self):
        	return self.end
        
    def is_part_of_solution(self):
        return self.part_of_solution

    def reset(self):
        if not self.is_default():
            self.default = True
            self.start = False
            self.end = False
            self.barrier = False
            self.closed = False
            self.open = False
            self.part_of_solution = False            
        # outside of the if block for changing the colour mode
        self.colour = BACKGROUND_COLOUR

    def prepare_cell_change(self):
        self.reset()
        self.default = False

    def make_start(self):
        if not self.is_start():
            self.prepare_cell_change()
            self.start = True
        # outside of the if block for changing the colour mode
        self.colour = START_COLOUR

    # already visited/explored
    def make_closed(self):
        if not self.is_closed():
            self.prepare_cell_change()
            self.closed = True
        # outside of the if block for changing the colour mode
        self.colour = EXPLORED_COLOUR

    # currently being visited/explored
    def make_open(self):
        if not self.is_open():
            self.prepare_cell_change()
            self.open = True
        # outside of the if block for changing the colour mode
        self.colour = EXPLORING_COLOUR

    def make_barrier(self):
        if not self.is_barrier():
            self.prepare_cell_change()
            self.barrier = True
        # outside of the if block for changing the colour mode
        self.colour = BARRIER_COLOUR

    def make_end(self):
        if not self.is_end():
            self.prepare_cell_change()
            self.end = True
        # outside of the if block for changing the colour mode
        self.colour = END_COLOUR

    # solution path
    def make_part_of_path(self):
        if not self.is_part_of_solution():
            self.prepare_cell_change()
            self.part_of_solution = True
        # outside of the if block for changing the colour mode
        self.colour = SOLUTION_PATH_COLOUR
            
    def paint(self, win):
        pygame.draw.rect(win, self.colour, (self.x, self.y, self.width, self.width))
       
    # The order in which neighbour cells are put into a cell's neighbour list
    #   here determines the order in which neighbouring cells will be explored.
    #   Here for example, first the lower neighbour will be explored
    #   (if possible), then the upper neighbour, then the right, then the left.
    #   The opposite order will hold in Dept-First-Search due to the LIFO 
    #   Queue applied in Depth-First-Search.
    def set_neighbors(self, grid):
        self.neighbors = []
        # neighbour below
        if self.row < self.total_rows - 1 and not grid[self.row + 1][self.col].is_barrier(): 
        		self.neighbors.append(grid[self.row + 1][self.col])
        # neighbour above        
        if self.row > 0 and not grid[self.row - 1][self.col].is_barrier(): 
        		self.neighbors.append(grid[self.row - 1][self.col])
        # neighbour on the right        
        if self.col < self.total_rows - 1 and not grid[self.row][self.col + 1].is_barrier(): 
        		self.neighbors.append(grid[self.row][self.col + 1])
        # neighbour on the left        
        if self.col > 0 and not grid[self.row][self.col - 1].is_barrier(): 
        		self.neighbors.append(grid[self.row][self.col - 1])

# buttons
colour_mode_button = Button(OTHER_BUTTON_COLOUR, OTHER_BUTTON_OUTLINE_COLOUR, \
    OTHER_BUTTON_FONT_COLOUR, OTHER_BUTTON_HOVER_COLOUR, OTHER_BUTTON_FONT_HOVER_COLOUR, \
    OTHER_BUTTON_COLOUR, WIDTH+50, 50, 200, 50, text = 'Dark mode')

start_cell_button = Button(BACKGROUND_COLOUR, START_COLOUR, START_END_BARRIER_BUTTON_FONT_COLOUR, \
    START_COLOUR, START_END_BARRIER_BUTTON_FONT_HOVER_COLOUR, START_COLOUR, \
    WIDTH+50, colour_mode_button.get_yPosEnd() + 60, 200, 50, text = 'Choose start cell')
    
end_cell_button = Button(BACKGROUND_COLOUR, END_COLOUR, START_END_BARRIER_BUTTON_FONT_COLOUR, \
    END_COLOUR, START_END_BARRIER_BUTTON_FONT_HOVER_COLOUR, END_COLOUR, \
    WIDTH+50, start_cell_button.get_yPosEnd() + 30, 200, 50, text = 'Choose end cell') 
    
barrier_cell_button = Button(BACKGROUND_COLOUR, BARRIER_COLOUR, START_END_BARRIER_BUTTON_FONT_COLOUR, \
    BARRIER_COLOUR, START_END_BARRIER_BUTTON_FONT_HOVER_COLOUR, BARRIER_COLOUR, \
    WIDTH+50, end_cell_button.get_yPosEnd() + 30, 200, 50, text = 'Choose barrier cells')
    
previous_algorithm_button = Button(OTHER_BUTTON_COLOUR, OTHER_BUTTON_OUTLINE_COLOUR, \
    OTHER_BUTTON_FONT_COLOUR, OTHER_BUTTON_HOVER_COLOUR, OTHER_BUTTON_FONT_HOVER_COLOUR, \
    OTHER_BUTTON_COLOUR, WIDTH+20, barrier_cell_button.get_yPosEnd() + 80, 20, \
    50, text = '<')
    
next_algorithm_button = Button(OTHER_BUTTON_COLOUR, OTHER_BUTTON_OUTLINE_COLOUR, \
    OTHER_BUTTON_FONT_COLOUR, OTHER_BUTTON_HOVER_COLOUR, OTHER_BUTTON_FONT_HOVER_COLOUR, \
    OTHER_BUTTON_COLOUR, WIDTH+260, barrier_cell_button.get_yPosEnd() + 80, 20, \
    50, text = '>')
    
# using a button that does not react to anything as label to display the 
#   current algorithm 
current_algorithm_label = Button(OTHER_BUTTON_COLOUR, OTHER_BUTTON_COLOUR, \
    OTHER_BUTTON_FONT_COLOUR, OTHER_BUTTON_COLOUR, OTHER_BUTTON_FONT_COLOUR, \
    OTHER_BUTTON_COLOUR, WIDTH+50, \
    barrier_cell_button.get_yPosEnd() + 80, 200, 50, text = current_algorithm)   
    
# using a button that does not react to anything as label to display the 
#   solution text. 
solution_label = Button(OTHER_BUTTON_COLOUR, OTHER_BUTTON_COLOUR, \
    OTHER_BUTTON_FONT_COLOUR, OTHER_BUTTON_COLOUR, OTHER_BUTTON_FONT_COLOUR, \
    OTHER_BUTTON_COLOUR, WIDTH+50, \
    current_algorithm_label.get_yPosEnd() + 30, 200, 50, text = '')

play_button = Button(OTHER_BUTTON_COLOUR, OTHER_BUTTON_OUTLINE_COLOUR, \
    OTHER_BUTTON_FONT_COLOUR, OTHER_BUTTON_HOVER_COLOUR, OTHER_BUTTON_FONT_HOVER_COLOUR, \
    OTHER_BUTTON_COLOUR, WIDTH+50, solution_label.get_yPosEnd() + 30, \
    200, 50, text = 'Play')
    
reset_button = Button(OTHER_BUTTON_COLOUR, OTHER_BUTTON_OUTLINE_COLOUR, \
    OTHER_BUTTON_FONT_COLOUR, OTHER_BUTTON_HOVER_COLOUR, OTHER_BUTTON_FONT_HOVER_COLOUR, \
    OTHER_BUTTON_COLOUR, WIDTH+50, play_button.get_yPosEnd() + 30, 200, 50, \
    text = 'Reset cells')
   
buttons = [colour_mode_button, start_cell_button, end_cell_button, \
    barrier_cell_button, previous_algorithm_button, next_algorithm_button, \
    current_algorithm_label, solution_label, play_button, reset_button]
    
# currently_clicked_button is the button that was previously cliked and should
#   remain clicked (only for start, end and barrier button)
currently_clicked_button = None
currently_hovered_button = None
current_hover = False
# auxiliary variable for differentiating between mouse being hold and simple click
mouse_hold = False
    

# set bright colour mode
def set_bright_mode(grid):
    global START_COLOUR, END_COLOUR, EXPLORED_COLOUR, EXPLORING_COLOUR, \
        SOLUTION_PATH_COLOUR, BACKGROUND_COLOUR, BARRIER_COLOUR, \
        GRID_LINES_COLOUR, START_END_BARRIER_BUTTON_FONT_COLOUR, \
        START_END_BARRIER_BUTTON_FONT_HOVER_COLOUR, OTHER_BUTTON_COLOUR, \
        OTHER_BUTTON_OUTLINE_COLOUR, OTHER_BUTTON_HOVER_COLOUR, \
        OTHER_BUTTON_FONT_COLOUR, OTHER_BUTTON_FONT_HOVER_COLOUR, is_dark_mode
        
    is_dark_mode = False
    
    START_COLOUR = DARK_BLUE
    END_COLOUR = RED 
    EXPLORED_COLOUR = BRIGHT_BLUE
    EXPLORING_COLOUR = DARK_BLUE_B
    SOLUTION_PATH_COLOUR = STRONG_YELLOW
    BACKGROUND_COLOUR = WHITE
    BARRIER_COLOUR = VERY_BRIGHT_BLUE
    GRID_LINES_COLOUR = VERY_BRIGHT_BLUE
    START_END_BARRIER_BUTTON_FONT_COLOUR = MID_BLUE
    START_END_BARRIER_BUTTON_FONT_HOVER_COLOUR = WHITE
    OTHER_BUTTON_COLOUR = WHITE
    OTHER_BUTTON_OUTLINE_COLOUR = MID_BLUE
    OTHER_BUTTON_HOVER_COLOUR = MID_BLUE
    OTHER_BUTTON_FONT_COLOUR = MID_BLUE
    OTHER_BUTTON_FONT_HOVER_COLOUR = WHITE
    
    apply_colour_mode_to_cells(grid)
    apply_colour_mode_to_buttons()
        
# set dark colour mode
def set_dark_mode(grid):
    global START_COLOUR, END_COLOUR, EXPLORED_COLOUR, EXPLORING_COLOUR, \
        SOLUTION_PATH_COLOUR, BACKGROUND_COLOUR, BARRIER_COLOUR, \
        GRID_LINES_COLOUR, START_END_BARRIER_BUTTON_FONT_COLOUR, \
        START_END_BARRIER_BUTTON_FONT_HOVER_COLOUR, OTHER_BUTTON_COLOUR, \
        OTHER_BUTTON_OUTLINE_COLOUR, OTHER_BUTTON_HOVER_COLOUR, \
        OTHER_BUTTON_FONT_COLOUR, OTHER_BUTTON_FONT_HOVER_COLOUR, is_dark_mode
            
    is_dark_mode = True
    
    START_COLOUR = BLUE_B
    END_COLOUR = RED  
    EXPLORED_COLOUR = DARK_TURQUOISE
    EXPLORING_COLOUR = TURQUOISE
    SOLUTION_PATH_COLOUR = YELLOW
    BACKGROUND_COLOUR = DARK_NAVY
    BARRIER_COLOUR = GREY_B
    GRID_LINES_COLOUR = GREY_B
    START_END_BARRIER_BUTTON_FONT_COLOUR = WHITE
    START_END_BARRIER_BUTTON_FONT_HOVER_COLOUR = WHITE
    OTHER_BUTTON_COLOUR = DARK_NAVY
    OTHER_BUTTON_OUTLINE_COLOUR = WHITE
    OTHER_BUTTON_HOVER_COLOUR = BLACK
    OTHER_BUTTON_FONT_COLOUR = WHITE
    OTHER_BUTTON_FONT_HOVER_COLOUR = WHITE
    
    apply_colour_mode_to_cells(grid)
    apply_colour_mode_to_buttons()
    
def apply_colour_mode_to_cells(grid):
    for row in grid:
        for cell in row:
            # reset() or make...() only repaints the cell if the cell is
            #   already the corresponding type of cell.
            if cell.is_default():
                cell.reset()
            if cell.is_start():
                cell.make_start()
            elif cell.is_end():
                cell.make_end()
            elif cell.is_barrier():
                cell.make_barrier()
            elif cell.is_open():
                cell.make_open()
            elif cell.is_closed():
                cell.make_closed()
            elif cell.is_part_of_solution():
                cell.make_part_of_path()

# Resetting buttons' colours with colours from new colour mode. Could also  
#   implement a function in the Button class that does this reset and then
#   just call this function on each button in the following function here.
def apply_colour_mode_to_buttons():
    for button in buttons:
        # normal/other buttons
        if button == start_cell_button or button == end_cell_button or\
            button == barrier_cell_button:
            button.set_colour_background(BACKGROUND_COLOUR)
            button.set_colour_unhover(BACKGROUND_COLOUR)
            button.set_colour_font(START_END_BARRIER_BUTTON_FONT_COLOUR)
            button.set_colour_font_hover(START_END_BARRIER_BUTTON_FONT_HOVER_COLOUR)
            button.set_colour_font_unhover(START_END_BARRIER_BUTTON_FONT_COLOUR)
            if button == start_cell_button:
                button.set_colour_outline(START_COLOUR)
                button.set_colour_hover(START_COLOUR)
                button.set_colour_clicked(START_COLOUR)
                if button.is_clicked():
                    button.click()
            elif button == end_cell_button:
                button.set_colour_outline(END_COLOUR)
                button.set_colour_hover(END_COLOUR)
                button.set_colour_clicked(END_COLOUR)
                if button.is_clicked():
                    button.click()
            elif button == barrier_cell_button:
                button.set_colour_outline(BARRIER_COLOUR)
                button.set_colour_hover(BARRIER_COLOUR)
                button.set_colour_clicked(BARRIER_COLOUR)
                if button.is_clicked():
                    button.click()
        else:
            button.set_colour_background(OTHER_BUTTON_COLOUR)
            button.set_colour_unhover(OTHER_BUTTON_COLOUR)
            button.set_colour_font(OTHER_BUTTON_FONT_COLOUR)
            button.set_colour_outline(OTHER_BUTTON_OUTLINE_COLOUR)
            button.set_colour_hover(OTHER_BUTTON_HOVER_COLOUR)
            button.set_colour_font_hover(OTHER_BUTTON_FONT_HOVER_COLOUR)
            button.set_colour_font_unhover(OTHER_BUTTON_FONT_COLOUR)
            if button == current_algorithm_label or button == solution_label:
                button.set_colour_outline(BACKGROUND_COLOUR)
                button.set_colour_hover(BACKGROUND_COLOUR)
                button.set_colour_font_hover(OTHER_BUTTON_FONT_COLOUR)
            if button == colour_mode_button:
                if is_dark_mode:
                    button.set_text('Bright mode')
                else:
                    button.set_text('Dark mode')

# to switch visualisation of currently clicked button between the buttons
#   start_cell, end_cell and barrier_cell. new_current_cell must be accordingly
#   'start', 'end or 'barrier'.
def swap_clicked_button(new_current_cell_type, new_clicked_button):
    global currently_clicked_button, current_cell_type
    if currently_clicked_button != None:
        currently_clicked_button.unclick()
    current_cell_type = new_current_cell_type
    new_clicked_button.click()
    currently_clicked_button = new_clicked_button
    
def swap_hovered_button(new_current_hover_button):
    global currently_hovered_button
    # not unhovering/resetting a clicked start/end/barrier button
    if currently_hovered_button != None and \
        currently_hovered_button != currently_clicked_button:
        currently_hovered_button.unhover()
    new_current_hover_button.hover()
    currently_hovered_button = new_current_hover_button

# going backwards through parent dictionary, starting at found end node
def paint_solution_path(parent_dict, current, redraw):
    global solution_path_length_count
    play_button.set_text('Painting path...')
    end = current
    end.make_end()
    while current in parent_dict:
        current = parent_dict[current]
        current.make_part_of_path()
        solution_path_length_count += 1
        redraw()
    current.make_start()
        
def make_grid():
    grid = []
    for i in range(ROWS):
        grid.append([])
        for j in range(ROWS):
            cell = Cell(i, j, CELL_WIDTH, ROWS)
            grid[i].append(cell)
    return grid

def draw_gridlines():
    for i in range(ROWS):
        # horizontal lines
        pygame.draw.line(WINDOW, GRID_LINES_COLOUR, (0, (i+1)*CELL_WIDTH), \
                         (WIDTH, (i+1)*CELL_WIDTH))
        for j in range(ROWS):
            # vertical lines
            pygame.draw.line(WINDOW, GRID_LINES_COLOUR, ((j+1)*CELL_WIDTH, 0),\
                             ((j+1)*CELL_WIDTH, WIDTH))

def redraw(grid):
    WINDOW.fill(BACKGROUND_COLOUR)
    #draw cells
    for row in grid:
        for cell in row:
            cell.paint(WINDOW)
    draw_gridlines()
    # draw buttons
    for button in buttons:
        button.draw()
    
    pygame.display.update()

# get position in row and col from mouse position in pixels
def get_click_position(pos):
    x, y = pos
    row = y // CELL_WIDTH
    col = x // CELL_WIDTH
    return row, col
    
def get_clicked_cell(grid):
    pos = pygame.mouse.get_pos()
    row, col = get_click_position(pos)
    cell = grid[row][col]
    return cell

def update_current_algorithm():
    global current_algorithm, current_algorithm_label
    current_algorithm = algorithms[current_algorithm_index]
    current_algorithm_label.set_text(current_algorithm)

def visualize_solution(parent_dict, end, redraw):
    global solution_path_length_count
    paint_solution_path(parent_dict, end, redraw)
    solution_label.set_text('Path length: '+str(solution_path_length_count))
    solution_path_length_count = 0

def main():
    global parent_dict, is_dark_mode, current_cell_type, current_algorithm_index,\
        currently_hovered_button, mouse_hold, previously_clicked_cell
    grid = make_grid()
    start = None
    end = None 
    
    clock = pygame.time.Clock()
    is_running = True
    
    while is_running:
        # limiting frame rate to 40 fps
        clock.tick(40)
        redraw(grid)
        
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                is_running = False
                pygame.quit()
                exit()
            
            mouse_pos = pygame.mouse.get_pos()
            
            # mouse over grid
            if mouse_pos[0] <= 1500:
            
                if event.type == pygame.MOUSEBUTTONDOWN:
                    mouse_hold = True
                
                if event.type == pygame.MOUSEBUTTONUP:
                    mouse_hold = False
            
                # When the mouse is hold, barrier cells are chosen, or start
                #   or end cell is dragged. The second if statement here makes
                #   sure that holding the mouse on a cell will not cause the
                #   cell to be un-/selected back and forth.
                if pygame.mouse.get_pressed()[0]: 
                    cell = get_clicked_cell(grid)
                    if previously_clicked_cell == None or \
                        cell != previously_clicked_cell or not mouse_hold:
                        if current_cell_type == 'barrier':
                            # click on a barrier cell resets this cell
                            if cell.is_barrier():
                                cell.reset()
                            else:
                                cell.make_barrier() 
                            previously_clicked_cell = cell
                        elif current_cell_type == 'start':
                            # click on current start cell resets this cell
                            if cell.is_start():
                                cell.reset()
                                start = None
                            else:
                                # reset previous start cell
                                if start != None:
                                    start.reset()
                                start = cell
                                start.make_start()
                                previously_clicked_cell = cell
                        elif current_cell_type == 'end':
                            # click on current end cell resets this cell
                            if cell.is_end():
                                cell.reset()
                                end = None                       
                            else:
                                # reset previous end cell
                                if end != None:
                                    end.reset()
                                end = cell
                                end.make_end()
                                previously_clicked_cell = cell
            # normal clicks on buttons
            elif event.type == pygame.MOUSEBUTTONDOWN:
                if colour_mode_button.mouse_is_over(mouse_pos):
                    if is_dark_mode:
                        set_bright_mode(grid)
                    else:
                        set_dark_mode(grid)
                        
                elif start_cell_button.mouse_is_over(mouse_pos):
                    swap_clicked_button('start', start_cell_button)
                    
                elif end_cell_button.mouse_is_over(mouse_pos):
                    swap_clicked_button('end', end_cell_button)
                    
                elif barrier_cell_button.mouse_is_over(mouse_pos):
                    swap_clicked_button('barrier', barrier_cell_button)
                    
                elif previous_algorithm_button.mouse_is_over(mouse_pos):
                    current_algorithm_index = (current_algorithm_index - 1) % len(algorithms)
                    update_current_algorithm()
                    
                elif next_algorithm_button.mouse_is_over(mouse_pos):
                    current_algorithm_index = (current_algorithm_index + 1) % len(algorithms)
                    update_current_algorithm()
                    
                elif play_button.mouse_is_over(mouse_pos) and start and end:
                    solution_label.set_text('')
                    play_button.set_text('Searching...')
                    for row in grid:
                        for cell in row:
                            cell.set_neighbors(grid)
                            if cell != start and cell != end and not cell.is_barrier():
                                cell.reset()
                    # Resetting the paren_dict to an empty dictionary before
                    #   each invocation of the specific algorithm.
                    parent_dict = {}
                    if current_algorithm == 'A*-Search' and \
                        astar_graphsearch.search(lambda: redraw(grid), grid, \
                        start, end, parent_dict):
                        visualize_solution(parent_dict, end, lambda: redraw(grid))
                        
                    elif current_algorithm == 'Breadth-First-Search' and \
                        breadth_first_graphsearch.search(lambda: redraw(grid), grid, \
                        start, end, parent_dict):
                        visualize_solution(parent_dict, end, lambda: redraw(grid))
                        
                    elif current_algorithm == 'Depth-First-Search' and \
                        depth_first_graphsearch.search(lambda: redraw(grid), grid, \
                        start, end, parent_dict):
                        visualize_solution(parent_dict, end, lambda: redraw(grid))

                    elif current_algorithm == 'Greedy-Best-First' and \
                        greedy_best_first_graphsearch.search(lambda: redraw(grid), grid, \
                        start, end, parent_dict):
                        visualize_solution(parent_dict, end, lambda: redraw(grid))
                        
                    elif current_algorithm == 'Uniform-Cost/Dijkstra' and \
                        uniform_cost_graphsearch.search(lambda: redraw(grid), grid, \
                        start, end, parent_dict):
                        visualize_solution(parent_dict, end, lambda: redraw(grid))
                        
                    else:
                        solution_label.set_text('No path to goal.')
                        # solution_label.set_colour_font(RED)
                    play_button.set_text('Play')
                elif reset_button.mouse_is_over(mouse_pos):
                    solution_label.set_text('')
                    start = None
                    end = None
                    # actual visible reset happens in next iteration of the 
                    #   current while loop when draw(...) is invoked, see above.
                    grid = make_grid() 
                        
            if event.type == pygame.MOUSEMOTION:
                current_hover = False
                for button in buttons:
                    if button.mouse_is_over(mouse_pos):
                        current_hover = True
                        if currently_hovered_button != button:
                            swap_hovered_button(button)
                    if not current_hover:
                        # not unhovering/resetting a clicked start/end/barrier button
                        if currently_hovered_button != None and \
                            currently_hovered_button != currently_clicked_button:
                            currently_hovered_button.unhover()
                            currently_hovered_button = None

    pygame.quit()   

main()           
            