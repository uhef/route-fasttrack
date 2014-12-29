#include <iostream>
#include <cstdio>

int main() {
  std::cout << "Hello world!" << std::endl;
  FILE* inputFile = fopen("graph.json", "r");
  if (inputFile != NULL) {
    std::cout << "File open successfull" << std::endl;
    fclose(inputFile);
  }
  return 0;
}

