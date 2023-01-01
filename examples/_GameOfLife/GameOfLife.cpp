#ifdef UNITY_BUILD
#include "../../src/unity.cpp"
#endif

const int SizeX = 86;
const int SizeY = 64;
const int MapLength = SizeX * SizeY;

int seed = 1548;

int GetCell(int* map, int x, int y) {
    assert(x >= 0 && x < SizeX && y >= 0 && y < SizeY);

    return map[y * SizeX + x];
}

int GetNeighboursCount(int* map, int x, int y) {
    int count = 0;

    for(int nX = x - 1; nX <= x + 1; nX++) 
    for(int nY = y - 1; nY <= y + 1; nY++) {
        if(nX == x && nY == y) {
            continue;
        }

        // wrap cell coords:
        int iX = nX < 0      ? SizeX + nX :
                 nX >= SizeX ? nX % SizeX :
                               nX;

        int iY = nY < 0      ? SizeY + nY :
                 nY >= SizeY ? nY % SizeY :
                               nY;

        count += GetCell(map, iX, iY);
    }


    return count;
}

void GameOfLife(int* map, int* backbuffer) {
    for(int x = 0; x < SizeX; x++) 
    for(int y = 0; y < SizeY; y++) {
        int neighbours = GetNeighboursCount(map, x, y);

        int index = y * SizeX + x;
        if(map[index] == 1 && (neighbours == 2 || neighbours == 3)) {
            // Cell survives
            backbuffer[index] = map[index];
        }
        else if(map[index] == 0 && neighbours == 3) {
            // Cell become alive
            backbuffer[index] = 1;
        }
        else {
            // Cell dies, or stays dead
            backbuffer[index] = 0;
        }
    }
}


int main() {
    SRWindow* window = InitializeWindow(Str8Lit("Cellular Automaton"), SizeX * 10, SizeY * 10);

    Mesh quad = CreateQuadMesh(&window->persistentArena);

    Camera camera = CreateOrtographic(SizeY / 2.0f, 0.001f, 100.0f, (float) window->width / window->height);
    camera.position.z = -5;
    camera.position.x = SizeX / 2.0f - 0.5f;
    camera.position.y = SizeY / 2.0f - 0.5f;

    camera.rotation.y = 3.14f / 2;

    int* map        = (int*) PushArena(&window->persistentArena, MapLength * sizeof(int));
    int* backbuffer = (int*) PushArena(&window->persistentArena, MapLength * sizeof(int));

    srand(seed);
    for(int i = 0; i < MapLength; i++) {
        map[i] = rand() % 100  < 20;
    }

    UseShader(window, ColorShader);

    float timer = 0;

    while(ShouldClose(window) == false) {
        FrameStart(window);

        ClearColorBuffer({0.8f, 0.8f, 0.8f, 0.f});

        if(timer >= 0.1f) {
            timer = 0;

            GameOfLife(map, backbuffer);

            int* tmp = map;
            map = backbuffer;
            backbuffer = tmp;
        }

        timer += window->timeDelta;

        Matrix vp = GetVPMatrix(&camera);
        for(int i = 0; i < MapLength; i++) {
            float x = i % SizeX;
            float y = i / SizeX;

            SetShaderUniform(window, "tint", map[i] == 0 ? Vector4{0, 0, 0, 1} : Vector4{1, 1, 1, 1});

            DrawMesh(window, quad, vp * MatrixTranslate(x, y, 0));
        }

        FrameEnd(window);
    }

    return 0;
}