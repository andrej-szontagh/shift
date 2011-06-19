
uniform sampler2DRect base;

#if (M_MINLEVEL <= 0) && (M_MAXLEVEL >= 0)
    uniform sampler2DRect tex0;
#endif

#if (M_MINLEVEL <= 1) && (M_MAXLEVEL >= 1)
    uniform sampler2DRect tex1;
#endif

#if (M_MINLEVEL <= 2) && (M_MAXLEVEL >= 2)
    uniform sampler2DRect tex2;
#endif

#if (M_MINLEVEL <= 3) && (M_MAXLEVEL >= 3)
    uniform sampler2DRect tex3;
#endif

#if (M_MINLEVEL <= 4) && (M_MAXLEVEL >= 4)
    uniform sampler2DRect tex4;
#endif

#if (M_MINLEVEL <= 5) && (M_MAXLEVEL >= 5)
    uniform sampler2DRect tex5;
#endif

#if (M_MINLEVEL <= 6) && (M_MAXLEVEL >= 6)
    uniform sampler2DRect tex6;
#endif

uniform float bloom;

void main ()
{
    vec4 c = vec4 (0.0, 0.0, 0.0, 0.0);

    #if (M_MINLEVEL <= 0) && (M_MAXLEVEL >= 0)
        c += texture2DRect (tex0, gl_TexCoord[0].xy             );
    #endif

    #if (M_MINLEVEL <= 1) && (M_MAXLEVEL >= 1)
        c += texture2DRect (tex1, gl_TexCoord[0].xy * 0.5       );
    #endif

    #if (M_MINLEVEL <= 2) && (M_MAXLEVEL >= 2)
        c += texture2DRect (tex2, gl_TexCoord[0].xy * 0.25      );
    #endif

    #if (M_MINLEVEL <= 3) && (M_MAXLEVEL >= 3)
        c += texture2DRect (tex3, gl_TexCoord[0].xy * 0.125     );
    #endif

    #if (M_MINLEVEL <= 4) && (M_MAXLEVEL >= 4)
        c += texture2DRect (tex4, gl_TexCoord[0].xy * 0.0625    );
    #endif

    #if (M_MINLEVEL <= 5) && (M_MAXLEVEL >= 5)
        c += texture2DRect (tex5, gl_TexCoord[0].xy * 0.03125   );
    #endif

    #if (M_MINLEVEL <= 6) && (M_MAXLEVEL >= 6)
        c += texture2DRect (tex6, gl_TexCoord[0].xy * 0.015625  );
    #endif

    gl_FragColor = texture2DRect (base, gl_TexCoord[0].xy) + bloom * c;
}

