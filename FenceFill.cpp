#include <GL/glut.h>
#include <vector>
#include <iostream>
#include <algorithm>

using namespace std;

const int WIDTH = 600, HEIGHT = 600;
vector<pair<int, int>> vertices;
bool polygonDrawn = false;

// Edge table entry
struct Edge {
    int yMax;
    float xMin, slopeInverse;
};

// Function to put a pixel
void putPixel(int x, int y, float fillColor[3]) {
    glColor3f(fillColor[0], fillColor[1], fillColor[2]);
    glBegin(GL_POINTS);
    glVertex2i(x, y);
    glEnd();
    glFlush();
}

// Function to fill the polygon using Fence Fill Algorithm
void fenceFill() {
    if (vertices.size() < 3) return;

    vector<vector<Edge>> edgeTable(HEIGHT);

    // Build edge table
    for (size_t i = 0; i < vertices.size(); i++) {
        pair<int, int> v1 = vertices[i];
        pair<int, int> v2 = vertices[(i + 1) % vertices.size()];

        if (v1.second == v2.second) continue; // Ignore horizontal edges

        if (v1.second > v2.second) swap(v1, v2);

        Edge e;
        e.yMax = v2.second;
        e.xMin = v1.first;
        e.slopeInverse = (float)(v2.first - v1.first) / (v2.second - v1.second);
        edgeTable[v1.second].push_back(e);
    }

    // Active edge table
    vector<Edge> activeEdges;

    for (int y = 0; y < HEIGHT; y++) {
        // Add edges from edge table
        for (auto &e : edgeTable[y]) {
            activeEdges.push_back(e);
        }

        // Remove edges where yMax is reached
        activeEdges.erase(remove_if(activeEdges.begin(), activeEdges.end(), [y](Edge e) { return e.yMax == y; }), activeEdges.end());

        // Sort active edges by xMin
        sort(activeEdges.begin(), activeEdges.end(), [](Edge a, Edge b) { return a.xMin < b.xMin; });

        // Fence fill effect: Fill alternate spans
        float fillColor[] = {1.0, 1.0, 0.0};
        bool fill = true;
        for (size_t i = 0; i + 1 < activeEdges.size(); i += 2) {
            if (fill) {
                int startX = (int)activeEdges[i].xMin;
                int endX = (int)activeEdges[i + 1].xMin;
                for (int x = startX; x <= endX; x++) {
                    putPixel(x, y, fillColor);
                }
            }
            fill = !fill; // Toggle filling on and off
        }

        // Update xMin values for next scanline
        for (auto &e : activeEdges) {
            e.xMin += e.slopeInverse;
        }
    }
}

// Draw polygon
void drawPolygon() {
    if (vertices.size() < 2) return;
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_LINE_LOOP);
    for (auto v : vertices) {
        glVertex2i(v.first, v.second);
    }
    glEnd();
    glFlush();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    drawPolygon();
}

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && !polygonDrawn) {
        vertices.push_back({x, HEIGHT - y});
        glPointSize(5.0);
        glColor3f(1.0, 1.0, 1.0);
        glBegin(GL_POINTS);
        glVertex2i(x, HEIGHT - y);
        glEnd();
        glFlush();
    }
    else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN && !polygonDrawn) {
        polygonDrawn = true;
        glutPostRedisplay();
    }
    else if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && polygonDrawn) {
        fenceFill();
    }
}

void init() {
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WIDTH, 0, HEIGHT);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("Fence Fill Algorithm");

    init();
    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutMainLoop();
    return 0;
}

