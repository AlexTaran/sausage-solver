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

  bool operator==(const Vec& other) const {
    return x == other.x && y == other.y;
  }
};

template<>
struct std::hash<Vec> {
  size_t operator() (const Vec& v) const {
    return std::hash<int>()(v.x) ^ (std::hash<int>()(v.y) >> 1);
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

class Field {
 public:
  Field(const vector<string>& field) : field(field) {
    for (int y = 0; y < field.size(); ++y) {
	  for (int x = 0; x < field[y].size(); ++x) {
		char ch = field[y][x];
		if (ch == 'G') {
		  grillPoints.insert(Vec(x, y));
		}
		if (ch == 'G' || ch == 'L') {
		  supportPoints.insert(Vec(x, y));
		}
	  }
	}
  }
  bool isSausageSupported(const Sausage& s) {
    for (const auto& v: s.getPoints()) {
	  if (supportPoints.contains(v)) {
	    return true;
	  }
    }
	return false;
  }

  void grillSausage(Sausage& s) {
    s.grillByPoints(grillPoints);
  }

  bool IsPlayerPositionValid(const Vec& pos) {
	return supportPoints.contains(pos) and !grillPoints.contains(pos);
  }

 private:
  vector<string> field;
  unordered_set<Vec> grillPoints;
  unordered_set<Vec> supportPoints;
};

int main() {
  cout << "Faster solver" << endl;
}
