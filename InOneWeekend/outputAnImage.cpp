#include <iostream>
#include <fstream>
using namespace std;

int main(){
	int nx = 200;
	int ny = 100;
	ofstream outFile;
	outFile.open("alpha.ppm");

	outFile << "P3\n" << nx << " " << ny << "\n255\n";
	for(int j = ny-1; j >= 0; --j){
		for(int i = 0; i < nx; ++i){
			float r = float(i) / float(nx);
			float g = float(j) / float(ny);
			float b = 0.2;
			int ir = int(255.99 * r);
			int ig = int(255.99 * g);
			int ib = int(255.99 * b);
			outFile << ir << " " << ig << " " << ib << "\n";
		}
	}
	outFile.close();
}