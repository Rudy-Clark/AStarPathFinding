#version 330

#define SIZE 256

out vec4 fragColor;
uniform vec2 screenSize;
uniform float gridSpacing;
uniform vec3 gridColor;

uniform vec2 u_activeCell;
uniform vec3 u_activeCellColor;

uniform vec3 u_obstacleColor;
uniform vec2 u_obstacles[SIZE];

void main() {
    vec2 fragCoord = gl_FragCoord.xy;
    vec2 gridPos = floor(fragCoord / gridSpacing);
    vec4 color = vec4(gridColor, 1.0);
    vec4 backgroundColor = vec4(1.0, 1.0, 1.0, 1.0);

    

    for (int i = 0; i < SIZE; i++) {
        if (u_obstacles[i] == gridPos) {
            backgroundColor = vec4(u_obstacleColor, 1.0);
            break;
        }
    }

    // change color of the active cell
    if (gridPos == u_activeCell)
    {
        backgroundColor = vec4(u_activeCellColor, 1.0);
    }

    fragColor = backgroundColor;

    float lineX = mod(fragCoord.x, gridSpacing);
    float lineY = mod(fragCoord.y, gridSpacing);

    if (lineX < 1.0 || lineY < 1.0) {  // Adjust thickness if needed
        fragColor = vec4(gridColor, 1.0);
    }
}