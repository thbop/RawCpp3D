# RawCpp3D
A custom 3D renderer using only raw C++

Renders:

![render0](https://github.com/thbop/RawCpp3D/blob/main/showcase/render1.png)
![normals0](https://github.com/thbop/RawCpp3D/blob/main/showcase/normals0.png)

The original inspiration for this project: [How do Video Game Graphics Work by Branch Education](https://www.youtube.com/watch?v=C8YtdC8mxTU&t=819s).

After 3-5 attempts at a custom "3D rendering engine," this is my first one to actually work. It takes triangles (assumed to already exist in camera space), converts them to screen space, rasterizes them onto an image (taking into account per-pixel z-depth from the camera for overlap cases), and applies **very basic** shading using a single light source. The image produced is exported in .ppm format, as they are very easy to generate from raw C++.

Much of the code is messy (like calculating normals / lighting in the rasterization function), but it still works.

Aspiring young developers wishing to make simple 3D renderers like this one should first learn basic graphics concepts (maybe by experimenting with [Blender](https://www.blender.org/) and other resources; specificially educational resources), vector operations such as [dot](https://www.khanacademy.org/math/multivariable-calculus/thinking-about-multivariable-function/x786f2022:vectors-and-matrices/a/dot-products-mvc) and [cross products](https://www.khanacademy.org/math/multivariable-calculus/thinking-about-multivariable-function/x786f2022:vectors-and-matrices/a/cross-products-mvc), and possibly other [linear algebra](https://www.youtube.com/playlist?list=PLZHQObOWTQDPD3MizzM2xVFitgF8hE_ab) concepts such as planes and matrices.

Check out my [references](https://github.com/thbop/RawCpp3D/blob/main/references/references.md) for more helpful websites, videos, and equations on the topic.