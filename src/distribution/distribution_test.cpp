#include <iostream>
#include <random>

template <size_t SLOT_NUM, typename T>
void print_distribution(T distribution, const std::string& name) {
  const int nrolls = 100000;  // number of experiments
  const int nstars = 200;     // maximum number of stars to distribute

  std::random_device rd;
  std::mt19937 gen(rd());

  int p[SLOT_NUM] = {};

  for (int i = 0; i < nrolls; ++i) {
    double number = distribution(gen);
    ++p[int(number) % SLOT_NUM];
  }

  std::cout << name << std::endl;

  for (size_t i = 0; i < SLOT_NUM; ++i) {
    std::cout << i << ": ";
    std::cout << std::string(p[i] * nstars / nrolls, '*') << std::endl;
  }
}

int main(int argc, const char* argv[]) {
  print_distribution<10>(std::geometric_distribution<>(0.16), "geometric_distribution(0.16)");
  print_distribution<15>(std::geometric_distribution<>(0.16), "geometric_distribution(0.16)");
  print_distribution<20>(std::geometric_distribution<>(0.16), "geometric_distribution(0.16)");

  print_distribution<10>(std::geometric_distribution<>(0.2), "geometric_distribution(0.2)");
  print_distribution<15>(std::geometric_distribution<>(0.2), "geometric_distribution(0.2)");
  print_distribution<20>(std::geometric_distribution<>(0.2), "geometric_distribution(0.2)");

  print_distribution<10>(std::geometric_distribution<>(0.32), "geometric_distribution(0.32)");
  print_distribution<15>(std::geometric_distribution<>(0.32), "geometric_distribution(0.32)");
  print_distribution<20>(std::geometric_distribution<>(0.32), "geometric_distribution(0.32)");

  print_distribution<10>(std::chi_squared_distribution<double>(1.0), "chi_squared_distribution(1.0)");
  print_distribution<15>(std::chi_squared_distribution<double>(1.0), "chi_squared_distribution(1.0)");
  print_distribution<20>(std::chi_squared_distribution<double>(1.0), "chi_squared_distribution(1.0)");

  print_distribution<10>(std::chi_squared_distribution<double>(2.0), "chi_squared_distribution(2.0)");
  print_distribution<15>(std::chi_squared_distribution<double>(2.0), "chi_squared_distribution(2.0)");
  print_distribution<20>(std::chi_squared_distribution<double>(2.0), "chi_squared_distribution(2.0)");

  print_distribution<10>(std::chi_squared_distribution<double>(2.5), "chi_squared_distribution(2.5)");
  print_distribution<15>(std::chi_squared_distribution<double>(2.5), "chi_squared_distribution(2.5)");
  print_distribution<20>(std::chi_squared_distribution<double>(2.5), "chi_squared_distribution(2.5)");

  print_distribution<10>(std::chi_squared_distribution<double>(3.0), "chi_squared_distribution(3.0)");
  print_distribution<15>(std::chi_squared_distribution<double>(3.0), "chi_squared_distribution(3.0)");
  print_distribution<20>(std::chi_squared_distribution<double>(3.0), "chi_squared_distribution(3.0)");

  print_distribution<10>(std::chi_squared_distribution<double>(4.0), "chi_squared_distribution(4.0)");
  print_distribution<15>(std::chi_squared_distribution<double>(4.0), "chi_squared_distribution(4.0)");
  print_distribution<20>(std::chi_squared_distribution<double>(4.0), "chi_squared_distribution(4.0)");

  print_distribution<10>(std::negative_binomial_distribution<int>(4, 0.5),
                         "std::negative_binomial_distribution<int>(4, 0.5)");
  print_distribution<15>(std::negative_binomial_distribution<int>(4, 0.5),
                         "std::negative_binomial_distribution<int>(4, 0.5)");
  print_distribution<20>(std::negative_binomial_distribution<int>(4, 0.5),
                         "std::negative_binomial_distribution<int>(4, 0.5)");

  return 0;
}