/* 
@author Brian Gauch - CS396
*/

#include <algorithm>
#include <vector>
#include <set>

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

using namespace std;

const double WORLD_SIZE = 1.0;

double min(double a, double b)
{ 
   return (a < b) ? a : b;
}

double max(double a, double b)
{ 
   return (a > b) ? a : b;
}

struct point_t {
  double x, y;
  bool used;
};

struct rect_t {
  double min_x, max_x, min_y, max_y;

  bool operator<(const rect_t &a) const
  {
    double id = min_x + 2*max_x + 3*min_y + 5*max_y;
    double otherId = a.min_x + 2*a.max_x + 3*a.min_y +5*a.max_y;
    return id < otherId;
  }  

  rect_t& operator=(const rect_t &a)
  {     
    min_x=a.min_x;
    max_x=a.max_x;
    min_y=a.min_y;
    max_y=a.max_y;
    return *this;
  }

  bool operator==(const rect_t &a) const
  {
    return min_x == a.min_x
      && max_x == a.max_x
      && min_y == a.min_y
      && max_y == a.max_y;
  }
};

struct intersection_t {
  rect_t bounds;
  set<rect_t> intersected_rects;
  
  bool operator<(const intersection_t &a) const
  {
    return bounds < a.bounds;
  }
  
  intersection_t& operator=(const intersection_t& a)
  {     
    bounds=a.bounds;
    intersected_rects=a.intersected_rects;
    return *this;
  }

  bool operator==(const intersection_t& a) const
  {
    return (intersected_rects == a.intersected_rects
         && bounds == a.bounds);
  }
};

struct segment_t {
  // rectangle that this segment is a part of
  rect_t* rectPtr;
  // y coord if horiz line segment
  double position;
  // x coords if horiz line segment
  double start;
  double end;
};

int compare_position(const void* aa, const void* bb)
{
  segment_t *a = (segment_t*)aa;
  segment_t *b = (segment_t*)bb;
  return a->position - b->position;
}

int compare_start(const void* aa, const void* bb)
{
  segment_t *a = (segment_t*)aa;
  segment_t *b = (segment_t*)bb;
  return a->start - b->start;
}

int compare_end(const void* aa, const void* bb)
{
  segment_t *a = (segment_t*)aa;
  segment_t *b = (segment_t*)bb;
  return a->end - b->end;
}

string p2str(point_t p)
{
  stringstream ss;
  ss << "(" << p.x << ", " << p.y << ")";
  return ss.str(); 
}

string r2str(rect_t r)
{
  stringstream ss;
  ss << "(x:[" << r.min_x << ", " << r.max_x << "], ";
  ss << "y:[" << r.min_y << ", " << r.max_y << "])";
  return ss.str(); 
}

string i2str(intersection_t i)
{
  stringstream ss;
  ss << "intersection of " << i.intersected_rects.size() << " rects: ";
  ss << r2str(i.bounds);
  return ss.str(); 
}

void print_points(point_t points[], int num_points)
{
  string str = "{";
  for(int i=0; i<num_points; i++)
  {
    str += p2str(points[i]);
    if(i < num_points-1)
    {
      str += ", \n";
    }
  }
  str += "}\n";
  cout << str;
}

void print_rects(rect_t rects[], int num_rects)
{
  string str = "{";
  for(int i=0; i<num_rects; i++)
  {
    str += r2str(rects[i]);
    if(i < num_rects-1)
    {
      str += ", \n";
    }
  }
  str += "}\n";
  cout << str;
}

void print_inters(set<intersection_t> inters)
{
  stringstream ss;
  ss << inters.size();
  ss << " intersections: {\n";
  for (set<intersection_t>::iterator i = inters.begin(); i != inters.end(); ++i)
  {
    intersection_t inter = *i;
    ss << i2str(inter);
    ss << ", \n";
  }
  string str = ss.str();
  // delete last comma
  if(!inters.empty())
  {
    str.erase(str.end()-3, str.end());
  }
  str += "}\n";
  cout << str;
}

int my_rand(int min, int max)
{
  int output = min + (rand() % (int)(max - min + 1));
  return output;
}

// returns a random double between 0 and 1
double my_double_rand(double min, double max)
{
  double x = min + (rand() / ( RAND_MAX / (max-min) ) ) ;
  //double x = ((double)rand()/(double)RAND_MAX);
  return x;
}

bool containsPoint(point_t p, rect_t rect)
{
  if(p.x < rect.min_x || p.x > rect.max_x)
  {
    return false;
  }
  if(p.y < rect.min_y || p.y > rect.max_y)
  {
    return false;
  }
  return true;
}

bool getIntersection(rect_t &intersection, rect_t rect1, rect_t rect2)
{
  intersection.min_x = max(rect1.min_x, rect2.min_x);
  intersection.max_x = min(rect1.max_x, rect2.max_x);
  intersection.min_y = max(rect1.min_y, rect2.min_y);
  intersection.max_y = min(rect1.max_y, rect2.max_y);
  if(intersection.min_x > intersection.max_x
  || intersection.min_y > intersection.max_y)
  {
    return false;
  }
  return true;
}

void naiveMaxCliquesHelper(set<intersection_t> &inters, 
                             rect_t rects[], int num_rects, int depth)
{
  if(depth<=0)
  {
    return;
  }
  set<intersection_t> new_inters;
  bool doneRecursion = true;
  //for(int i=0; i<inters.size(); i++)
  for (set<intersection_t>::iterator i = inters.begin(); i != inters.end(); ++i) {
    intersection_t inter = *i;
    //cout << "searching for intersections with: " << i2str(inter) << endl;
    bool intersectsAtLeastOne = false;
    for(int j=0; j<num_rects; j++)
    {
      rect_t rect = rects[j];
      rect_t new_inter_bounds;
      bool doIntersect = getIntersection(new_inter_bounds, rect, inter.bounds);
      if(doIntersect)
      {
        intersection_t new_inter = inter;
        new_inter.bounds = new_inter_bounds;
        //cout << "new " << i2str(new_inter) << endl;
        
        pair<set<rect_t>::iterator, bool> ret = new_inter.intersected_rects.insert(rect);
        if(ret.second == true)
        {
          // was actually inserted
          //cout << "was actually inserted" << endl;
          new_inters.insert(new_inter);
          intersectsAtLeastOne = true;
          doneRecursion = false;
        }
      }
    }
    // replace this clique with other, larger cliques if they exist
    if(!intersectsAtLeastOne)
    {
      new_inters.insert(inter);
      //cout << "re-inserting " << i2str(inter) << endl;
    }
  }
  if(!doneRecursion)
  {
    naiveMaxCliquesHelper(new_inters, rects, num_rects, depth-1);
  }
  inters = new_inters;
}

void naiveMaxCliques(set<intersection_t> &maximal_inters, rect_t rects[], int num_rects)
{
  for(int i=0; i<num_rects; i++)
  {
    rect_t rect = rects[i];
    intersection_t inter;
    inter.bounds = rect;

    //pair<set<rect_t>::iterator, bool> ret1 = 
    inter.intersected_rects.insert(rect);
    pair<set<intersection_t>::iterator, bool> ret2 = maximal_inters.insert(inter);
    if(ret2.second == false)
    {
      cout << "failed to insert: " << i2str(inter) << endl;
      set<intersection_t>::iterator it = ret2.first;
      intersection_t other = *it;
      cout << "because of other: " << i2str(other) << endl;
    }
  }
  naiveMaxCliquesHelper(maximal_inters, rects, num_rects, num_rects);
}

// scanline from left to right
void scanForMaxCliques(rect_t rects[], int num_rects)
{
  int num_segments = 2*num_rects;
  segment_t horizontal[num_segments];
  segment_t vertical[num_segments];
  for(int i = 0; i<num_rects; i++)
  {
    rect_t rect = rects[i];

    vertical[i].rectPtr = &rect;
    vertical[i].position = rect.min_x;
    vertical[i].start = rect.min_y;
    vertical[i].end = rect.max_y;

    vertical[i+1].rectPtr = &rect;
    vertical[i+1].position = rect.max_x;
    vertical[i+1].start = rect.min_y;
    vertical[i+1].end = rect.max_y;

    horizontal[i].rectPtr = &rect;
    horizontal[i].position = rect.min_y;
    horizontal[i].start = rect.min_x;
    horizontal[i].end = rect.max_x;
    
    horizontal[i+1].rectPtr = &rect;
    horizontal[i+1].position = rect.max_y;
    horizontal[i+1].start = rect.min_x;
    horizontal[i+1].end = rect.max_x;
  }
  segment_t h_starts[num_segments];
  segment_t h_ends[num_segments];
  memcpy(h_starts, horizontal, num_segments*sizeof(segment_t));
  memcpy(h_ends, horizontal, num_segments*sizeof(segment_t));
  qsort(vertical, num_segments, sizeof(segment_t), compare_position);
  qsort(h_starts, num_segments, sizeof(segment_t), compare_start);
  qsort(h_ends, num_segments, sizeof(segment_t), compare_end);

  int num_active = 0;
  segment_t active_horizontal_lines[num_segments];
  int h_start_index = 0;
  int h_end_index = 0;
  for(int v_index=0; v_index < num_segments; v_index++)
  {
    double x = vertical[v_index].position;
    double h_start_x = 0.0;
    double h_end_x = 0.0;
    while(h_start_x < x)
    {
      h_start_index++;
      h_start_x = h_starts[h_start_index].start;
      active_horizontal_lines[num_active++] = h_starts[h_start_index];
    }
    while(h_end_x < x)
    {
      h_end_index++;
      h_end_x = h_ends[h_end_index].end;
    }
  }
}

void genRect(rect_t &rect)
{
   double x1 = random() * 1.0 / RAND_MAX;
   double x2 = random() * 1.0 / RAND_MAX;
   double y1 = random() * 1.0 / RAND_MAX;
   double y2 = random() * 1.0 / RAND_MAX;
   rect.min_x = min(x1, x2);
   rect.max_x = max(x1, x2);
   rect.min_y = min(y1, y2);
   rect.max_y = max(y1, y2);
}

void genRects(rect_t* rects, int num_rects)
{
  srand(time(NULL));
  for(int i=0; i<num_rects; i++)
  {
    genRect(rects[i]);
  }
}

void rectsFromPoints(rect_t rects[], point_t points[], 
                     int arr_size, double radius)
{
  // ensure that the square falls entirely within the circle
  double d = (1.0/sqrt(2.0))*radius;
  for(int i=0; i<arr_size; i++)
  {
    point_t p = points[i];
    rect_t rect;
    rect.min_x = p.x - d;
    rect.max_x = p.x + d;
    rect.min_y = p.y - d;
    rect.max_y = p.y + d;
    rects[i] = rect;
  }
}

// Any point within the intersection's bounds will do.
// We arbitrarily select the center point.
void pointsFromIntersections(vector<point_t> &points, set<intersection_t> inters)
{
  for(set<intersection_t>::iterator i = inters.begin(); i != inters.end(); ++i)
  {
    intersection_t inter = *i;
    point_t p;
    p.x = (inter.bounds.min_x + inter.bounds.max_x)/2;
    p.y = (inter.bounds.min_y + inter.bounds.max_y)/2;
    points.push_back(p);
  }
}

// main function called externally
void maxCliquesFromPoints(vector<point_t> &clique_points, point_t centers[], 
                          int num_rects, double radius)
{
  rect_t rects[num_rects];
  rectsFromPoints(rects, centers, num_rects, radius);
  //cout << "---------- associated rectangles: ----------" << endl;
  //print_rects(rects, num_rects);
  set<intersection_t> maximal_cliques;
  naiveMaxCliques(maximal_cliques, rects, num_rects);
  //cout << "---------- maximal cliques: ----------" << endl;
  //print_inters(maximal_cliques);
  pointsFromIntersections(clique_points, maximal_cliques);
}

/*
int main (int argc, char **argv)
{ 
  int num_rects;
  cin >> num_rects; 
  rect_t rects[num_rects];
  for(int i=0; i<num_rects; i++)
  {
    cin >> rects[i].min_x >> rects[i].max_x >> rects[i].min_y >> rects[i].max_y;
  }
  //genRects(rects, num_rects);
  print_rects(rects, num_rects);
  set<intersection_t> maximal_inters;
  naiveCliqueSearch(maximal_inters, rects, num_rects);
  print_inters(maximal_inters);
  //scanForCliques(rects, num_rects);
  return 0;
}
*/
