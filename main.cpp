#include <iostream>
#include <fstream>

int main() {
  std::cout << "Hello world!" << std::endl;
  std::ifstream inputFileStream("graph.json", std::ifstream::in);
  char c;
  while(inputFileStream.get(c)) {
    std::cout << c;
  }
  inputFileStream.close();
  return 0;
}

