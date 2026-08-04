#include <complex>
#include <fstream>
#include <iostream>
#include <thread>
#include <vector>

int HEIGHT = 100;
int WIDTH = 150;
int MAX_ITER = 12;

int get_thread_count() {
  int threads = std::thread::hardware_concurrency();
  return threads == 0 ? 4 : threads;
}

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

void write_ppm(std::vector<int> &pixels) {
  std::ofstream file("mandelbrot_set.ppm");
  if (!file.is_open()) {
    std::cerr << "Failed to create file\n";
    std::exit(1);
  }

  file << "P3\n";
  file << WIDTH << " " << HEIGHT << "\n";
  file << "255\n";
  for (int i = 0; i < HEIGHT; i++) {
    for (int j = 0; j < WIDTH; j++) {
      int idx = (i * WIDTH + j) * 3;
      file << pixels[idx] << " " << pixels[idx + 1] << " " << pixels[idx + 2]
           << " ";
    }
    file << "\n";
  }
  file.close();
}

void fill_color(int i, int j, std::vector<int> &pixels) {
  int iter = 0;
  std::complex<double> z{0.0, 0.0};
  std::complex<double> c = map_pixel_to_cmplx(i, j);

  while (iter < MAX_ITER && std::norm(z) <= 4.0) {
    z = z * z + c;
    iter++;
  }

  int color;

  if (iter == MAX_ITER)
    color = 0;
  else {
    double mu = iter + 1 - std::log2(std::log(std::abs(z)));
    color = std::min(255, static_cast<int>(mu * 255 / MAX_ITER));
  }

  int idx = (j * WIDTH + i) * 3;
  pixels[idx] = color;
  pixels[idx + 1] = color;
  pixels[idx + 2] = color;
}

void process_rows(int start_row, int end_row, std::vector<int> &pixels) {
  for (int i = start_row; i < end_row; i++) {
    for (int j = 0; j < WIDTH; j++) {
      fill_color(j, i, pixels);
    }
  }
}

void make_image() {
  int num_threads = get_thread_count();
  std::vector<int> pixels(WIDTH * HEIGHT * 3);
  std::vector<std::thread> threads;
  
  int rows_p_thr = HEIGHT / num_threads;

  for (int i = 0; i < num_threads; i++) {
    int start = i * rows_p_thr;
    int end = i == num_threads - 1 ? HEIGHT : start + rows_p_thr;

    threads.emplace_back(process_rows, start, end, std::ref(pixels));
  }

  for (auto &t : threads)
    t.join();

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