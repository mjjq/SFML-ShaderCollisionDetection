
uniform sampler2D collisionTexture;

uniform int nSpheres;

bool getIntersectingPixel(int sphere1, int sphere2)
{
    float texCoordX = (float(sphere1) +0.5)/ float(nSpheres);
    float texCoordY = (float(sphere2) +0.5)/ float(nSpheres);

    vec4 pixelValue = texture2D(collisionTexture, vec2(texCoordX, texCoordY));

    if(pixelValue.x > 0.5)
        return true;

    return false;
}

bool isIntersecting(int currentSphere)
{
    for(int i=0; i<nSpheres; ++i)
        if(getIntersectingPixel(i, currentSphere)==true && i != currentSphere)
            return true;

    return false;
}

void main()
{
    int sphere = int(gl_TexCoord[0].x * float(nSpheres));

    if(isIntersecting(sphere))
        gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);
    else
        gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);

    //gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}
