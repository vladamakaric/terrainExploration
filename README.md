# Unknown terrain exploration
This project was created to aid a research project I did at Petnica science center (http://www.petnica.rs/), during the summer of 2011 after finishing my third year of high school.

The create a model of 2D terrain exploration by an entity (explorer) with a narrow view field. 
The goal was to make an "intelligent" entity able to find a path from start to goal points positioned in 
an unknown terrain with obstacles and being able to go back to the starting point along an optimized (shortest) path.

As prerequisites for this simulation (UTE), the following problems had to be solved:

1. Terrain representation: A 2D vector aproach was chosen, where obstacles are simple polygons (they can be concave but not self intersecting).
2. Procedural complex terrain genration: The ability to generate terrain with different characteristics, for example terrain with small amounts of long detailed connected (chain-like) land masses similar to mountain ranges and terrain with small and simple but many polygons can also be generated. 
3. Optimization of collision detection: Because the size of the terrain can be quite large (number of vertices of the polygons), collision detection between the entityes field of view and the obstacles had to be heavily optimized.
4. Memory: The ability to keep track of terrain already explored, and the ability to expand this knowledge with newly explored terrain.
5. Terrain navigation: optimal navigation through known terrain and heuristic navigation through unknown terrain.

The result of the research is a paper on unknown terrain exploring that was published in the 2011 Petnica science center students' projects journal, ISBN: 978-86-7861-048-6.

The paper, with all the details of the above mentione problems and more (most of it is in serbian)
is available in [PDF](flocking2010.pdf).

##Dependencies, compilation
The only library needed to compile this is [libsdl1.2](https://www.libsdl.org/download-1.2.php)
For compilation just use the makefile.

##Instructions

Run the application from the console. Type the number of the algorithm you want to run. 
After that type 0 to create a new simulation. Press ENTER.

![Alt text](/../screenshots/img/consoleScreen.png?raw=true "Optional Title")

After that you will get a new window with a blank screen, where you must define obstacles,
goal and entity positions, the instructions for doing so are given in the console window.

##Example


###Leader following
![Alt text](/../screenshots/img/leader.png?raw=true "Optional Title")
###Closest entity following
![Alt text](/../screenshots/img/closest.png?raw=true "Optional Title")
###Autonomous movement
![Alt text](/../screenshots/img/autonomous.png?raw=true "Optional Title")

