#!/usr/bin/env python3

PlayerCharToDirection = {
  '<': (-1, 0),
  '^': (0, -1),
  'V': (0, 1),
  '>': (1, 0),
}

PlayerDirectionToChar = {
  (-1, 0): '<',
  (0, -1): '^',
  (0, 1) : 'V',
  (1, 0) : '>',
}

def VAdd(v1, v2):
  return (v1[0] + v2[0], v1[1] + v2[1])

def VNeg(v):
  return (-v[0], -v[1])

class Sausage:
  def __init__(self, points):
    self._points = [(p[0], p[1]) for p in points]  # [[x, y]...]
    self._grilled = [(0, 0) for p in points]  # [[bottom, top]...]

    if not self.IsVertical() and not self.IsHorizontal():
      raise Exception('Bad sausage ' + str(self._points))

  def __hash__(self):
    values = [coord for p in self._points for coord in p] + \
             [grillvalue for g in self._grilled for grillvalue in g]
    return hash(tuple(values))

  def __eq__(self, other):
    return self._points == other._points and self._grilled == other._grilled

  def Clone(self):
    s = Sausage(self._points)
    s._grilled = self._grilled[:]
    return s

  def IsVertical(self):
    return all([p[0] == self._points[0][0] for p in self._points])
  
  def IsHorizontal(self):
    return all([p[1] == self._points[0][1] for p in self._points])

  def IsBurned(self):
    return any([g[0] > 1 or g[1] > 1 for g in self._grilled])

  def IsReady(self):
    return all([g[0] == 1 and g[1] == 1 for g in self._grilled])

  def Roll(self, dx, dy):
    for i, p in enumerate(self._points):
      self._points[i] = VAdd(p, (dx, dy))
    if (self.IsHorizontal() and dy % 2 == 1) or (self.IsVertical() and dx % 2 == 1):
      for i, g in enumerate(self._grilled):
        self._grilled[i] = (g[1], g[0])

  def HasPoint(self, pt):
    return any([p[0] == pt[0] and p[1] == pt[1] for p in self._points])

  def GetPoints(self):
    return self._points[:]

  def HashValues(self):
    return [coord for p in self._points for coord in p] + \
           [grillvalue for g in self._grilled for grillvalue in g]

  # Ineffecive but fuck it.
  def GrillByPoints(self, grillPoints):
    for i, p in enumerate(self._points):
      if p in grillPoints:
        self._grilled[i] = (self._grilled[i][0] + 1, self._grilled[i][1])

  def IsSupportedByPoints(self, supportPoints):
    return any([p in supportPoints for p in self._points])

class Field:
  def __init__(self, field):
    self._field = field
    self._grillPoints = set()
    self._supportPoints = set()
    for y, line in enumerate(field):
      for x, ch in enumerate(line):
        if ch == 'G':
          self._grillPoints.add((x, y))
        if ch in ['G', 'L']:
          self._supportPoints.add((x, y))

  def IsSausageSupported(self, sausage):
    return sausage.IsSupportedByPoints(self._supportPoints)

  def GrillSausage(self, sausage):
    sausage.GrillByPoints(self._grillPoints)

  def IsPlayerPositionValid(self, pos):
    return pos in self._supportPoints and pos not in self._grillPoints

tuples = set()

class Position:
  def __init__(self, field, sausages, playerPos, playerDir):
    self._sausages = [s.Clone() for s in sausages]
    self._field = field

    self._playerPosition = (playerPos[0], playerPos[1])
    self._playerDirection = (playerDir[0], playerDir[1])

  def __hash__(self):  # Should hash only changeable fields: sausages and player state
    tp = self._sausages[:] + list(self._playerPosition) + list(self._playerDirection)
    return hash(tuple(tp))

  def __eq__(self, other):
    if other == None:
      return False
    return self._sausages == other._sausages and self._playerPosition == other._playerPosition and self._playerDirection == other._playerDirection

  def Clone(self):
    return Position(self._field, self._sausages, self._playerPosition, self._playerDirection)

  def IsWinning(self):
    return all([s.IsReady() for s in self._sausages])

  def IsLosing(self):
    return any([s.IsBurned() or not self._field.IsSausageSupported(s) for s in self._sausages]) or not self._IsPlayerStateValid()

  def Expand(self):  # Returns adjacent 4 positions
    # Forward
    fwd = self.Clone()
    fwdDir = self._DirectionForward()
    fwd._playerPosition = VAdd(fwd._playerPosition, fwdDir)
    fwd._TouchAllSausages(VAdd(fwd._playerPosition, fwdDir), fwdDir)
    # Backward
    bck = self.Clone()
    bckDir = self._DirectionBackward()
    bck._playerPosition = VAdd(bck._playerPosition, bckDir)
    bck._TouchAllSausages(bck._playerPosition, bckDir)
    # Left
    lft = self.Clone()
    lftDir = self._DirectionLeft()
    lft._playerDirection = lftDir
    lft._TouchAllSausages(VAdd(fwd._playerPosition, lftDir), lftDir)
    lft._TouchAllSausages(VAdd(lft._playerPosition, lftDir), bckDir)
    # Right
    rgh = self.Clone()
    rghDir = self._DirectionRight()
    rgh._playerDirection = rghDir
    rgh._TouchAllSausages(VAdd(fwd._playerPosition, rghDir), rghDir)
    rgh._TouchAllSausages(VAdd(rgh._playerPosition, rghDir), bckDir)
    return [fwd, bck, lft, rgh]

  def Render(self):
    lines = [list(line) for line in self._field._field]
    lines[self._playerPosition[1]][self._playerPosition[0]] = PlayerDirectionToChar[self._playerDirection]
    for i, s in enumerate(self._sausages):
      print(s._grilled)
      for p in s._points:
        lines[p[1]][p[0]] = str(i)
    for l in lines:
      print(''.join(l))


  def _DirectionLeft(self):
    return (self._playerDirection[1], -self._playerDirection[0])
  
  def _DirectionRight(self):
    return (-self._playerDirection[1], self._playerDirection[0])

  def _DirectionForward(self):
    return self._playerDirection
  
  def _DirectionBackward(self):
    return (-self._playerDirection[0], -self._playerDirection[1])

  def _TouchAllSausages(self, point, direction):
    moved = [False] * len(self._sausages)
    touchPoints = [point]
    nextTouchPoints = []
    while len(touchPoints) > 0:
      for i, s in enumerate(self._sausages):
        if moved[i]:
          continue
        if any([s.HasPoint(p) for p in touchPoints]):
          s.Roll(direction[0], direction[1])
          self._field.GrillSausage(s)
          moved[i] = True
          nextTouchPoints.extend(s.GetPoints())
      touchPoints = nextTouchPoints
      nextTouchPoints = []

  def _IsPlayerStateValid(self):
    return self._field.IsPlayerPositionValid(self._playerPosition)

DBGSTEP = 10000

def POS1():
  mp = [
      "       ",
      " LGGGL ",
      " LLLLL ",
      " LLLLL ",
      " LGGGL ",
      "       ",
  ]
  field = Field(mp)
  sausages = [
    Sausage([(1, 2), (2, 2)]),
    Sausage([(3, 2), (3, 3)]),
    Sausage([(4, 3), (5, 3)]),
  ]
  return Position(field, sausages, (1, 4), PlayerCharToDirection["^"])

def POS2():
  mp = [
      "        ",
      "  LLLLL ",
      " LL   L ",
      " L  G L ",
      " L G  L ",
      " L   LL ",
      " LLLLL  ",
      "        ",
  ]
  field = Field(mp)
  sausages = [
    Sausage([(3, 1), (4, 1)]),
  ]
  return Position(field, sausages, (1, 5), PlayerCharToDirection["^"])

def POS3():
  mp = [
      "       ",
      " LLLLL ",
      " LGLLG ",
      " LGLLG ",
      " LLLLL ",
      "       ",
  ]
  field = Field(mp)
  sausages = [
    Sausage([(3, 2), (3, 3)]),
    Sausage([(4, 2), (4, 3)]),
  ]
  return Position(field, sausages, (1, 4), PlayerCharToDirection["^"])

def main():
  initPosition = POS3()
  visited = {initPosition: 0}
  prevs = {initPosition: None}
  queue = [initPosition]

  vthresh = DBGSTEP

  while len(queue) > 0:
    elem = queue[0]
    queue = queue[1:]
    if elem.IsLosing():
      continue
    if len(visited) > vthresh:
      print('Depth', visited[elem], 'total', len(visited))
      vthresh += DBGSTEP
    if elem.IsWinning():
      print('Found for', visited[elem], 'moves')
      lst = [elem]
      while prevs[elem] != None:
        lst.append(prevs[elem])
        elem = prevs[elem]
      for i, pos in enumerate(lst[::-1]):
        print("{})".format(i))
        pos.Render()
      break
    positions = elem.Expand()
    for pos in positions:
      if pos in visited:
        continue
      visited[pos] = visited[elem] + 1
      prevs[pos] = elem
      queue.append(pos)

if __name__ == "__main__":
  main()
