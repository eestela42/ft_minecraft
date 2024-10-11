# ft_vox <br>
C++ OpenGL voxel with advanced procedural generation <br>

We generate a world using Perlin noise in  Classes/World/ChunkGenerator. <br>
The player can move their camera, flying in the air indefinitely throughout the world. <br>
The Perlin noises that define the generation loop around every few thousand blocks, but not at the same pace, <br>
making it almost an infinite world. <br>
There is a simple gravity, block collision, and entity management system in place. <br>

The abstract class Chunk allows us to use different types of child classes, enabling us to manage the data as needed. <br>
The main child class we utilize is ChunkRLE, <br>
where we manage the blocks of the chunk as ribbons (rubans). <br>

We experimented with the geometry shader. <br>
In the CPU, we generate a buffer containing our face information. <br>
Each face info includes the position of the block, its type, and the 'size of the face'. <br>

'Size of the face' refers to grouping multiple faces of the same wall and block type, <br>
reducing the number of triangles to display. <br>

Inside the GPU, we create our triangles using the geometry shader. It reduces the amount of RAM used however this does not significantly improve the FPS <br>

![Montains](assets/montain.png)

![Desert](assets/desert.png)

![Desert](assets/eaublock.png)

[![Watch the video](assets/video.jpg)](https://www.youtube.com/watch?v=5nc3raWCv7w)


--eestela main dev       ChunkRLE/Shaders/World logic/Generation <br>
--leocrabe225 main dev   tools/managing data <br>
--jdamoise main dev      Makefile <br>

We will implement an easy way to install in the next version <br>
If you want to try, it runs on linux and we more or less followed learnopengl.com/Getting-started/Creating-a-window <br>

The window says not responding while loading the world for the first time, it is normal, we will change that later. <br>
