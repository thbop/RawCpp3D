#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>


static const int
    img_width = 256,
    img_height = 256,
    clipFar = 1000;

float min(float a, float b) {
    if (a < b) {
        return a;
    } else {
        return b;
    }
}
float max(float a, float b) {
    if (a > b) {
        return a;
    } else {
        return b;
    }
}


struct Vector2 {
    float x, y;
    Vector2(float X=0, float Y=0) {
        x = X;
        y = Y;
    }
    void print() {
        std::cout << "Vector2(" << x << ", " << y << ")\n";
    }

    float magnitude() {
        return sqrt( pow(x, 2) + pow(y, 2) );
    }
    Vector2 normalize() {
        float mag = magnitude();
        return Vector2(
            x / mag,
            y / mag
        );
    }

    Vector2 operator+( Vector2 other ) {
        return Vector2( x + other.x, y + other.y );
    }
    Vector2 operator-( Vector2 other ) {
        return Vector2( x - other.x, y - other.y );
    }
    Vector2 operator*( Vector2 other ) {
        return Vector2( x * other.x, y * other.y );
    }
    Vector2 operator/( Vector2 other ) {
        return Vector2( x / other.x, y / other.y );
    }
};


struct Edge2D {
    Vector2 a, b;
    // Ensure a is below b
    Edge2D(Vector2 A, Vector2 B) {
        if (A.y < B.y) {
            a = A;
            b = B;
        } else {
            a = B;
            b = A;
        }
    }
};

struct Span2D {
    int x1, x2;
    // Ensure x1 < x2
    Span2D(int X1, int X2) {
        if (X1 < X2) {
            x1 = X1;
            x2 = X2;
        } else {
            x1 = X2;
            x2 = X1;
        }
    }
};

struct Vector3 {
    float x, y, z;
    Vector3(float X=0, float Y=0, float Z=0) {
        x = X;
        y = Y;
        z = Z;
    }
    void print() {
        std::cout << "Vector3(" << x << ", " << y << ", " << z << ")\n";
    }

    float magnitude() {
        return sqrt( pow(x, 2) + pow(y, 2) + pow(z, 2) );
    }
    Vector3 normalize() {
        float mag = magnitude();
        return Vector3(
            x / mag,
            y / mag,
            z / mag
        );
    }

    Vector3 normal_to_color() {
        return Vector3(
            // (int)(x * 255),
            // (int)(y * 255),
            // (int)(z * 255)
            (int)((x + 1)/2 * 255),
            (int)((y + 1)/2 * 255),
            (int)((z + 1)/2 * 255)
        );
    }

    Vector2 ignoreZ() {
        return Vector2(x, y);
    }

    float dot( Vector3 other ) {
        return x * other.x + y * other.y + z * other.z;
    }

    Vector3 cross( Vector3 other ) {
        return Vector3(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }

    Vector3 operator+( Vector3 other ) {
        return Vector3( x + other.x, y + other.y, z + other.z );
    }
    Vector3 operator-( Vector3 other ) {
        return Vector3( x - other.x, y - other.y, z - other.z );
    }
    Vector3 operator*( Vector3 other ) {
        return Vector3( x * other.x, y * other.y, z * other.z );
    }
    Vector3 operator/( Vector3 other ) {
        return Vector3( x / other.x, y / other.y, z / other.z );
    }
};



struct Triangle {
    Vector3 a, b, c;
    Triangle() {
        a = Vector3();
        b = Vector3();
        c = Vector3();
    }
    Triangle(Vector3 A, Vector3 B, Vector3 C) {
        a = A;
        b = B;
        c = C;
    }

    Triangle operator+( Vector3 other ) {
        return Triangle( a + other, b + other, c + other );
    }
    Triangle operator*( Vector3 other ) {
        return Triangle( a * other, b * other, c * other );
    }

};

struct LinearPlane3D {
    Vector3 point, normal;
    LinearPlane3D(Triangle t) {
        normal = (t.b - t.a).cross( (t.c - t.a) ).normalize();
        point = t.a;
    }

    int z(int x, int y) { // Returns the z value that lies on the plane given the x y coordinates.
        return (( -normal.x * ( x - point.x ) - normal.y * ( y - point.y ) ) / normal.z) + point.z;
    }
};

struct Renderer {
    static const int width = 256, height = 256;
    Vector2 shift = Vector2( width / 2, height / 2 );

    int pixels[height][width][3] = {};

    int zBuffer[height][width] = {};

    void Initialize(const int clipFar) {
        for (int y = 0; y < img_height; y++) {
            for (int x = 0; x < img_width; x++) {
                zBuffer[y][x] = clipFar;
            }
        }
    }

    void SetPixel(Vector2 pos, Vector3 color) { // Pass vec3 instead
        int x = (int)pos.x, y = (int)pos.y;

        pixels[y][x][0] = color.x;
        pixels[y][x][1] = color.y;
        pixels[y][x][2] = color.z;
    }

    void Set3DPixel(Vector3 pos, Vector3 color) {
        int x = (int)pos.x, y = (int)pos.y;
        x += shift.x;
        y += shift.y;
        if (x > 0 and x < width and y > 0 and y < height) {
            if (zBuffer[y][x] > pos.z) {
                zBuffer[y][x] = (int)pos.z;

                pixels[y][x][0] = color.x;
                pixels[y][x][1] = color.y;
                pixels[y][x][2] = color.z;

            }
        }
    }

    void DrawSpan(const Span2D &span, int y, Vector3 color, LinearPlane3D plane ) {
        for (int x = span.x1; x < span.x2; x++) {
            Set3DPixel(Vector3(x, y, plane.z(x, y)), color); // for normals plane.normal.normal_to_color()
        }
    }

    void DrawSpansBetweenEdges(const Edge2D &e1, const Edge2D &e2, Vector3 color, LinearPlane3D plane) {
        // Calculate the difference between the y coordinates of the two edges.
        float e1ydiff = e1.b.y - e1.a.y;
        if (e1ydiff == 0.0f)
            return;

        float e2ydiff = e2.b.y - e2.a.y;
        if (e2ydiff == 0.0f)
            return;
        
        float e1xdiff = e1.b.x - e1.a.x;
        float e2xdiff = e2.b.x - e2.a.x;

        // Interpolation factors
        float factor1 = (e2.a.y - e1.a.y) / e1ydiff;
        float factorStep1 = 1.0f / e1ydiff;
        float factor2 = 0.0f;
        float factorStep2 = 1.0f / e2ydiff;

        // Loop through the lines between the edges and draw spans
        for (int y = e2.a.y; y < e2.b.y; y++) {
            Span2D span = Span2D(
                e1.a.x + (int)(e1xdiff * factor1),
                e2.a.x + (int)(e2xdiff * factor2)
            );
            DrawSpan(span, y, color, plane);

            // increment factors
            factor1 += factorStep1;
            factor2 += factorStep2;
        }
        
    }

    void RasterizeTriangle(Triangle t) {
        // Based on: https://joshbeam.com/articles/triangle_rasterization/
        // Form edges
        Edge2D edges[3] = {
            Edge2D(t.a.ignoreZ(), t.b.ignoreZ()),
            Edge2D(t.b.ignoreZ(), t.c.ignoreZ()),
            Edge2D(t.c.ignoreZ(), t.a.ignoreZ())
        };

        int maxLength = 0;
        int longEdge = 0;

        // Find the edge with the greatest length in the y axis
        for (int i = 0; i < 3; i++) {
            int length = edges[i].b.y - edges[i].a.y;
            if (length > maxLength) {
                maxLength = length;
                longEdge = i;
            }
        }

        int shortEdge1 = (longEdge + 1) % 3;
        int shortEdge2 = (longEdge + 2) % 3;

        // Init plane for zBuffer stuff
        LinearPlane3D plane = LinearPlane3D(t);

        Vector3 light = Vector3(0, 60, -100);
        float light_intensity = 200;
        
        Vector3 light_dir = (light - t.a).normalize(); // Not totally accurate

        float v = max(plane.normal.dot(light_dir), 0) * light_intensity + 55;
        Vector3 color = Vector3(v, v, v);

        DrawSpansBetweenEdges(edges[longEdge], edges[shortEdge1], color, plane);
        DrawSpansBetweenEdges(edges[longEdge], edges[shortEdge2], color, plane);
    }


    void Save() {
        std::ofstream imgFile("output.ppm");
        imgFile << "P3\n" << width << " " << height << "\n255\n";

        for ( int y = height-1; y >= 0; y-- ) {
            for ( int x = 0; x < width; x++ ) {
                for ( int i = 0; i < 3; i++ ) {
                    imgFile << pixels[y][x][i] << " ";
                }
            }
            imgFile << "\n";
        }

        imgFile.close();
    }
    void ViewZBuffer() {
        std::ofstream imgFile("zbuffer.ppm");
        imgFile << "P2\n" << width << " " << height << "\n100\n";

        for ( int y = height-1; y >= 0; y-- ) {
            for ( int x = 0; x < width; x++ ) {
                imgFile << zBuffer[y][x] << " ";
            }
            imgFile << "\n";
        }

        imgFile.close();
    }
};

struct Camera {
    Vector3 c = Vector3(0, 0, 0); // Camera origin in camera space
    Vector3 rot = Vector3(0, 0, 0); // Camera rotation in world space
    Vector3 e = Vector3(0, 0, 64); // Display surface / Image plane

    // Vector3 toCameraSpace(Vector3 a) {
    //     ...
    //     return ...
    // }

    Vector3 toImagePlane(Vector3 d) {
        // https://en.wikipedia.org/wiki/3D_projection
        float z = (e.z / d.z);
        return Vector3(
            z * d.x + e.x,
            z * d.y + e.y,
            d.z - e.z // Should be the z-depth of that point.
        );
    }
    Triangle TriToImagePlane(Triangle d) {
        return Triangle(
            toImagePlane(d.a),
            toImagePlane(d.b),
            toImagePlane(d.c)
        );
    }
    
};

struct Object {
    std::vector<Triangle> triangles;

    Object(std::vector<Triangle> Triangles) {
        triangles = Triangles;
    }
    void toWorldSpace(Vector3 pos, Vector3 scale) {
        for (int i = 0; i < triangles.size(); i++) {
            // Quick transform cross to world space
            triangles[i] = triangles[i] * pos;
            triangles[i] = triangles[i] + scale;
        }
    }

    void toDisplayBuffer(std::vector<Triangle> &triangleDisplayBuffer, Camera &camera) {
        for (int i = 0; i < triangles.size(); i++) {
            triangleDisplayBuffer.push_back(
                camera.TriToImagePlane(triangles[i])
            );
        }
    }
};



Object cube = Object({
    Triangle( // Back face
        Vector3(-1, 1, 1),
        Vector3(-1, -1, 1),
        Vector3(1, -1, 1)
    ),
    Triangle(
        Vector3(1, -1, 1),
        Vector3(1, 1, 1),
        Vector3(-1, 1, 1)
    ),
    Triangle( // Front face
        Vector3(1, -1, -1),
        Vector3(-1, -1, -1),
        Vector3(-1, 1, -1)
    ),
    Triangle(
        Vector3(-1, 1, -1),
        Vector3(1, 1, -1),
        Vector3(1, -1, -1)
    ),

    Triangle( // Right face
        Vector3(1, -1, 1),
        Vector3(1, -1, -1),
        Vector3(1, 1, -1)
    ),
    Triangle(
        Vector3(1, 1, -1),
        Vector3(1, 1, 1),
        Vector3(1, -1, 1)
    ),
    Triangle( // Left face
        Vector3(-1, -1, 1),
        Vector3(-1, 1, 1),
        Vector3(-1, 1, -1)
    ),
    Triangle(
        Vector3(-1, 1, -1),
        Vector3(-1, -1, -1),
        Vector3(-1, -1, 1)
    ),

    Triangle( // Bottom face
        Vector3(1, -1, 1),
        Vector3(-1, -1, 1),
        Vector3(-1, -1, -1)
    ),
    Triangle(
        Vector3(-1, -1, -1),
        Vector3(1, -1, -1),
        Vector3(1, -1, 1)
    ),
    Triangle( // Top face
        Vector3(1, 1, 1),
        Vector3(1, 1, -1),
        Vector3(-1, 1, -1)
    ),
    Triangle(
        Vector3(-1, 1, -1),
        Vector3(-1, 1, 1),
        Vector3(1, 1, 1)
    ),
});

Object cross = Object({ // Two planes intersecting each other
    Triangle(
        Vector3(0, -1, 1),
        Vector3(0, -1, -1),
        Vector3(0, 1, -1)
    ),
    Triangle(
        Vector3(0, 1, -1),
        Vector3(0, 1, 1),
        Vector3(0, -1, 1)
    ),

    Triangle(
        Vector3(-1, -1, 0),
        Vector3(-1, 1, 0),
        Vector3(1, 1, 0)
    ),
    Triangle(
        Vector3(1, 1, 0),
        Vector3(1, -1, 0),
        Vector3(-1, -1, 0)
    )
});

int main() {
    Renderer renderer;
    Camera camera;

    renderer.Initialize(clipFar);

    std::vector<Triangle> triangleDisplayBuffer; // Triangles on the display surface also storing z-depth.


    cross.toWorldSpace(Vector3(32, 32, 32), Vector3(-60, 60, 100));
    cross.toDisplayBuffer(triangleDisplayBuffer, camera);

    cube.toWorldSpace(Vector3(32, 32, 32), Vector3(60, -60, 100));
    cube.toDisplayBuffer(triangleDisplayBuffer, camera);

    for ( int i = 0; i < triangleDisplayBuffer.size(); i++ ) {
        renderer.RasterizeTriangle(triangleDisplayBuffer[i]);
    }



    renderer.Save();
    renderer.ViewZBuffer();

    std::cout << "Success! Image updated.\n";


    return 0;
}