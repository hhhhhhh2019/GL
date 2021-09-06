#include "renderer.cpp"

unsigned int width = 200;
unsigned int height = 200;

vec3f vert[] = {
	vec3f(-1, -1, 0),
	vec3f(-1, 1, 0),
	vec3f(1, -1, 0),
	vec3f(1, 1, 0)
};

vec2i texC[] = {
	vec2i(0, 0),
	vec2i(0, 225),
	vec2i(225, 0),
	vec2i(225, 225)
};

Renderer renderer(width, height, vert);

bitmap_image texture("texture.bmp");

mat3 rotY = mat3(
	cos(0.5), 0, -sin(0.5),
	0, 1, 0,
	sin(0.5), 0, cos(0.5)
);

vec3f pos = vec3f(0.5,0,-1.5);
vec3f bx = vec3f(1,0,0) * rotY;
vec3f by = vec3f(0,1,0);
vec3f bz = vec3f(0,0,1) * rotY;

int main() {
	renderer.setCamera(pos, bx, by, bz, 0.5, 100, M_PI / 2.f);

	renderer.setTexture(&texture, texC);

	renderer.render(4);
	renderer.save("output.bmp");
	return 0;
}