#version 330

// Input uniform values
uniform mat4 mvp;

// Output vertex attributes (to fragment shader)
out vec2 fragTexCoord; //uv
out vec4 fragColor; //gl_FragColor

// Input values
in vec3 vertexPosition; 
in vec2 vertexTexCoord;
in vec4 vertexColor; //frag colour in

// NOTE: Add here your custom variables

void main()
{
    // Send vertex attributes to fragment shader
    fragTexCoord = vertexTexCoord;
    fragColor = vertexColor;

    gl_Position = mvp*vec4(vertexPosition, 1.0);
}