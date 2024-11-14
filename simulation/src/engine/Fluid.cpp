#include "../../include/engine/Fluid.hpp"

#include <raylib.h>

#include <cstring>
#include <iostream>

#define PRINT_ARRAY(arr, size)                             \
    printf("{ ");                                          \
    for (int i = 0; i < size; i++) printf("%f, ", arr[i]); \
    printf("}\n");

Fluid::Fluid(int container_size, float fluid_size, float diffusion, float viscosity, float dt) {
    this->container_size = container_size;
    this->fluid_size = fluid_size;  // raylib coordinate size of a single cell
    this->dt = dt;
    this->diff = diffusion;
    this->visc = viscosity;

    int array_size = pow(container_size, 3);
    this->s = new float[array_size]();
    this->solid = new bool[array_size]();
    this->density = new float[array_size]();
    this->Vx = new float[array_size]();
    this->Vy = new float[array_size]();
    this->Vz = new float[array_size]();
    this->Vx0 = new float[array_size]();
    this->Vy0 = new float[array_size]();
    this->Vz0 = new float[array_size]();
}

Fluid::~Fluid() {
    delete[] this->s;
    delete[] this->solid;
    delete[] this->density;
    delete[] this->Vx;
    delete[] this->Vy;
    delete[] this->Vz;
    delete[] this->Vx0;
    delete[] this->Vy0;
    delete[] this->Vz0;
}

float Fluid::Density(v3 position) { return density[IXv(position)]; }
bool Fluid::Solid(v3 position) { return solid[IXv(position)]; }
float Fluid::FluidSize() { return fluid_size; }
int Fluid::ContainerSize() { return container_size; }

void Fluid::reset() {
    int array_size = pow(container_size, 3);
    std::fill(s, s + array_size, 0.0f);
    std::fill(density, density + array_size, 0.0f);
    std::fill(Vx, Vx + array_size, 0.0f);
    std::fill(Vy, Vy + array_size, 0.0f);
    std::fill(Vz, Vz + array_size, 0.0f);
    std::fill(Vx0, Vx0 + array_size, 0.0f);
    std::fill(Vy0, Vy0 + array_size, 0.0f);
    std::fill(Vz0, Vz0 + array_size, 0.0f);
}

void Fluid::add_gravity() {
    int N = container_size;
    float gravity = -2.0f;

    for (int z = 1; z < N - 1; z++) {
        for (int y = 1; y < N - 1; y++) {
            for (int x = 1; x < N - 1; x++) {
                Vy[IX(x, y, z)] += -2.0f;
            }
        }
    }
}

void Fluid::add_density(v3 position, float amount) { this->density[IXv(position)] += amount; }

void Fluid::add_velocity(v3 position, v3 amount) {
    int index = IXv(position);
    Vx[index] += amount.x;
    Vy[index] += amount.y;
    Vz[index] += amount.z;
}

void Fluid::advect(int b, float *d, float *d0, float *velocX, float *velocY, float *velocZ) {
    float dt = this->dt;
    float N = container_size;

    float i0, i1, j0, j1, k0, k1;

    float dtx = dt * (N - 2);
    float dty = dt * (N - 2);
    float dtz = dt * (N - 2);

    float s0, s1, t0, t1, u0, u1;
    float tmp1, tmp2, tmp3, x, y, z;

    float Nfloat = N;
    float ifloat, jfloat, kfloat;
    int i, j, k;

    for (k = 1, kfloat = 1; k < N - 1; k++, kfloat++) {
        for (j = 1, jfloat = 1; j < N - 1; j++, jfloat++) {
            for (i = 1, ifloat = 1; i < N - 1; i++, ifloat++) {
                if (solid[IX(i, j, k)]) {
                    if (b >= 1 && b <= 3) {  // For velocity components
                        d[IX(i, j, k)] = 0;
                    }
                    continue;
                }

                tmp1 = dtx * velocX[IX(i, j, k)];
                tmp2 = dty * velocY[IX(i, j, k)];
                tmp3 = dtz * velocZ[IX(i, j, k)];
                x = ifloat - tmp1;
                y = jfloat - tmp2;
                z = kfloat - tmp3;

                if (x < 0.5f) x = 0.5f;
                if (x > Nfloat + 0.5f) x = Nfloat + 0.5f;
                i0 = floorf(x);
                i1 = i0 + 1.0f;
                if (y < 0.5f) y = 0.5f;
                if (y > Nfloat + 0.5f) y = Nfloat + 0.5f;
                j0 = floorf(y);
                j1 = j0 + 1.0f;
                if (z < 0.5f) z = 0.5f;
                if (z > Nfloat + 0.5f) z = Nfloat + 0.5f;
                k0 = floorf(z);
                k1 = k0 + 1.0f;

                s1 = x - i0;
                s0 = 1.0f - s1;
                t1 = y - j0;
                t0 = 1.0f - t1;
                u1 = z - k0;
                u0 = 1.0f - u1;

                int i0i = i0;
                int i1i = i1;
                int j0i = j0;
                int j1i = j1;
                int k0i = k0;
                int k1i = k1;

                d[IX(i, j, k)] =
                    s0 * (t0 * (u0 * d0[IX(i0i, j0i, k0i)] + u1 * d0[IX(i0i, j0i, k1i)]) +
                          (t1 * (u0 * d0[IX(i0i, j1i, k0i)] + u1 * d0[IX(i0i, j1i, k1i)]))) +
                    s1 * (t0 * (u0 * d0[IX(i1i, j0i, k0i)] + u1 * d0[IX(i1i, j0i, k1i)]) +
                          (t1 * (u0 * d0[IX(i1i, j1i, k0i)] + u1 * d0[IX(i1i, j1i, k1i)])));
            }
        }
    }
    set_bnd(b, d);
}

void Fluid::diffuse(int b, float *x, float *x0, float diff) {
    float a = dt * diff * pow(container_size - 2, 3);
    lin_solve(b, x, x0, a, 1 + 6 * a);
}

void Fluid::lin_solve(int b, float *x, float *x0, float a, float c) {
    float cRecip = 1.0 / c;
    int N = container_size;

    for (int k = 0; k < 4; k++) {
        for (int m = 1; m < N - 1; m++) {
            for (int j = 1; j < N - 1; j++) {
                for (int i = 1; i < N - 1; i++) {
                    x[IX(i, j, m)] =
                        (x0[IX(i, j, m)] +
                         a * (x[IX(i + 1, j, m)] + x[IX(i - 1, j, m)] + x[IX(i, j + 1, m)] +
                              x[IX(i, j - 1, m)] + x[IX(i, j, m + 1)] + x[IX(i, j, m - 1)])) *
                        cRecip;
                }
            }
        }
        set_bnd(b, x);
    }
}

void Fluid::project(float *velocX, float *velocY, float *velocZ, float *p, float *div) {
    int N = container_size;

    for (int k = 1; k < N - 1; k++) {
        for (int j = 1; j < N - 1; j++) {
            for (int i = 1; i < N - 1; i++) {
                div[IX(i, j, k)] =
                    -0.5f *
                    (velocX[IX(i + 1, j, k)] - velocX[IX(i - 1, j, k)] + velocY[IX(i, j + 1, k)] -
                     velocY[IX(i, j - 1, k)] + velocZ[IX(i, j, k + 1)] - velocZ[IX(i, j, k - 1)]) /
                    N;
                p[IX(i, j, k)] = 0;
            }
        }
    }

    set_bnd(0, div);
    set_bnd(0, p);
    lin_solve(0, p, div, 1, 6);

    for (int k = 1; k < N - 1; k++) {
        for (int j = 1; j < N - 1; j++) {
            for (int i = 1; i < N - 1; i++) {
                velocX[IX(i, j, k)] -= 0.5f * (p[IX(i + 1, j, k)] - p[IX(i - 1, j, k)]) * N;
                velocY[IX(i, j, k)] -= 0.5f * (p[IX(i, j + 1, k)] - p[IX(i, j - 1, k)]) * N;
                velocZ[IX(i, j, k)] -= 0.5f * (p[IX(i, j, k + 1)] - p[IX(i, j, k - 1)]) * N;
            }
        }
    }

    set_bnd(1, velocX);
    set_bnd(2, velocY);
    set_bnd(3, velocZ);
}

void Fluid::set_bnd(int b, float *x) {
    int N = container_size;

    // Handle solid boundaries first
    for (int k = 1; k < N - 1; k++) {
        for (int j = 1; j < N - 1; j++) {
            for (int i = 1; i < N - 1; i++) {
                if (solid[IX(i, j, k)]) {
                    // For velocity components, enforce no-slip condition
                    if (b == 1) x[IX(i, j, k)] = 0;  // x velocity
                    if (b == 2) x[IX(i, j, k)] = 0;  // y velocity
                    if (b == 3) x[IX(i, j, k)] = 0;  // z velocity

                    // For density and pressure, use average of neighboring non-solid cells
                    if (b == 0) {
                        float sum = 0;
                        int count = 0;

                        if (!solid[IX(i - 1, j, k)]) {
                            sum += x[IX(i - 1, j, k)];
                            count++;
                        }
                        if (!solid[IX(i + 1, j, k)]) {
                            sum += x[IX(i + 1, j, k)];
                            count++;
                        }
                        if (!solid[IX(i, j - 1, k)]) {
                            sum += x[IX(i, j - 1, k)];
                            count++;
                        }
                        if (!solid[IX(i, j + 1, k)]) {
                            sum += x[IX(i, j + 1, k)];
                            count++;
                        }
                        if (!solid[IX(i, j, k - 1)]) {
                            sum += x[IX(i, j, k - 1)];
                            count++;
                        }
                        if (!solid[IX(i, j, k + 1)]) {
                            sum += x[IX(i, j, k + 1)];
                            count++;
                        }

                        x[IX(i, j, k)] = count > 0 ? sum / count : x[IX(i, j, k)];
                    }
                }
            }
        }
    }

    for (int j = 1; j < N - 1; j++) {
        for (int i = 1; i < N - 1; i++) {
            x[IX(i, j, 0)] = b == 3 ? -x[IX(i, j, 1)] : x[IX(i, j, 1)];
            x[IX(i, j, N - 1)] = b == 3 ? -x[IX(i, j, N - 2)] : x[IX(i, j, N - 2)];
        }
    }

    for (int k = 1; k < N - 1; k++) {
        for (int i = 1; i < N - 1; i++) {
            x[IX(i, 0, k)] = b == 2 ? -x[IX(i, 1, k)] : x[IX(i, 1, k)];
            x[IX(i, N - 1, k)] = b == 2 ? -x[IX(i, N - 2, k)] : x[IX(i, N - 2, k)];
        }
    }

    for (int k = 1; k < N - 1; k++) {
        for (int j = 1; j < N - 1; j++) {
            x[IX(0, j, k)] = b == 1 ? -x[IX(1, j, k)] : x[IX(1, j, k)];
            x[IX(N - 1, j, k)] = b == 1 ? -x[IX(N - 2, j, k)] : x[IX(N - 2, j, k)];
        }
    }

    x[IX(0, 0, 0)] = 0.33f * (x[IX(1, 0, 0)] + x[IX(0, 1, 0)] + x[IX(0, 0, 1)]);
    x[IX(0, N - 1, 0)] = 0.33f * (x[IX(1, N - 1, 0)] + x[IX(0, N - 2, 0)] + x[IX(0, N - 1, 1)]);
    x[IX(0, 0, N - 1)] = 0.33f * (x[IX(1, 0, N - 1)] + x[IX(0, 1, N - 1)] + x[IX(0, 0, N)]);
    x[IX(0, N - 1, N - 1)] =
        0.33f * (x[IX(1, N - 1, N - 1)] + x[IX(0, N - 2, N - 1)] + x[IX(0, N - 1, N - 2)]);
    x[IX(N - 1, 0, 0)] = 0.33f * (x[IX(N - 2, 0, 0)] + x[IX(N - 1, 1, 0)] + x[IX(N - 1, 0, 1)]);
    x[IX(N - 1, N - 1, 0)] =
        0.33f * (x[IX(N - 2, N - 1, 0)] + x[IX(N - 1, N - 2, 0)] + x[IX(N - 1, N - 1, 1)]);
    x[IX(N - 1, 0, N - 1)] =
        0.33f * (x[IX(N - 2, 0, N - 1)] + x[IX(N - 1, 1, N - 1)] + x[IX(N - 1, 0, N - 2)]);
    x[IX(N - 1, N - 1, N - 1)] = 0.33f * (x[IX(N - 2, N - 1, N - 1)] + x[IX(N - 1, N - 2, N - 1)] +
                                          x[IX(N - 1, N - 1, N - 2)]);
}

void Fluid::step() {
    // add_gravity();
    diffuse(1, Vx0, Vx, visc);
    diffuse(2, Vy0, Vy, visc);
    diffuse(3, Vz0, Vz, visc);

    project(Vx0, Vy0, Vz0, Vx, Vy);

    advect(1, Vx, Vx0, Vx0, Vy0, Vz0);
    advect(2, Vy, Vy0, Vx0, Vy0, Vz0);
    advect(3, Vz, Vz0, Vx0, Vy0, Vz0);

    project(Vx, Vy, Vz, Vx0, Vy0);

    diffuse(0, s, density, diff);
    advect(0, density, s, Vx, Vy, Vz);
}

void Fluid::add_cube(v3 pos, float size) {
    for (int z = pos.z; z < pos.z + size && z < container_size; z++) {
        for (int y = pos.y; y < pos.y + size && y < container_size; y++) {
            for (int x = pos.x; x < pos.y + size && x < container_size; x++) {
                solid[IX(x, y, z)] = true;
            }
        }
    }
}
