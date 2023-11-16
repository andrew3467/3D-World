//
// Created by apgra on 10/25/2023.
//

#include <random>
#include <queue>
#include "ErosionSimulator.h"


#define GRAVITY 9.81

int randMToN(int M, int N)
{
    return M + (rand() / (RAND_MAX / (N-M)));
}
float randMToN(float M, float N)
{
    return M + (rand() / (RAND_MAX / (N-M)));
}


glm::vec2 calcGradient(std::vector<std::vector<float>> &map, glm::ivec2 p){
    //Interpolation parameters, 0.5f is set to center of cell
    float u = 0.5f;
    float v = 0.5f;


    glm::vec2 p1 = {
            (map[p.x + 1][p.y] - map[p.x][p.y]) * (1 - v),
            (map[p.x][p.y + 1] - map[p.x][p.y] * (1 - u))
    };

    glm::vec2 p2 = {
            (map[p.x + 1][p.y + 1] - map[p.x][p.y + 1]) * v,
            (map[p.x + 1][p.y + 1] - map[p.x + 1][p.y] * u)
    };

    return p1 + p2;
}


void ErosionSimulator::SimulateErosion2D(std::vector<std::vector<float>> &map, ErosionConfig &config, int seed) {
    int mapWidth = map.size();
    int mapHeight = map[0].size();

    srand(seed);

#define DEPOSIT_AT(X, Z, W) \
  { \
    float delta=ds*(W); \
    map[(X)][(Z)] += delta; \
  }

#define DEPOSIT(H) \
    DEPOSIT_AT(xi  , zi  , (1-xf)*(1-zf)) \
      DEPOSIT_AT(xi+1, zi  ,    xf *(1-zf)) \
      DEPOSIT_AT(xi  , zi+1, (1-xf)*   zf ) \
      DEPOSIT_AT(xi+1, zi+1,    xf *   zf ) \
    (H)+=ds;

    for(int iter = 0; iter < config.numDroplets; iter++){
        int xi = randMToN(0, mapWidth - 1);
        int zi = randMToN(0, mapHeight - 1);

        float xp = xi, zp = zi;
        float xf = 0.0f, zf = 0.0f;

        float h = map[xi][zi];
        float s = 0.0f, v = 0.0f, w = 1.0f;

        float h00 = h;
        float h10 = map[xi + 1][zi];
        float h01 = map[xi][zi + 1];
        float h11 = map[xi + 1][zi + 1];

        float dx = 0.0f, dz = 0.0f;

        unsigned int numMoves = 0;
        for(; numMoves < config.maxSteps; numMoves++){
            //calc gradient
            float gx = h00 + h01 - h10 - h11;
            float gz = h00 + h10 - h01 - h00;

            dx = (dx - gx) * config.inertia + gx;
            dz = (dz - gz) * config.inertia + gz;

            float dl = sqrt(dx*dx + dz*dz);
            //Particle stopped, pick random new dirction
            if(dl < 0.0001f){
                dx = randMToN(0.0f, 1.0f);
                dz = randMToN(0.0f, 1.0f);
            }else{
                dx /= dl;
                dz /= dl;
            }

            float nxp=xp+dx;
            float nzp=zp+dz;

            int nxi = fmax(0, floor(nxp));
            int nzi = fmax(0, floor(nzp));
            float nxf = nxp - nxi;
            float nzf = nzp - nzi;

            float nh00 = map[nxi][nzi];
            float nh10 = map[nxi + 1][nzi];
            float nh01 = map[nxi][nzi + 1];
            float nh11 = map[nxi + 1][nzi + 1];

            float nh=(nh00*(1-nxf)+nh10*nxf)*(1-nzf)+(nh01*(1-nxf)+nh11*nxf)*nzf;

            if(nh >= h){
                float ds = (nh - h) + 0.001f;

                if(ds >= s){
                    ds = s;
                    DEPOSIT(h)
                    s = 0.0f;
                    break;
                }

                DEPOSIT(h)
                s-= ds;
                v=0.0f;
            }

            //Transport Carry
            float dh = h - nh;
            float slope = dh;
            float q = fmax(slope, config.minSlope) * v * w;

            //Deposit / erode
            float ds = s - q;
            if(ds >= 0.0f){
                //deposit
                ds *= config.depositionRate;
                DEPOSIT(dh)
                s-=ds;
            }else{
                //erode
                ds *= config.erosionRate;
                ds = fmin(ds, dh*0.99f);

                #define ERODE(X, Z, W) \
                 { \
                     float delta=ds*(W); \
                     map[(X)][(Z)]-=delta; \
                }

                for (int z=zi-1; z<=zi+2; ++z)
                {
                    float zo=z-zp;
                    float zo2=zo*zo;

                    for (int x=xi-1; x<=xi+2; ++x)
                    {
                        float xo=x-xp;

                        float w=1-(xo*xo+zo2)*0.25f;
                        if (w<=0) continue;
                        w*=0.1591549430918953f;

                        ERODE(x, z, w)
                    }
                }

                dh -= ds;
                #undef ERODE
                s+=ds;
            }

            // move to the neighbour
            v=sqrtf(v*v*dh);
            w*=1;

            xp=nxp; zp=nzp;
            xi=nxi; zi=nzi;
            xf=nxf; zf=nzf;

            h=nh;
            h00=nh00;
            h10=nh10;
            h01=nh01;
            h11=nh11;
        }
    }

#undef DEPOSIT
#undef DEPOSIT_AT
}
