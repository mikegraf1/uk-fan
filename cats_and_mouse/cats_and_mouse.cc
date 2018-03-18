#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cmath>

struct S {
  int x;
  int y;
  int z;
  std::string output;
};
using location_info = std::vector<S>;

std::string calculate_output(int& a, int& b, int& c) {
  if (std::abs(a - c) == std::abs(b -c)) return "Mouse C";
  if (std::abs(a - c) > std::abs(b -c)) return "Cat B";
  else return "Cat A";
}

location_info read_info_from_file(std::string filename) {
  location_info vec;
  std::ifstream data(filename.c_str());
  char line[100];
  int i = 0;
  while (data.getline(line, sizeof(line))) {
   if (i > 0) {
     S s;
     std::stringstream ss;
     ss << line;
     ss >> s.x >> s.y >> s.z;
//std::cout << "s.x: " << s.x << " s.y: " << s.y << " s.c: " << s.z << '\n';
     s.output = calculate_output(s.x, s.y, s.z);
     vec.push_back(s);
   }
    ++i;
  }
  return vec;
}

int main() {
  location_info li = read_info_from_file("data.txt");
  for (auto const & s : li) {
    std::cout << s.output << '\n';
  }
  std::cout << "done!\n";
}
