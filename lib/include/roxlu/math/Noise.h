/** 
 * This class is based on the open source SimplexNoise123 clas from 
 * Stefan Gustavson who made it freely available. Many thanks to Stefan!
 *
 * Original author:
 * ----------------- 
 * Copyright © 2003-2011, Stefan Gustavson
 * Contact: stegu@itn.liu.se
 * URL: http://staffwww.itn.liu.se/~stegu/aqsis/aqsis-newnoise/
 */

class Noise {

  public:
    Noise() {}
    ~Noise() {}

    /* SIGNED: 1D, 2D, 3D and 4D float Perlin noise: range is between -1.0f and 1.0f */
    static float noise( float x );
    static float noise( float x, float y );
    static float noise( float x, float y, float z );
    static float noise( float x, float y, float z, float w );


    /* UNSIGNED: 1D, 2D, 3D and 4D float Perlin noise: range is between 0.0f and 1.0f */
    static float unoise( float x );
    static float unoise( float x, float y );
    static float unoise( float x, float y, float z );
    static float unoise( float x, float y, float z, float w );

  private:
    static unsigned char perm[];
    static float grad( int hash, float x );
    static float grad( int hash, float x, float y );
    static float grad( int hash, float x, float y , float z );
    static float grad( int hash, float x, float y, float z, float t );
};


