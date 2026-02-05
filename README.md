# GP3 Coursework
A repository for the work done on my Y4 Games Programming 3 module's labs and subsequent coursework


# Naming Conventions

C++ Style Link: https://google.github.io/styleguide/cppguide.html

## Quick Notes (C++)

Variables: Snake Case
- Private (Classes Only): '_' suffix
- Constants: 'k' prefix

Enums:
- Name enums like constants ('k' prefix), not macros (All caps)

Functions:
- Pascal Case (Start with caps, first letter of words is capitalised)
- Accessors & Mutators (Getters & Setters): Named like variables (E.g. int count -> set_count(int count))

 
## Shader Notes:

Naming Convention:
- Vertex Shader Layout Variables: Pascal Case
- Variable & Parameter Names: Snake Case (All lowercase, words separated by underscores)
- Function Names: Pascal Case

Vertex Layout Input Names & Positions:
- Layout 0: vec3 VertexPosition
- Layout 1: vec3 VertexNormal
- Layout 2: vec2 TextureCoordinate


Matrix Names:
- MVP Matrix: transform
- Model Matrix: modelMatrix
- View Matrix: viewMatrix
- Projection Matrix: projectionMatrix
- Normal Matrix: normalMatrix

Texture Names (Replace N with a positive integer >= 1):
- Diffuse: texture_diffuseN
- Specular: texture_specularN
- Normal: texture_normalN
- Height: texture_heightN