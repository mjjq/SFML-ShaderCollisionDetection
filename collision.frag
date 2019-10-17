void main()
{
    vec2 coord = gl_TexCoord[0].xy;
    gl_FragColor = vec4(coord, 1.0, 1.0);
}
