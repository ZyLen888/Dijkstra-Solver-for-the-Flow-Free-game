# Dijkstra Algorithm: Solver for the Flow Free game
🎯 Applying learned knowledge about Dijkstra Algorithm to build a solver for the Flow Free Game   

💻 Algorithm related work done in C as school projects  

## Background Information

### Flow Free - a circuit design problem

![Image1](https://static.au.edusercontent.com/files/OF1bMTkGGh75ayNrEGGm9idk)

In this programming assignment you’ll be expected to build a solver for the [Flow Free game](https://en.wikipedia.org/wiki/Flow_Free). Its origins can be traced back to 1897, a puzzle published by the professional puzzler Sam Loyd, in a column he wrote for the Brooklyn Daily Eagle (more details in the Matematica Journal). This puzzle was popularised in Japan under the name of Numberlink. 

You can play the game for free on your android/apple phone/pc, over a web browser or solve it compiling the code given to you.

The code in this assignment was adapted from the open-source terminal version made available by [mzucker](https://github.com/mzucker/flow_solver).

![Image2](https://static.au.edusercontent.com/files/cEsSDRKv1xYVNoLCBZ8cjzze)

The game presents a grid with colored dots occupying some of the squares. The objective is to connect dots of the same color by drawing pipes between them such that the entire grid is occupied by pipes. However, pipes may not intersect. 

The difficulty is determined by the size of the grid, ranging from 5x5 to 15x15 squares.

## Deliverables

### Deliverable 1 - Dijkstra Solver source code

You are expected to hand in the source code for your solver, written in C. 
Your implementation should be able to solve the regular puzzles provided.

![Image 3](https://static.au.edusercontent.com/files/EPADi6FchzuZCkaVNG9bq4FC)

⭐️ dead-end detection: A dead-end is a configuration for which you know a solution cannot exist. The figure above shows an example of a dead-end: the cell marked with X cannot be filled with any color. The problem is unsolvable with the current configuration, and we do not need to continue searching any of its successors. 

### Deliverable 2 – Experimentation 

Besides handing in the solver source code, you’re required to provide a table reporting at least the execution time and number of generated nodes with and without dead-end detection. Include in the table only the puzzles that your solver finds a solution to. 
