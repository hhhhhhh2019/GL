#include "bitmap_image.hpp"
#include "vector.cpp"

struct Renderer {
	private:
		bitmap_image image;
		float *zBuffer;
		unsigned int width;
		unsigned int height;
		vec3f *vertexes;

		bitmap_image *texture;
		vec2i *texCoord;

		mat4 camMat;
		mat4 projMat;
	
	public:
		Renderer(unsigned int _width, unsigned int _height, vec3f *vert) {
			width = _width;
			height = _height;
			image = bitmap_image(width, height);
			zBuffer = new float[width * height];
			for (int i = 0; i < width * height; i++) zBuffer[i] = std::numeric_limits<float>::max();

			vertexes = vert;
		}

		void setTexture(bitmap_image *tex, vec2i *tc) {
			texture = tex;
			texCoord = tc;
		}

		void setCamera(vec3f p, vec3f bx, vec3f by, vec3f bz, float n, float f, float fov) {
			camMat = mat4(
				1, 0, 0, -p.x,
				0, 1, 0, -p.y,
				0, 0, 1, -p.z,
				0, 0, 0, 1
			) * mat4(
				bx.x, bx.y, bx.z, 0, 
				by.x, by.y, by.z, 0,
				bz.x, bz.y, bz.z, 0,
				0, 0, 0, 1
			);

			float hfov = fov;
			float vfov = hfov * (height / width);

			float right = tan(hfov / 2);
			float top = tan(vfov / 2);

			float m00 = 2 / (right * 2);
        	float m11 = 2 / (top * 2);
        	float m22 = (f + n) / (f - n);
        	float m32 = -2 * n * f / (f - n);
        	projMat = mat4(
				m00, 0, 0, 0,
				0, -m11, 0, 0,
				0, 0, m22, 1,
				0, 0, m32, 0
			);
		}

		void point(float x, float y, rgb_t color) {
			if (x < 0 || x >= width || y < 0 || y >= height) return;
			image.set_pixel(floor(x), floor(y), color.red, color.green, color.blue);
		}

		vec3f prepeareVert(vec3f v) {
			vec4f nv = vec4f(v.x, v.y, v.z, 1) * camMat * projMat;
			return vec3f(((nv.x / nv.w + 1) / 2) * width, ((nv.y / nv.w + 1) / 2) * height, nv.z / nv.w);
		}

		void triangle(vec3f a, vec3f b, vec3f c, vec2i uva, vec2i uvb, vec2i uvc) {
			if (a.y > b.y) {std::swap(a, b); std::swap(uva, uvb);}
			if (a.y > c.y) {std::swap(a, c); std::swap(uva, uvc);}
			if (b.y > c.y) {std::swap(b, c); std::swap(uvb, uvc);}

			float height = c.y - a.y;
			for  (float y = 0; y < height; y++) {
				bool secSeg = y >= b.y - a.y || b.y == a.y;
				float segH = secSeg ? c.y - b.y : b.y - a.y;
				float alpha = y / height;
				float beta = (y - (secSeg ? b.y - a.y : 0.f)) / segH;
				vec3f A = a + (c - a) * alpha;
				vec3f B = secSeg ? b + (c - b) * beta : a + (b - a) * beta;
				vec2i uvA = uva + (uvc - uva) * alpha;
				vec2i uvB = secSeg ? uvb + (uvc - uvb) * beta : uva + (uvb - uva) * beta;
				if (A.x > B.x) {std::swap(A, B); std::swap(uvA, uvB);}
				for (float x = A.x; x < B.x; x++) {
					float phi = A.x == B.x ? 1 : (x - A.x) / (B.x - A.x);
					vec3f P = A + (B - A) * phi;
					vec2i uvP = uvA + (uvB - uvA) * phi;
					point(P.x, P.y, texture->get_pixel(uvP.x, uvP.y));
				}
			}
		}

		void render(int elemCount) {
			vec3f a, b, c;

			for (int i = 0; i < elemCount - 2; i++) {
				a = prepeareVert(vertexes[i]);
				b = prepeareVert(vertexes[i+1]);
				c = prepeareVert(vertexes[i+2]);
				triangle(a, b, c, texCoord[i], texCoord[i+1], texCoord[i+2]);
			}
		}

		void save(std::string filename) {
			image.save_image(filename);
		}
};