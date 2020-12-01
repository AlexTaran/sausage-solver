#include <algorithm>
#include <iostream>
#include <unordered_set>
#include <utility>
#include <vector>

using namespace std;

template <class T>
inline void combine(size_t& seed, const T& v) {
    seed ^= hash<T>()(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

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
    return hash<int>()(v.x) ^ (hash<int>()(v.y) >> 1);
  }
};

char PlayerDirectionToChar(const Vec& v) {
  if (v == Vec(-1, 0)) return '<';
  if (v == Vec(0, -1)) return '^';
  if (v == Vec(0, 1))  return 'V';
  if (v == Vec(1, 0))  return '>';
  return '?';
}

Vec PlayerCharToDirection(char ch) {
  if (ch == '<') return Vec(-1, 0);
  if (ch == '^') return Vec(0, -1);
  if (ch == 'V') return Vec(0, 1);
  if (ch == '>') return Vec(1, 0);
  return Vec(0, 0);
}

class Sausage {
 public:
  Sausage(const vector<Vec>& points) : points(points) {
	grilled.resize(points.size());
  }

  bool operator==(const Sausage& other) const {
    return points == other.points && grilled == other.grilled;
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
 
  void roll(const Vec& dv) {
    for (auto& p: points) {
      p = p + dv;
    }
    if ((isHorizontal() && dv.y % 2 == 1) || (isVertical() && dv.x % 2 == 1)) {
      for (auto& g: grilled) {
		swap(g.first, g.second);
	  }
    }
  }

  bool hasPoint(const Vec& p) {
	return find(points.begin(), points.end(), p) != points.end();
  }

  template<template<class T> class Collection>
  bool hasAnyOfPoints(const Collection<Vec>& points) {
    for (const auto& p: points) {
	  if (hasPoint(p)) {
	    return true;
	  }
	}
	return false;
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

  const vector<pair<int, int>>& getGrilled() const {
    return grilled;
  }

  size_t hash() const {
	size_t result = 0;
	for (const auto& v: points) {
      combine(result, v.x);
      combine(result, v.y);
    }
    for (const auto& g: grilled) {
      combine(result, g.first);
      combine(result, g.second);
    }
    return result;
  }

 private:
  vector<Vec> points;
  vector<pair<int, int>> grilled; // first = bottom, second = top
};

template<>
struct std::hash<Sausage> {
  size_t operator() (const Sausage& v) const {
    return v.hash();
  }
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

  bool isSausageSupported(const Sausage& s) const {
    for (const auto& v: s.getPoints()) {
	  if (supportPoints.contains(v)) {
	    return true;
	  }
    }
	return false;
  }

  void grillSausage(Sausage& s) const {
    s.grillByPoints(grillPoints);
  }

  bool isPlayerPositionValid(const Vec& pos) const {
	return supportPoints.contains(pos) and !grillPoints.contains(pos);
  }

  const vector<string>& getField() const {
    return field;
  }

 private:
  vector<string> field;
  unordered_set<Vec> grillPoints;
  unordered_set<Vec> supportPoints;
};

class Position {
 public:
  Position(const Field* field, const vector<Sausage> sausages, const Vec& playerPos, const Vec& playerDir)
    : field(field), sausages(sausages), playerPos(playerPos), playerDir(playerDir)
  { }

  Position(const Position& other)
    : field(other.field), sausages(other.sausages), playerPos(other.playerPos), playerDir(other.playerDir)
  { }

  bool operator==(const Position& other) const {
    return field == other.field && sausages == other.sausages &&
		   playerPos == other.playerPos && playerDir == other.playerDir;
  }

  size_t hash() const {
    size_t result = 0;
	for (const auto& s: sausages) {
	  combine(result, s);
	}
	combine(result, playerPos);
	combine(result, playerDir);
	return result;
  }

  bool isWinning() const {
	for (const auto& s: sausages) {
	  if (!s.isReady()) {
	    return false;
	  }
	}
	return true;
  }

  bool isLosing() const {
	for (const auto& s: sausages) {
	  if (s.isBurned() || !field->isSausageSupported(s)) {
	    return true;
	  }
	}
	return !isPlayerStateValid();
  }

  vector<Position> expand() const {  // Returns adjacent 4 positions
    // Forward
    Position fwd(*this);
    Vec fwdDir = directionForward();
    fwd.playerPos = fwd.playerPos + fwdDir;
    fwd.touchAllSausages(fwd.playerPos + fwdDir, fwdDir);
    // Backward
    Position bck(*this);
    Vec bckDir = directionBackward();
    bck.playerPos = bck.playerPos + bckDir;
    bck.touchAllSausages(bck.playerPos, bckDir);
    // Left
    Position lft(*this);
    Vec lftDir = directionLeft();
    lft.playerDir = lftDir;
    lft.touchAllSausages(fwd.playerPos + lftDir, lftDir);
    lft.touchAllSausages(lft.playerPos + lftDir, bckDir);
    // Right
    Position rgh(*this);
    Vec rghDir = directionRight();
    rgh.playerDir = rghDir;
    rgh.touchAllSausages(fwd.playerPos + rghDir, rghDir);
    rgh.touchAllSausages(rgh.playerPos + rghDir, bckDir);
    return vector<Position> {fwd, bck, lft, rgh};
  }

  void render() {
	vector<string> lines = field->getField();
    lines[playerPos.y][playerPos.x] = PlayerDirectionToChar(playerDir);
    for (int i = 0; i < sausages.size(); ++i) {
      for (const auto& gr: sausages[i].getGrilled()) {
        cout << "[" << gr.first << ", " << gr.second << "] ";
      }
      cout << endl;
      for (const auto& p: sausages[i].getPoints()) {
        lines[p.y][p.x] = '0' + i;
      }
    }
    for (const auto& l: lines) {
      cout << l << endl;
    }
  }

 private:
  Vec directionLeft() const {
    return Vec(playerDir.y, -playerDir.x);
  }

  Vec directionRight() const {
    return Vec(-playerDir.y, playerDir.x);
  }

  Vec directionForward() const {
    return playerDir;
  }

  Vec directionBackward() const {
    return Vec(-playerDir.x, -playerDir.y);
  }

  void touchAllSausages(const Vec& point, const Vec& direction) {
    vector<bool> moved(sausages.size(), false);
    vector<Vec> touchPoints {point};
    vector<Vec> nextTouchPoints;
    while (touchPoints.size() > 0) {
      for (int i = 0; i < sausages.size(); ++i) {
        if (moved[i]) {
          continue;
        }
        if (sausages[i].hasAnyOfPoints(touchPoints)) {
          sausages[i].roll(direction);
          field->grillSausage(sausages[i]);
          moved[i] = true;
		  for (const auto& p: sausages[i].getPoints()) {
		    nextTouchPoints.push_back(p);
		  }
        }
      }
      touchPoints = nextTouchPoints;
      nextTouchPoints.clear();
    }
  }

  bool isPlayerStateValid() const {
    return field->isPlayerPositionValid(playerPos);
  }

  const Field* field;
  vector<Sausage> sausages;
  Vec playerPos;
  Vec playerDir;
};

template<>
struct std::hash<Position> {
  size_t operator() (const Position& p) const {
    return p.hash();
  }
};


int main() {
  cout << "Faster solver" << endl;
}
