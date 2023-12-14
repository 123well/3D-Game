#include <iostream>
#include <math.h>
#include <stdio.h>
#include <windows.h>
#include <cstring>
#include <stdlib.h>
#include <wchar.h>
#include <list>
#include <iterator>
using namespace std;

float clamp(float value, float min, float max) { return fmax(fmin(value, max), min); }
double sign(double a) { return (0 < a) - (a < 0); }
double step(double edge, double x) { return x > edge; }
struct Vec2 {
    float x, y;

    Vec2(float value) : x(value), y(value) {}
    Vec2(float _x, float _y) : x(_x), y(_y) {}

    Vec2 operator+(Vec2 const& other) { return Vec2(x + other.x, y + other.y); }
    Vec2 operator-(Vec2 const& other) { return Vec2(x - other.x, y - other.y); }
    Vec2 operator*(Vec2 const& other) { return Vec2(x * other.x, y * other.y); }
    Vec2 operator/(Vec2 const& other) { return Vec2(x / other.x, y / other.y); }
};
struct Vec3
{
    float x, y, z;

    Vec3(float _value) : x(_value), y(_value), z(_value) {};
    Vec3(float _x, Vec2 const& v) : x(_x), y(v.x), z(v.y) {};
    Vec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {};

    Vec3 operator+(Vec3 const& other) { return Vec3(x + other.x, y + other.y, z + other.z); }
    Vec3 operator-(Vec3 const& other) { return Vec3(x - other.x, y - other.y, z - other.z); }
    Vec3 operator*(Vec3 const& other) { return Vec3(x * other.x, y * other.y, z * other.z); }
    Vec3 operator/(Vec3 const& other) { return Vec3(x / other.x, y / other.y, z / other.z); }
    Vec3 operator-() { return Vec3(-x, -y, -z); }

};
struct sph2 {
    Vec3 a1, a2;
    float b1, b2;

    sph2(Vec3 a1, float b1, Vec3 a2, float b2) : a1(a1), b1(b1), a2(a2), b2(b2) {};
};
float length(Vec2 const& v) { return sqrt(v.x * v.x + v.y * v.y); }
float length(Vec3 const& v) { return sqrt(v.x * v.x + v.y * v.y + v.z * v.z); }
Vec3 norm(Vec3 v) { return v / length(v); }
float dot(Vec3 const& a, Vec3 const& b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
Vec3 abs(Vec3 const& v) { return Vec3(fabs(v.x), fabs(v.y), fabs(v.z)); }
Vec3 sign(Vec3 const& v) { return Vec3(sign(v.x), sign(v.y), sign(v.z)); }
Vec3 step(Vec3 const& edge, Vec3 v) { return Vec3(step(edge.x, v.x), step(edge.y, v.y), step(edge.z, v.z)); }
Vec3 reflect(Vec3 rd, Vec3 n) { return rd - n * (2 * dot(n, rd)); }
Vec2 sphere(Vec3 const& ro, Vec3 const& rd , float r){
    float b = dot(ro, rd);
    float c = dot(ro, ro) - r * r;
    float h = b * b - c;
    if (h < 0.0) return Vec2(-1.0);
    h = sqrt(h);
    return Vec2(-b - h, -b + h);
}
Vec2 box(Vec3 ro, Vec3 rd, Vec3 boxSize, Vec3& outNormal) {
    Vec3 m = Vec3(1.0) / rd;
    Vec3 n = m * ro;
    Vec3 k = abs(m) * boxSize;
    Vec3 t1 = -n - k;
    Vec3 t2 = -n + k;
    float tN = fmax(fmax(t1.x, t1.y), t1.z);
    float tF = fmin(fmin(t2.x, t2.y), t2.z);
    if (tN > tF || tF < 0.0) return Vec2(-1.0);
    Vec3 yzx = Vec3(t1.y, t1.z, t1.x);
    Vec3 zxy = Vec3(t1.z, t1.x, t1.y);
    outNormal = -sign(rd) * step(yzx, t1) * step(zxy, t1);
    return Vec2(tN, tF);
}
float plane(Vec3 ro, Vec3 rd, Vec3 p, float w) {
    return -(dot(ro, p) + w) / dot(rd, p);
}
Vec3 rotateX(Vec3 a, double angle)
{
    Vec3 b = a;
    b.z = a.z * cos(angle) - a.y * sin(angle);
    b.y = a.z * sin(angle) + a.y * cos(angle);
    return b;
}

Vec3 rotateY(Vec3 a, double angle)
{
    Vec3 b = a;
    b.x = a.x * cos(angle) - a.z * sin(angle);
    b.z = a.x * sin(angle) + a.z * cos(angle);
    return b;
}

Vec3 rotateZ(Vec3 a, double angle)
{
    Vec3 b = a;
    b.x = a.x * cos(angle) - a.y * sin(angle);
    b.y = a.x * sin(angle) + a.y * cos(angle);
    return b;
}
void SetWindow(int Width, int Height)
{
    _COORD coord;
    coord.X = Width;
    coord.Y = Height;
    _SMALL_RECT Rect;
    Rect.Top = 0;
    Rect.Left = 0;
    Rect.Bottom = Height - 1;
    Rect.Right = Width - 1;
    HANDLE Handle = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleScreenBufferSize(Handle, coord);
    SetConsoleWindowInfo(Handle, TRUE, &Rect);
}
void cursora(wchar_t* screen, int width) {
    screen[width * 29 + 120] = '*';
    screen[width * 30 + 120] = '*';
    screen[width * 30 + 121] = '*';
    screen[width * 30 + 119] = '*';
    screen[width * 31 + 120] = '*';
    screen[width * 30 + 122] = '*';
    screen[width * 30 + 118] = '*';
}
const wchar_t* floatToWideChar(float number) {
    wchar_t buffer[256];
    swprintf(buffer, sizeof(buffer), L"%f", number);

    size_t length = wcslen(buffer) + 1;
    wchar_t* result = new wchar_t[length];
    wcscpy_s(result, length, buffer);

    return result;
}
void replaceString(wchar_t* str, size_t startPos, const wchar_t* replacement) {
    size_t len = wcslen(replacement);
    for (size_t i = 0; i < len; i++) {
        str[startPos + i] = replacement[i];
    }
}
void infoa(wchar_t* screen, int width, Vec3 rotation, Vec3 ro) {
    replaceString(screen, 5, floatToWideChar(rotation.z));
    replaceString(screen, 15, floatToWideChar(rotation.y));
    replaceString(screen, 5 + 240, floatToWideChar(ro.x));
    replaceString(screen, 15 + 240, floatToWideChar(ro.y));
    replaceString(screen, 25 + 240, floatToWideChar(ro.z));
}
float distance(const Vec3& vec1, const Vec3& vec2) {
    float deltaX = vec1.x - vec2.x;
    float deltaY = vec1.y - vec2.y;
    float deltaZ = vec1.z - vec2.z;

    return std::sqrt(deltaX * deltaX + deltaY * deltaY + deltaZ * deltaZ);
}

float DotProduct(Vec3 v1, Vec3 v2) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}


Vec3 CrossProduct(Vec3 v1, Vec3 v2) {
    Vec3 result = {
        v1.y * v2.z - v1.z * v2.y,
        v1.z * v2.x - v1.x * v2.z,
        v1.x * v2.y - v1.y * v2.x
    };
    return result;
}
Vec3 Normalize(const Vec3& vec) {
    float length = std::sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
    return { vec.x / length, vec.y / length, vec.z / length };
}

Vec3 CalculateReflection(const Vec3& playerPos, const Vec3& spherePos, float radius) {
    Vec3 displacement = { playerPos.x - spherePos.x, playerPos.y - spherePos.y, playerPos.z - spherePos.z };
    float distance = std::sqrt(displacement.x * displacement.x + displacement.y * displacement.y + displacement.z * displacement.z);

    Vec3 playerToSphere = { spherePos.x - playerPos.x, spherePos.y - playerPos.y, spherePos.z - playerPos.z };
    Vec3 normalizedPlayerToSphere = Normalize(playerToSphere);

    if (distance <= radius) {
        Vec3 reflection = { normalizedPlayerToSphere.x * (radius - distance),
                                normalizedPlayerToSphere.y * (radius - distance),
                                normalizedPlayerToSphere.z * (radius - distance) };
        return reflection;
    }
    else { return Vec3(0); }
}


int main()
{
    sph2 spheres1 = sph2(Vec3(0, 3, 0), 1,  Vec3(0, 1, 0), 1);
    int width = 120*2;
    int height = 30*2;
    SetWindow(width, height);
    float aspect = (float)width / height;
    float aspectp = 11.0f / 24.0f;
    wchar_t* screen = new wchar_t[width * height];
    HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(hConsole);
    DWORD dwBytesWritten = 0;
    char gradient[] = " .:!/r(l1Z4H9W8$@";
    int gradients = std::size(gradient) - 2;
    Vec3 light = norm(Vec3(0, 0, -2));
    Vec3 ro = Vec3(-5, 0, 0);
    Vec3 rotation = Vec3(0, 0.0001, 0.0001);
    float cw = 0;
    float cw2 = 0;
    int cursor = -1;
    int info = -1;
    for (int t = 0; t < 10000; t++) {
        if ((rotation.z > 1.5625) or (rotation.z < -1.5625)) { cw2 = rotation.z / 2; }
        else { cw2 = rotation.z; }
        if (rotation.z < 0) { cw = -rotation.z; }
        else { cw = rotation.z; }
        if (GetKeyState(0x57) & 0x8000) { ro = ro + Vec3(0.02 - (cw / 78.125), 0 + (cw2 / 78.125), 0); }
        if (GetKeyState(0x53) & 0x8000) { ro = ro - Vec3(0.02 - (cw / 78.125), 0 + (cw2 / 78.125), 0); }
        if (GetKeyState(0x41) & 0x8000) { ro = ro - Vec3(0 - (cw2 / 78.125), 0.02 - (cw / 78.125), 0); }
        if (GetKeyState(0x44) & 0x8000) { ro = ro + Vec3(0 - (cw2 / 78.125), 0.02 - (cw / 78.125), 0); }
        if (GetKeyState(0x51) & 0x8000) { ro = ro + Vec3(0, 0, 0.02); }
        if (GetKeyState(0x45) & 0x8000) { ro = ro - Vec3(0, 0, 0.02); }
        if (GetKeyState(0x55) & 0x8000) { rotation.y -= 0.02f; }
        if (GetKeyState(0x4A) & 0x8000) { rotation.y += 0.02f; }
        if (GetKeyState(0x48) & 0x8000) { rotation.z -= 0.02f; }
        if (GetKeyState(0x4B) & 0x8000) { rotation.z += 0.02f; }
        if ((GetKeyState(0x52) & 0x8000) and (GetKeyState(0x5A) & 0x8000) and (GetKeyState(0x09) & 0x8000)) { rotation = Vec3(0, 0.0001, 0.0001); ro = Vec3(-5, 0, 0); Sleep(500); }
        if ((GetKeyState(0x4B) & 0x8000) and (GetKeyState(0x5A) & 0x8000) and (GetKeyState(0x09) & 0x8000)) { cursor = -cursor; Sleep(500); }
        if ((GetKeyState(0x49) & 0x8000) and (GetKeyState(0x5A) & 0x8000) and (GetKeyState(0x09) & 0x8000)) { info = -info; Sleep(500); }
        if ((rotation.z > 3.125)) { rotation = Vec3(0, rotation.y, -3.12); }
        if ((rotation.z < -3.125)) { rotation = Vec3(0, rotation.y, 3.12); }
        for (int i = 0; i < width; i++)
        {
            for (int j = 0; j < height; j++)
            {
                Vec2 uv = Vec2(i, j) / Vec2(width, height) * 2.0f - 1.0f;
                uv.x *= aspect * aspectp;
                Vec3 rd = norm(Vec3(1, uv));
                rd = rotateY(rd, rotation.y);
                rd = rotateZ(rd, rotation.z);
                float diff = 1;
                Vec3 ro1 = ro;
                for (int k = 0; k < 15; k++) {
                    float minIt = 99999;
                    Vec3 n = 0;
                    float albedo = 1;
                    Vec3 spherePos = Vec3(0);
                    Vec3 np = Vec3(0);
                    float sphereRad = 0;
                    for (int w = 1; w < 3; w++) 
                    {
                        if (w == 1) { spherePos = spheres1.a1; sphereRad = spheres1.b1; }
                        if (w == 2) { spherePos = spheres1.a2; sphereRad = spheres1.b2; }
                        Vec2 intersection = sphere(ro1 - spherePos, rd, sphereRad);
                        if (intersection.x > 0) {
                            Vec3 itPoint = ro1 - spherePos + rd * intersection.x;
                            minIt = intersection.x;
                            n = norm(itPoint);
                        }
                        np = np + CalculateReflection(ro, spherePos, sphereRad+0.01);
                    }
                    if (minIt < 99999) {
                        diff *= (dot(n, light) * 0.5 + 0.5) * albedo;
                        ro1 = ro1 + rd * (minIt - 0.01);
                        rd = reflect(rd, n);
                    }
                    else break;
                    ro = ro - np;
                }
                int color = (int)(diff * 20);
                if (color-3 > gradients) color = 0;
                color = clamp(color, 0, gradients);
                char pixel = gradient[color];
                screen[i + j * width] = pixel;
            }
        }
        screen[width * height - 1] = '\0';
        if (cursor == 1) { cursora(screen, width); }
        if (info == 1) { infoa(screen, width, rotation, ro); }
        WriteConsoleOutputCharacter(hConsole, screen, width * height, { 0, 0 }, &dwBytesWritten);
    }
    getchar();
}
