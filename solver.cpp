#include <iostream>
#include <vector>

using namespace std;

struct Vec {
  int x;
  int y;

  Vec() : x(0), y(0) { }
  Vec(int x, int y) : x(x), y(y) { }

  Vec operator+(const Vec& other) {
    return Vec(x + other.x, y + other.y);
  }
};

class Sausage {
 public:
  Sausage(const vector<Vec>& points) : points(points) {
	grilled.resize(points.size());
  }

  Sausage clone() const {
	return Sausage(points);
  }
 private:
  vector<Vec> points;
  vector<Vec> grilled;
};

int main() {
  cout << "Faster solver" << endl;
}
