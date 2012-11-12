/* Copyright (c) 2012 the authors listed at the following URL, and/or
   the authors of referenced articles or incorporated external code:
   http://en.literateprograms.org/Median_cut_algorithm_(C_Plus_Plus)?action=history&offset=20080309133934
 
   Permission is hereby granted, free of charge, to any person obtaining
   a copy of this software and associated documentation files (the
   "Software"), to deal in the Software without restriction, including
   without limitation the rights to use, copy, modify, merge, publish,
   distribute, sublicense, and/or sell copies of the Software, and to
   permit persons to whom the Software is furnished to do so, subject to
   the following conditions:
 
   The above copyright notice and this permission notice shall be
   included in all copies or substantial portions of the Software.
 
   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
   IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
   CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
   TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
   SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 
   Retrieved from: http://en.literateprograms.org/Median_cut_algorithm_(C_Plus_Plus)?oldid=12754
*/

#include <limits>
#include <queue>
#include <algorithm>
#include <gif/MedianCut.h>

MCBlock::MCBlock(MCPoint* MCPoints, int MCPointsLength) {
  this->MCPoints = MCPoints;
  this->MCPointsLength = MCPointsLength;
  for(int i=0; i < NUM_DIMENSIONS; i++)  {
    minCorner.x[i] = std::numeric_limits<unsigned char>::min();
    maxCorner.x[i] = std::numeric_limits<unsigned char>::max();
  }
}

MCPoint * MCBlock::getMCPoints() {
  return MCPoints;
}

int MCBlock::numMCPoints() const {
  return MCPointsLength;
}

int MCBlock::longestSideIndex() const {
  int m = maxCorner.x[0] - minCorner.x[0];
  int maxIndex = 0;
  for(int i=1; i < NUM_DIMENSIONS; i++)  {
    int diff = maxCorner.x[i] - minCorner.x[i];
    if (diff > m) {
      m = diff;
      maxIndex = i;
    }
  }
  return maxIndex;
}

int MCBlock::longestSideLength() const {
  int i = longestSideIndex();
  return maxCorner.x[i] - minCorner.x[i];
}

bool MCBlock::operator<(const MCBlock& rhs) const {
  return this->longestSideLength() < rhs.longestSideLength();
}

void MCBlock::shrink() {
  int i,j;
  for(j=0; j<NUM_DIMENSIONS; j++) {
    minCorner.x[j] = maxCorner.x[j] = MCPoints[0].x[j];
  }
	
  for(i=1; i < MCPointsLength; i++) {
    for(j=0; j< NUM_DIMENSIONS; j++) {
      minCorner.x[j] = min(minCorner.x[j], MCPoints[i].x[j]);
      maxCorner.x[j] = max(maxCorner.x[j], MCPoints[i].x[j]);
    }
  }
}

std::vector<MCPoint> medianCut(MCPoint* image, int numMCPoints, unsigned int desiredSize) {
  std::priority_queue<MCBlock> MCBlockQueue;
  MCBlock initialMCBlock(image, numMCPoints);
  initialMCBlock.shrink();
  MCBlockQueue.push(initialMCBlock);
  std::vector<MCPoint> result;
	
  while (MCBlockQueue.size() < desiredSize && MCBlockQueue.top().numMCPoints() > 1) {
    MCBlock longestMCBlock = MCBlockQueue.top();
		
    MCBlockQueue.pop();
    MCPoint * begin  = longestMCBlock.getMCPoints();
    MCPoint * median = longestMCBlock.getMCPoints() + (longestMCBlock.numMCPoints()+1)/2;
    MCPoint * end    = longestMCBlock.getMCPoints() + longestMCBlock.numMCPoints();
    switch(longestMCBlock.longestSideIndex()) {
    case 0: std::nth_element(begin, median, end, CoordinateMCPointComparator<0>()); break;
    case 1: std::nth_element(begin, median, end, CoordinateMCPointComparator<1>()); break;
    case 2: std::nth_element(begin, median, end, CoordinateMCPointComparator<2>()); break;
    }
		
    MCBlock MCBlock1(begin, median-begin), MCBlock2(median, end-median);
    MCBlock1.shrink();
    MCBlock2.shrink();
    MCBlockQueue.push(MCBlock1);
    MCBlockQueue.push(MCBlock2);

  }
	
  while(!MCBlockQueue.empty()) {
    MCBlock MCBlock = MCBlockQueue.top();
    MCBlockQueue.pop();
    MCPoint * MCPoints = MCBlock.getMCPoints();
		
    int sum[NUM_DIMENSIONS] = {0};
    for(int i=0; i < MCBlock.numMCPoints(); i++) {
      for(int j=0; j < NUM_DIMENSIONS; j++) {
        sum[j] += MCPoints[i].x[j];
      }
    }
		
    MCPoint averageMCPoint;
    for(int j=0; j < NUM_DIMENSIONS; j++) {
      averageMCPoint.x[j] = sum[j] / MCBlock.numMCPoints();
    }
		
    result.push_back(averageMCPoint);
  }
  return result;
}

