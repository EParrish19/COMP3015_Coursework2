# COMP3015 Coursework 2 OpenGL Developer Tool Prototype

### This is my submission for COMP3015 Coursework 2, this submission build supon my submission for coursework 1, adding extra effects such as shadows. As stated, my submission for coursework 1 was used as a start point for this submission.

## Software

### For this project, I have used Visual Studio 2019 and Windows 10.

## Project use

### This project builds upon the previous coursework, but instead of showing differences between phong, blinn-phong and toon shading, this project instead focuses on showcasing shadows and a wireframe renderer, as well as having limited user input, specifically, left arrow key to select shadows shader and right arrow key to select wireframe shader.

### To run the project, use the compiled Project_Template.exe in x64/debug, or access the solution with Project_Template.sln.

### Controls are left arrow for shadows shader, right arrow for wireframe shader.

## Code Flow

### Unlike the first coursework, this project splits up shaders into seperate shader files and GLSLProgram variables, which is cleaner to read. As before, the scenebasic_uniform.cpp file controls most of the matrices and uniforms for objects, shaders and textures, while also handling user input.

### For vertex shaders, it is mostly the same, with both the wireframe.vert and shadows.vert shaders doing minimal calculations, but the wireframe shader also has wireframe.geom, a geometry shader, responsible for generating and calculating the primitives (in this case Triangles) on an object, which is then passed to the fragment shader which will colour the edges of these triangles a different colour, giving a wireframe effect.

### For shadows, there is no need for a geometry shader, so data goes to the fragment shader, which first does one pass to generate a depth texture shadowmap (generated from the point of view of the light frustum in the scene), then does a second pass to render the scene, using the shadowmap to determine whether a certain fragment should be coloured according to all light or only ambient light, as well as texture colour.

## Additional Resources

### If not already set up, GLAD, GLFW and GLM will be required.

## Video Link
### https://youtu.be/ry8gWpIKBYs

## Repo Link
https://github.com/EParrish19/COMP3015_Coursework2
