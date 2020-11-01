#include <iostream>
#include <fstream>
#include "sphere.h"
#include "hitable_list.h"
#include "float.h"
#include "camera.h"
#include "material.h"
#include "random.h"
#include "time.h"
#include "moving_sphere.h"
#include "bvh.h"
#include "texture.h"
#include "surface_texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "aarect.h"
#include "box.h"

using namespace std;

vec3 color(const ray& r, hitable *world, int depth){
	hit_record rec;
	if(world->hit(r, 0.001, FLT_MAX, rec)){
		ray scattered;
		vec3 attenuation;
		vec3 emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);
		if(depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)){
			return emitted + attenuation*color(scattered, world, depth+1);
		}else{
			return emitted;
		}
	}else{
		return vec3(0, 0, 0);
	}
}


hitable *earth() {
    int nx, ny, nn;
    unsigned char *tex_data = stbi_load("earthMap.jpg", &nx, &ny, &nn, 0);
    material *mat =  new lambertian(new image_texture(tex_data, nx, ny));
    return new sphere(vec3(0, 0, 0), 2, mat);
}

hitable *random_scene() {
    int n = 500;
    hitable **list = new hitable*[n+1];
    // texture *checker = new checker_texture( new constant_texture(vec3(0.25,0.35, 0.4)), new constant_texture(vec3(0.89, 0.89, 0.9)));
    list[0] =  new sphere(vec3(0,-1000,0), 1000, new lambertian(new constant_texture(vec3(0.5, 0.5, 0.5))));
    int i = 1;
    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            float choose_mat = random_double();
            vec3 center(a+0.9*random_double(),0.2,b+0.9*random_double());
            if ((center-vec3(4,0.2,0)).length() > 0.9) {
                if (choose_mat < 0.8) {  // diffuse
                    list[i++] = new moving_sphere(center, center+vec3(0, 0.5*random_double(), 0),
                    							  0.0, 1.0, 0.2,
                    							  new lambertian(new constant_texture(vec3(
                    							  	random_double()*random_double(),
                    							  	random_double()*random_double(),
                    							  	random_double()*random_double())))
                    							  );
                }
                else if (choose_mat < 0.95) { // metal
                    list[i++] = new sphere(
                        center, 0.2,
                        new metal(vec3(0.5*(1 + random_double()),
                                       0.5*(1 + random_double()),
                                       0.5*(1 + random_double())),
                                  0.5*random_double())
                    );
                }
                else {  // glass
                    list[i++] = new sphere(center, 0.2, new dielectric(1.5));
                }
            }
        }
    }

    list[i++] = new sphere(vec3(0, 1, 0), 1.0, new dielectric(1.5));
    list[i++] = new sphere(vec3(-4, 1, 0), 1.0, new lambertian(new constant_texture(vec3(0.4, 0.2, 0.1))));
    list[i++] = new sphere(vec3(4, 1, 0), 1.0, new metal(vec3(0.7, 0.6, 0.5), 0.0));

    // return new hitable_list(list,i);
    return new bvh_node(list, i, 0.0, 1.0);
}

hitable* two_perlin_spheres(){
	texture* pertext = new noise_texture(4);
	hitable** list = new hitable*[2];
	list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(pertext));
	list[1] = new sphere(vec3(0, 2, 0), 2, new lambertian(pertext));
	return new hitable_list(list, 2);
}

hitable *simple_light() {
    texture *pertext = new noise_texture(4);
    hitable **list = new hitable*[4];
    list[0] =  new sphere(vec3(0,-1000, 0), 1000, new lambertian( pertext ));
    list[1] =  new sphere(vec3(0, 2, 0), 2, new lambertian( pertext ));
    list[2] =  new sphere(vec3(0, 7, 0), 2, new diffuse_light( new constant_texture(vec3(4,4,4))));
    list[3] =  new xy_rect(3, 5, 1, 3, -2, new diffuse_light(new constant_texture(vec3(4,4,4))));
    return new hitable_list(list,4);
}

hitable *cornell_box() {
    hitable **list = new hitable*[8];
    int i = 0;
    material *red = new lambertian( new constant_texture(vec3(0.65, 0.05, 0.05)) );
    material *white = new lambertian( new constant_texture(vec3(0.73, 0.73, 0.73)) );
    material *green = new lambertian( new constant_texture(vec3(0.12, 0.45, 0.15)) );
    material *light = new diffuse_light( new constant_texture(vec3(15, 15, 15)) );
    list[i++] = new flip_normals(new yz_rect(0, 555, 0, 555, 555, green));
    list[i++] = new yz_rect(0, 555, 0, 555, 0, red);
    list[i++] = new xz_rect(213, 343, 227, 332, 554, light);
    list[i++] = new flip_normals(new xz_rect(0, 555, 0, 555, 555, white));
    list[i++] = new xz_rect(0, 555, 0, 555, 0, white);
    list[i++] = new flip_normals(new xy_rect(0, 555, 0, 555, 555, white));
    list[i++] = new translate(new rotate_y(new box(vec3(0, 0, 0), vec3(165, 165, 165), white), -18), vec3(130,0,65));
    list[i++] = new translate(new rotate_y(new box(vec3(0, 0, 0), vec3(165, 330, 165), white),  15), vec3(265,0,295));
    // list[i++] = new box(vec3(130, 0, 65), vec3(295, 165, 230), white);
    // list[i++] = new box(vec3(265, 0, 295), vec3(430, 330, 460), white);
    return new hitable_list(list,i);
}

int main(){
	int nx = 1200;
	int ny = 800;
	int ns = 100;
	clock_t start, end;
	ofstream outfile;
	outfile.open("pic/rayTracingRectangesAndLights4.ppm");

	outfile << "P3\n" << nx << " " << ny << "\n255\n";
	hitable *world = cornell_box();
	
	// vec3 lookfrom(13, 2, 3);
	// vec3 lookat(0, 0, 0);
	// float dist_to_focus = 10.0;
	// float aperture = 0.0;
	// camera cam(lookfrom, lookat, vec3(0, 1, 0), 20, float(nx)/float(ny), aperture, dist_to_focus, 0.0, 1.0);
	vec3 lookfrom(278, 278, -800);
	vec3 lookat(278, 278, 0);
	float dist_to_focus = 10.0;
	float aperture = 0.0;
	float vfov = 40.0;
	camera cam(lookfrom, lookat, vec3(0, 1, 0), vfov, float(nx)/float(ny), aperture, dist_to_focus, 0.0, 1.0);

	int temp = 0, prev = 0;
	start = clock();
	for(int j = ny-1; j >= 0; --j){
		for(int i = 0; i < nx; ++i){
			vec3 col(0, 0, 0);
			for(int s = 0; s < ns; ++s){
				float u = float(i + random_double()) / float(nx);
				float v = float(j + random_double()) / float(ny);
				ray r = cam.get_ray(u, v);
				col += color(r, world, 0);
			}
			col /= float(ns);
			col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));
			int ir = int(255.99*col[0]);
			int ig = int(255.99*col[1]);
			int ib = int(255.99*col[2]);	
			outfile << ir << " " << ig << " " << ib << "\n";
		}
		temp = (ny-j)*100/ny;
		end = clock();
		if(temp != prev){
			cout << temp << "%\t" << (float)(end-start)*1.0/CLOCKS_PER_SEC << "s" << endl;
			fflush(stdout);
			prev = temp;
		}
	}
}