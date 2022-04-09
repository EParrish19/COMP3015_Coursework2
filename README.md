# COMP3015 Coursework 1 OpenGL shader prototype

### This is my submission for COMP3015 Coursework 1, within is my fragment and vertex shaders and scene set up files, models were provided as well as a template scene set up, as well as this, a basic diffuse model shader was also provided as a start point.

## Software

### For this project, I have used Visual Studio 2019 and Windows 10.

## Project use

### The project is purely to show the differences in different shading methods, specifically Phong, Blinn-Phong and Toon Shading, which will be swapped between in that order every 10 seconds.

## Code Flow

### The scenbasic_uniform.cpp file controls setting matrices for objects in the scene as well as setting up uniforms used in the shaders and what textures are used. The vertex shader does minimal calculations and passes on results to the fragment shader.The fragment shader contains the different methods for each shading method, including applying textures and lighting effects.

## Additional Resources

### If not already set up, GLAD, GLFW and GLM will be required.

## Video Link
### [Video](https://youtu.be/ltssehdzD9k)
