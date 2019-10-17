
uniform int nSpheres;
uniform vec2[1024] positions;
uniform float radius;


bool isIntersecting(int sphereX, int sphereY)
{
    float squareDist = dot(positions[sphereX] - positions[sphereY],
                           positions[sphereX] - positions[sphereY]);

    if(squareDist <= 4.0*radius*radius)
        return true;

    return false;
}

void main()
{
    int sphere1 = int(gl_TexCoord[0].x * float(nSpheres));
    int sphere2 = int((1.0-gl_TexCoord[0].y) * float(nSpheres));

    if(isIntersecting(sphere1, sphere2))
        gl_FragColor = vec4(1.0);
    else
        gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);

    //gl_FragColor = vec4(gl_TexCoord[0].xy, 0.0, 1.0);
}
