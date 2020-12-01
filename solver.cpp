#include <algorithm>
#include <iostream>
#include <unordered_set>
#include <utility>
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

  bool operator==(const Vec& other) {
    return x == other.x && y == other.y;
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

  bool isVertical() const {
    for (int i = 1 ; i < points.size(); ++i) {
	  if (points[0].x != points[i].x) {
	    return false;
	  }
	}
	return true;
  }

  bool isHorizontal() const {
    for (int i = 1 ; i < points.size(); ++i) {
	  if (points[0].y != points[i].y) {
	    return false;
	  }
	}
	return true;
  }

  bool isBurned() const {
    for (const auto& g: grilled) {
	  if (g.first > 1 || g.second > 1) {
	    return true;
	  }
	}
	return false;
  }

  bool isReady() const {
    for (const auto& g: grilled) {
	  if (g.first != 1 || g.second != 1) {
	    return false;
	  }
	}
	return true;
  }
 
  void Roll(const Vec& dv) {
    for (auto& p: points) {
      p = p + dv;
    }
    if ((isHorizontal() && dv.y % 2 == 1) || (isVertical() && dv.x % 2 == 1)) {
      for (auto& g: grilled) {
		swap(g.first, g.second);
	  }
    }
  }

  bool HasPoint(const Vec& p) {
	return find(points.begin(), points.end(), p) != points.end();
  }

  template<template<class T> class Collection>
  void grillByPoints(const Collection<Vec>& grillPoints) {
    for (size_t i = 0; i < points.size(); ++i) {
      if (grillPoints.contains(points[i])) {
        grilled[i].first++;
      }
    }
  }

  const vector<Vec>& getPoints() const {
    return points;
  }

 private:
  vector<Vec> points;
  vector<pair<int, int>> grilled; // first = bottom, second = top
};

int main() {
  cout << "Faster solver" << endl;
}
