#version 410 core

layout(location = 0) in vec4 pos;    // 1st attribute buffer = vertex positions
layout(location = 1) in vec4 col;       // 2nd attribute buffer = colors
layout(location = 2) in vec2 tex;     // 3rd attribute buffer = textures

out vec4 vCol;                             //color sent to rest of pipeline
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;                        //the matrix for the model

void main()
{
    gl_Position = projection * view * model * pos; //calculate the position of the model
    vCol  = col;                            //passing on the vertex color

    TexCoord = tex;
}
