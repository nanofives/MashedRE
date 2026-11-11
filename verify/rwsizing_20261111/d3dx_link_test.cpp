#include <windows.h>
#include <d3dx9math.h>
#include <cstdio>
int main(){ D3DXMATRIX a,b,c; D3DXMatrixIdentity(&a); D3DXMatrixRotationY(&b,0.5f); D3DXMatrixMultiply(&c,&a,&b); D3DXVECTOR3 v(1,2,3),o; D3DXVec3Normalize(&o,&v); printf("%f %f %f\n",c._11,c._13,o.x); return 0; }
