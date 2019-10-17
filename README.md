# SFML Shader Collision Detection

This is a basic implementation of collision detection of moving circles in SFML.

The source contains two different methods to detect collisions:

+ CPU based with a double nested loop
+ GPU based using a fragment shader

The CPU based algorithm simply loops through distinct circle pairs and checks if their distance is less than the combined radii of the circles.

The GPU algorithm is roughly the same. Imagine a 2D (square) texture where each pixel represents a pair of circles. For example, the pixel located at (1,2) in the texture represents circles 1 and 2. The color of this pixel tells us whether or not circles 1 and 2 are colliding (white for is colliding, black for isn't colliding). Reading from this texture for all pixels allows us to generate a list of all possible colliding pairs.

A shader can fill in this texture for us. A fragment shader operates on all pixels simultaneously or asynchronously. This is due to the huge number of cores available on a GPU being able to run their own threads separately. If we feed the positions and radii for all the circles to the shader, each core will then calculate its designated pixel value. Again, this is determined by seeing if the distance between the circles is less than the summed radii. Because of this asynchronous feature of GPUs, the resulting "collision texture" can be calculated extremely quickly.

When you attempt to compare the speeds of the CPU and GPU detection methods in the source, you find that the CPU method is in fact faster than the GPU method. This is because the "collision texture" needs to be copied off the GPU to the CPU for processing. For large numbers of circles, this texture can be extremely large and will take considerable time to copy to the CPU. It should, in theory, be possible to get around this if you move other aspects of the simulation to the GPU as well.

## Why not use a compute shader?

SFML 2.5.1 has no native support for compute shaders. It was interesting to see if collision detection could be done on the GPU while keeping to the limitations of SFML.
