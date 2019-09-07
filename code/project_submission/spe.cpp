/* 
@author Brian Gauch - CS396
*/

#include <sys/time.h>
#include <set>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "boxicity2/maxCliques.cpp"

using namespace std;

//const double WORLD_SIZE = 1.0;
const int MAX_RESOURCES = 100;

/*
struct point_t {
  double x, y;
  bool used;
};
*/

typedef point_t action_t[MAX_RESOURCES];

int factorial(int n)
{
  if (n <= 1)
    return 1;
  else
    return factorial(n - 1) * n;
}

/*
string p2str(point_t p)
{
  stringstream ss;
  ss << "(" << p.x << ", " << p.y << ")";
  return ss.str(); 
}
*/

double get_dist(point_t p1, point_t p2)
{
  double dx = abs(p1.x - p2.x);
  double dy = abs(p1.y - p2.y);
  double dist = pow((pow(dx,2) + pow(dy,2)), 0.5);
  return dist;
}

/*
void print_points(point_t points[], int num_points)
{
  string str = "{";
  for(int i=0; i<num_points; i++)
  {
    str += p2str(points[i]) + ", ";
  }
  str += "}\n";
  cout << str;
}
*/

/*
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
*/

bool good(point_t points[], int num_points, int x, int y)
{
   // check that two buildings are not in same location
   for(int i=0; i<num_points; i++)
      if (points[i].x == x && points[i].y == y)
         return false;
   return true;
}

void seedRand()
{
  struct timeval time; 
  gettimeofday(&time,NULL);

  // microsecond has 1 000 000
  // Assuming you did not need quite that accuracy
  // Also do not assume the system clock has that accuracy.
  srand((time.tv_sec * 1000) + (time.tv_usec / 1000));
}

void gen_random_points(point_t points[], int num_points, double max_x, double max_y)
{
  seedRand();
  double randx, randy;
  for(int i=0; i<num_points; i++)
  {
    // randx = my_rand(0, max_x);
    // randy = my_rand(0, max_y);
    randx = my_double_rand(0.0, max_x);
    randy = my_double_rand(0.0, max_y);
    //while (!good(points, num_points, randx, randy))
    //{
    //   randx = my_rand(0, max_x);
    //   randy = my_rand(0, max_y);
    //}

    points[i].x = randx;
    points[i].y = randy;
    points[i].used = false;
  }
}

/*
generates the combinatorial number of actions, given a set of 'good'
locations to consider
*/
void get_defender_actions(action_t actions[], int num_actions,
                          vector<point_t> good_locations,
                          int start_location, int num_defenders, int &curr_action)
{
  // check terminating condition
  if (num_defenders == 0)
  {
    int a=0;
    for (unsigned int i=0; i<good_locations.size(); i++)
      if (good_locations[i].used)
         actions[curr_action][a++] = good_locations[i];
    curr_action++;
  }

  // loop over all buildings
  else
  {
    for (unsigned int i=start_location; i<good_locations.size(); i++)
    {
       // select building
       if (!good_locations[i].used)
       {
          good_locations[i].used = true;
          get_defender_actions(actions, num_actions, good_locations, 
           i+1, num_defenders-1, curr_action);
          good_locations[i].used = false;
       }
    }
  }
}

// gets the (defender) utility of an action pair by determining if the attacker
// attacked a defended location, an using the value of the location
int get_state_utility(point_t a_action, 
           int num_resources, action_t d_action, double range)
{
  for(int i=0; i<num_resources; i++)
  {
    point_t resource_loc = d_action[i];
    double dist = get_dist(a_action, resource_loc);
    //printf("dist:%f\n", dist);
    if(dist <= range)
    {
      // caught attacker
      return 0;
    }
  }
  // todo: instead, return -value_of_destroyed_target
  return -1;
}

// zero sum
void init_game(int num_resources,
               int num_a_actions, point_t a_actions[], 
               int num_d_actions, action_t d_actions[], 
               double r, int* game )
{
  for(int i = 0; i < num_d_actions; i++)
  {
    for(int j = 0; j < num_a_actions; j++)
    {
      int index = i*num_a_actions + j;
      int utility =  get_state_utility(a_actions[j], 
                                          num_resources, d_actions[i], r);
      //printf("index:%d\n", index);
      //printf("utility:%d\n", utility);
      *(game+index) = utility;
    }
  }
}

void write_game_to_file(int num_a_actions, int num_d_actions, 
                        int* game, 
                        string file_name)
{
  ofstream Dout;
  Dout.open (file_name.c_str());
  if (Dout.fail())
  {
    cout << "Could not open " << file_name << endl;
  }
  else
  {
    //Dout << num_a_actions << " " << num_d_actions << endl << endl;
    Dout << num_d_actions << " " << num_a_actions << endl << endl;
    for(int i = 0; i < num_d_actions; i++)
    {
      for(int j = 0; j < num_a_actions; j++)
      {
        int index = i*num_a_actions + j;
        double utility = game[index];
        //printf(" %lf", -utility);
        Dout << -utility << " ";
      }
      //printf("\n");
      Dout << endl;
    }
    //printf("\n");
    Dout << endl;
    for(int i = 0; i < num_d_actions; i++)
    {
      for(int j = 0; j < num_a_actions; j++)
      {
        int index = i*num_a_actions + j;
        double utility = game[index];
        //printf(" %lf", utility);
        Dout << utility << " ";
      }
      //printf("\n");
      Dout << endl;
    }
  }
}

void printUsage()
{
  const char* usage = 
  "-----------------USAGE:----------------\n"
  "-r: effective range of each defender\n"
  "-d: number of defenders\n"
  "-b: number of buildings\n"
  "-f: output file name\n"
  "\n"
  "ex:\n"
  "./spe -r 0.1 -d 2 -b 4 -f 'matrix.txt'\n";
  
  printf("%s", usage);
}

/*
* Let main() just handle argument parsing
*/
void myMain(bool planar, double r, int d, int b, string o_fname)
{
  point_t buildings[b];
  gen_random_points(buildings, b, WORLD_SIZE, WORLD_SIZE);
  //cout << "---------- building locations: ----------\n";
  //print_points(buildings, b);
  vector<point_t> good_resource_locations;
  if(planar)
  {
    maxCliquesFromPoints(good_resource_locations,
                         buildings, b, r);
  }
  else
  {
    good_resource_locations.assign(buildings, buildings + b);
  }
  int n = good_resource_locations.size();
  point_t loc_arr[n];
  copy(good_resource_locations.begin(), good_resource_locations.end(), loc_arr);
  //cout << "---------- maximal clique points: ----------\n";
  //cout << n << " maximal cliques: \n";
  //print_points(loc_arr, n);

  int num_d_actions = factorial(n) / (factorial(d) * factorial(n-d));
  action_t d_actions[num_d_actions];
  int curr_action = 0;
  get_defender_actions(d_actions, num_d_actions, good_resource_locations, 0, d, curr_action);

  //cout << "---------- " << num_d_actions 
  //     << " defender actions (sets of size " << d << "): ----------\n";
  /*
  for (int a = 0; a < num_d_actions; a++)
  {
     print_points(d_actions[a], d);
     cout << endl;
  }
  */

  int game[b][num_d_actions];
  int* game_ptr = &game[0][0];
  init_game(d, b, buildings, num_d_actions, d_actions, r, game_ptr);
  //cout << "---------- Payoff Matrices: ----------" << endl;
  write_game_to_file(b, num_d_actions, game_ptr, o_fname);
}

int main (int argc, char **argv)
{
  int index;
  int c;

  // effective range
  double r = 0.00001;
  // # defense resources
  int d = 2;
  // # targets aka buildings
  int b = 5;
  // output filename
  string f = "output_game_matrix";
  // use planar model and find maximal cliques
  bool p = false;

  opterr = 0;
  while ((c = getopt (argc, argv, "p:r:d:b:f:")) != -1)
    switch (c)
      {
      case 'r':
        r = atof(optarg);
        break;
      case 'd':
        d = atoi(optarg);
        break;
      case 'b':
        b = atoi(optarg);
        break;
      case 'f':
        f = optarg;
        break;
      case 'p':
        p = atoi(optarg) > 0;
        break;
      case '?':
        if (optopt == 'r' || optopt == 'd' || optopt == 'b')
          fprintf (stderr, "Option -%c requires an argument.\n", optopt);
        else if (isprint (optopt))
          fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        else
          fprintf (stderr,
                   "Unknown option character `\\x%x'.\n",
                   optopt);
        printUsage();
        return 1;
      default:
        abort ();
      }
  //printf ("r = %f, d = %d, b = %d\n", r, d, b);

  for (index = optind; index < argc; index++)
    printf ("Non-option argument %s\n", argv[index]);

  myMain(p,r,d,b,f);
  return 0;
}
