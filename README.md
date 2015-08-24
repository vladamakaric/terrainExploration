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
is available in [PDF](ute2011.pdf).

##Dependencies, compilation
The only library needed to compile this is [libsdl1.2-dev](https://www.libsdl.org/download-1.2.php). For compilation just use the makefile.

##Instructions

Run the application from the console. Type 0 to proceduraly generate the terrain and 0 again to not save it to a file.

![Alt text](/../screenshots/img/console.png?raw=true "Optional Title")

After that you will get a new window with terrain, it will be zoomed in, zoom out using the down arrow until
you can see the whole terrain. 

![Alt text](/../screenshots/img/1.png?raw=true "Optional Title")


Left click anywhere to place the entity.

![Alt text](/../screenshots/img/2.png?raw=true "Optional Title")

The circle around the entity (which is represented by a point), will light up, this represents the explored terrain.

Right clicking somewhere in the unexplored terrain will initiate the exploration, the entity will attempt to move to that position.

![Alt text](/../screenshots/img/3.png?raw=true "Optional Title")

After the entity has reached the goal you gave it, you can right click again to give it another goal, for example you can click in the area inside the obstacle.

![Alt text](/../screenshots/img/4.png?raw=true "Optional Title")

The entity does not know that this goal is unreachable, because that area is unexplored. It will try to reach the point until it sees the whole obstacle. Then the obstacle turns red indicating that the entity has seen it fully and knows that points inside it are unreachable.

![Alt text](/../screenshots/img/5.png?raw=true "Optional Title")

Here are examples of paths traced out by the enitty when trying to reach goal points in different terrains, notice the smootheness of the terrain, that is a consequence of the small viewpoint, the entity keeps moving towards the edge of its viewport thinking that the obstacle wil end there.

![Alt text](/../screenshots/img/6.png?raw=true "Optional Title")


