#include <complex>
#include <fstream>
#include <iostream>
#include <vector>

#pragma omp parallel for

int HEIGHT = 100;
int WIDTH = 150;
int MAX_ITER = 12;

std::complex<double> map_pixel_to_cmplx(double x, double y) {
  double imag = -1.5 + (double)y / HEIGHT * 3.0;
  double real = -2.0 + (double)x / WIDTH * 3.0;
  return std::complex<double>(real, imag);
}

void print_on_terminal() {
  for (int j = 0; j < HEIGHT; j++) {
    for (int i = 0; i < WIDTH; i++) {
      std::complex<double> z = 0;
      std::complex<double> c = map_pixel_to_cmplx(i, j);

      bool escaped = false;
      for (int _iter = 0; _iter < MAX_ITER; _iter++) {
        z = z * z + c;
        escaped = escaped || (std::abs(z) > 2);
      }

      std::cout << (escaped ? "#" : ".");
    }
    std::cout << "\n";
  }
}

void write_ppm(std::vector<std::vector<int>> &pixels) {
  std::ofstream file("mandelbrot_set.ppm");
  if (!file.is_open()) {
    std::cerr << "Failed to create file\n";
    std::exit(1);
  }

  file << "P3\n";
  file << WIDTH << " " << HEIGHT << "\n";
  file << "255\n";
  for (auto &pixel_row : pixels) {
    for (auto pixel : pixel_row)
      file << pixel << " ";
    file << "\n";
  }
  file.close();
}

void make_image() {
  std::vector<std::vector<int>> pixels;

  for (int j = 0; j < HEIGHT; j++) {
    std::vector<int> pixel_row;
    for (int i = 0; i < WIDTH; i++) {
      int iter = 0;
      std::complex<double> z = 0;
      std::complex<double> c = map_pixel_to_cmplx(i, j);

      while (iter < MAX_ITER && std::norm(z) <= 4.0) {
        z = z * z + c;
        iter++;
      }

      int color;
      double mu = iter + 1 - std::log2(std::log(std::abs(z)));

      if (iter == MAX_ITER)
        color = 0;
      else {
        double mu = iter + 1 - std::log2(std::log(std::abs(z)));
        color = std::min(255, static_cast<int>(mu * 255 / MAX_ITER));
      }

      pixel_row.push_back(color);
      pixel_row.push_back(color);
      pixel_row.push_back(color);
    }
    pixels.push_back(pixel_row);
  }

  write_ppm(pixels);
}

/// cmnd line args: height width max_iter
int main(int argv, char **argc) {
  if (argv == 4) {
    HEIGHT = std::stod(argc[1]);
    WIDTH = std::stod(argc[2]);
    MAX_ITER = std::stod(argc[3]);
  }

  make_image();
}
