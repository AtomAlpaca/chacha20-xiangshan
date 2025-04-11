#include <assert.h>
#include <riscv_vector.h>
#include <stdio.h>

typedef unsigned int u32;
typedef unsigned char u8;

typedef union {
    u32 u[16];
    u8 c[64];
} chacha_buf;

const u32 p[12] = {
    5  * sizeof(u32), 6  * sizeof(u32), 7  * sizeof(u32), 4  * sizeof(u32),
    10 * sizeof(u32), 11 * sizeof(u32), 8  * sizeof(u32), 9  * sizeof(u32),
    15 * sizeof(u32), 12 * sizeof(u32), 13 * sizeof(u32), 14 * sizeof(u32)};

void chacha20(chacha_buf *output, const u32 input[16])
{
    u32 x[16];
    int i;

    for (int i = 0; i < 16; i++) {
        x[i] = input[i];
    }

    for (i = 20; i > 0; i -= 2) {
        size_t vl = vsetvl_e32m1(4);
        vuint32m1_t va, vb, vc, vt, vt1, vt2, vt3;

        va = vlwu_v_u32m1(x + 0, vl);
        vb = vlwu_v_u32m1(x + 4, vl);
        vc = vadd_vv_u32m1(va, vb, vl);
        vsw_v_u32m1(x + 0, vc, vl);

        /*
        asm volatile(
        "vlwu.v v1, (%0) \n \
            vlwu.v v2, (%1) \n \
            vxor.vv v3, v1, v2 \n \
            vrol v4, v3, 16 \n \
            vsw.v v4, (%0)"
        :"=r"(*(x + 12)), "=r"(*x)
        );*/
        va = vlwu_v_u32m1(x + 12, 4);
        vb = vlwu_v_u32m1(x + 0,  4);
        vt = vxor_vv_u32m1(va, vb, 4);
        vt1 = vsrl_vx_u32m1(vt, 16, 4);
        vt2 = vsll_vx_u32m1(vt, 16, 4);
        vt = vor_vv_u32m1(vt1, vt2, 4);
        vsw_v_u32m1(x + 12, vt, 4);
        /*
        x[12] = ROTATE(x[12] ^ x[0], 16);
        x[13] = ROTATE(x[13] ^ x[1], 16);
        x[14] = ROTATE(x[14] ^ x[2], 16);
        x[15] = ROTATE(x[15] ^ x[3], 16);
*/
        va = vlwu_v_u32m1(x + 8, vl);
        vb = vlwu_v_u32m1(x + 12, vl);
        vc = vadd_vv_u32m1(va, vb, vl);
        vsw_v_u32m1(x + 8, vc, vl);

        va = vlwu_v_u32m1(x + 4, 4);
        vb = vlwu_v_u32m1(x + 8,  4);
        vt = vxor_vv_u32m1(va, vb, 4);
        vt1 = vsrl_vx_u32m1(vt, 20, 4);
        vt2 = vsll_vx_u32m1(vt, 12, 4);
        vt = vor_vv_u32m1(vt1, vt2, 4);
        vsw_v_u32m1(x + 4, vt, 4);

        va = vlwu_v_u32m1(x + 0, vl);
        vb = vlwu_v_u32m1(x + 4, vl);
        vc = vadd_vv_u32m1(va, vb, vl);
        vsw_v_u32m1(x + 0, vc, vl);

        va = vlwu_v_u32m1(x + 12, 4);
        vb = vlwu_v_u32m1(x + 0,  4);
        vt = vxor_vv_u32m1(va, vb, 4);
        vt1 = vsrl_vx_u32m1(vt, 24, 4);
        vt2 = vsll_vx_u32m1(vt, 8, 4);
        vt = vor_vv_u32m1(vt1, vt2, 4);
        vsw_v_u32m1(x + 12, vt, 4);

        va = vlwu_v_u32m1(x + 8, vl);
        vb = vlwu_v_u32m1(x + 12, vl);
        vc = vadd_vv_u32m1(va, vb, vl);
        vsw_v_u32m1(x + 8, vc, vl);

        va = vlwu_v_u32m1(x + 4, 4);
        vb = vlwu_v_u32m1(x + 8, 4);
        vt = vxor_vv_u32m1(va, vb, 4);
        vt1 = vsrl_vx_u32m1(vt, 25, 4);
        vt2 = vsll_vx_u32m1(vt, 7, 4);
        vt = vor_vv_u32m1(vt1, vt2, 4);
        vsw_v_u32m1(x + 4, vt, 4);

        va = vlwu_v_u32m1(x + 0, 4);
        vt = vlwu_v_u32m1(p + 0, 4);   // vt = (5, 6, 7, 4)
        vb = vlxwu_v_u32m1(x, vt, 4); // vb = (x[5], x[6], x[7], x[4])
        vc = vadd_vv_u32m1(va, vb, vl);
        vsw_v_u32m1(x + 0, vc, vl);

        vc = vlwu_v_u32m1(p + 8, 4);
        va = vlxwu_v_u32m1(x, vc, 4);
        vb = vlwu_v_u32m1(x + 0, 4);
        vt = vxor_vv_u32m1(va, vb, 4);
        vt1 = vsrl_vx_u32m1(vt, 16, 4);
        vt2 = vsll_vx_u32m1(vt, 16, 4);
        vt = vor_vv_u32m1(vt1, vt2, 4);
        vsxw_v_u32m1(x, vc, vt, 4);

        vt1 = vlwu_v_u32m1(p + 4, 4);
        va = vlxwu_v_u32m1(x, vt1, vl);
        vt2 = vlwu_v_u32m1(p + 8, 4);
        vb = vlxwu_v_u32m1(x, vt2, vl);
        vc = vadd_vv_u32m1(va, vb, vl);
        vsxw_v_u32m1(x, vt1, vc, vl);

        vc = vlwu_v_u32m1(p + 4, 4);
        vb = vlxwu_v_u32m1(x, vc, 4);
        vc = vlwu_v_u32m1(p + 0, 4);
        va = vlxwu_v_u32m1(x, vc, 4);
        vt = vxor_vv_u32m1(va, vb, 4);
        vt1 = vsrl_vx_u32m1(vt, 20, 4);
        vt2 = vsll_vx_u32m1(vt, 12, 4);
        vt = vor_vv_u32m1(vt1, vt2, 4);
        vsxw_v_u32m1(x, vc, vt, 4);

        va = vlwu_v_u32m1(x + 0, vl);
        vt = vlwu_v_u32m1(p + 0, 4);
        vb = vlxwu_v_u32m1(x, vt, vl);
        vc = vadd_vv_u32m1(va, vb, vl);
        vsw_v_u32m1(x + 0, vc, vl);

        vc = vlwu_v_u32m1(p + 8, 4);
        va = vlxwu_v_u32m1(x, vc, 4);
        vb = vlwu_v_u32m1(x + 0, 4);
        vt = vxor_vv_u32m1(va, vb, 4);
        vt1 = vsrl_vx_u32m1(vt, 24, 4);
        vt2 = vsll_vx_u32m1(vt, 8, 4);
        vt = vor_vv_u32m1(vt1, vt2, 4);
        vsxw_v_u32m1(x, vc, vt, 4);

        vt1 = vlwu_v_u32m1(p + 4, 4);
        va = vlxwu_v_u32m1(x, vt1, vl);
        vt2 = vlwu_v_u32m1(p + 8, 4);
        vb = vlxwu_v_u32m1(x, vt2, vl);
        vc = vadd_vv_u32m1(va, vb, vl);
        vsxw_v_u32m1(x, vt1, vc, vl);

        vc = vlwu_v_u32m1(p + 4, 4);
        vb = vlxwu_v_u32m1(x, vc, 4);
        vc = vlwu_v_u32m1(p + 0, 4);
        va = vlxwu_v_u32m1(x, vc, 4);
        vt = vxor_vv_u32m1(va, vb, 4);
        vt1 = vsrl_vx_u32m1(vt, 25, 4);
        vt2 = vsll_vx_u32m1(vt, 7, 4);
        vt = vor_vv_u32m1(vt1, vt2, 4);
        vsxw_v_u32m1(x, vc, vt, 4);
    }

    for (i = 0; i < 16; ++i)
        output->u[i] = x[i] + input[i];
}
