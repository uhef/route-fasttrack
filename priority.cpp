#include <iostream>
#include <queue>
#include <string>

class Element {
public:
  std::string name;
  int priority;
  Element(const std::string& n, int p) {
    name = n;
    priority = p;
  }
  const std::string& toString() {
    return name;
  }
};

int main() {
  std::cout << "Hello priority queue!" << std::endl;
  Element e("foobar", 10);
  std::cout << e.toString() << std::endl;
  std::priority_queue<int> pq;
  pq.push(3);
  pq.push(6);
  pq.push(1);
  while(!pq.empty()) {
    std::cout << "Next element: " << pq.top() << std::endl;
    pq.pop();
  }
  return 0;
}

