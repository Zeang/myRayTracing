#include <iostream>
#include <fstream>
#include <random>
#include "sphere.h"
#include "hitable_list.h"
#include "float.h"
#include "camera.h"

using namespace std;

vec3 random_in_unit_sphere()
{
    vec3 p;
	static default_random_engine e;
	static long count = 0;
	uniform_real_distribution<float> ud(0.0, 1.0);

    do{
        p = 2.0*vec3(ud(e), ud(e), ud(e)) - vec3(1,1,1);
    }while (dot(p,p) >= 1.0);
    return p;
}

vec3 color(const ray& r, hitable *world){
	hit_record rec;
	if(world->hit(r, 0.001, FLT_MAX, rec)){
		vec3 target = rec.p + rec.normal + random_in_unit_sphere();
		return 0.5 * color(ray(rec.p, target - rec.p), world);
	}else{
		vec3 unit_direction = unit_vector(r.direction());
		float t = 0.5 * (unit_direction.y() + 1.0);
		return (1.0-t)*vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
	}
}

int main(){
	default_random_engine e;
	uniform_real_distribution<float> ud(0.0, 1.0);
	int nx = 200;
	int ny = 100;
	int ns = 100;
	ofstream outfile;
	outfile.open("pic/rayHit.ppm");

	outfile << "P3\n" << nx << " " << ny << "\n255\n";
	
	hitable *list[2];
	list[0] = new sphere(vec3(0, 0, -1), 0.5);
	list[1] = new sphere(vec3(0, -100.5, -1), 100);
	hitable * world = new hitable_list(list, 2);
	camera cam;
	for(int j = ny-1; j >= 0; --j){
		for(int i = 0; i < nx; ++i){
			vec3 col(0, 0, 0);
			for(int s = 0; s < ns; ++s){
				float u = float(i + ud(e)) / float(nx);
				float v = float(j + ud(e)) / float(ny);
				ray r = cam.get_ray(u, v);
				vec3 p = r.point_at_parameter(2.0);
				col += color(r, world);
			}
			col /= float(ns);
			col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));
			int ir = int(255.99*col[0]);
			int ig = int(255.99*col[1]);
			int ib = int(255.99*col[2]);	
			outfile << ir << " " << ig << " " << ib << "\n";
		}
	}
}