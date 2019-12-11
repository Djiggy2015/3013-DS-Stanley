//**************************************************************
//                       Matt Stanley
//                   Program 5 - Monster Maze
//                      3103 - Algorithms
//                        12/10/2019
//**************************************************************
// This program uses a pathing algorithm in order to
// make a "hunter" find a path to a "monster". This is
// implemented by creating a two dimensional array of cell
// structures, each cell containing a row and a column.
// A player structure is also implemented that will be used
// to keep track of the hunter and monster's locations. 
// To make the hunter chase after the monster, the program
// calculates the distance of each neighboring cell relative
// to the hunter's current position to the cell that the 
// monster is in. Through many repetitions of this procedure
// the hunter should eventually make their mark. Five 
// different variations of this pathfinding program are 
// implemented, each with their own twist. This program outputs
// graphics to the console through the use of TermGame.h, a 
// library created by Jeremy Glebe.
//***************************************************************

#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include "TermGame.h"

using namespace std;
namespace TG = TermGame;
namespace TP = TermPrint;

// Mainly used to control player icon and color
struct Player
{
  int row;
  int col;
  string ch;
  unsigned short foreground;
  unsigned short background;

  // Default constructor
  Player()
  {
    row = 0;
    col = 0;
    foreground = 8;
    background = 1;
    ch = ". ";
  }

  Player(int r, int c, string s)
  {
    row = r;
    col = c;
    foreground = 8;
    background = 1;
    ch = s;
  }

  Player(int r, int c, string s, unsigned short fore, 
  unsigned short back = 1)
  {
    row = r;
    col = c;
    foreground = fore;
    background = back;
    ch = s;
  }

  // Sets the game's colors
  void SetColor(unsigned short fore, unsigned short back = 1)
  {
    foreground = fore;
    background = back;
  }

  void Print()
  {
    TP::print(ch, foreground, background);
  }

};

struct Cell
{
  int row;
  int col;
  bool visited;
  bool wall;
  string ch;

  Cell(int r = 0, int c = 0)
  {
    row = r;
    col = c;
    ch = " ";
    wall = false;
    visited = false;
  }

  // Prints out the contents of the cell
  void PrintCell(int x = 1)
  {
    TP::print(ch, 8, x, false);
  }

  // Sets the cell to be empty
  void GetPlayer() {
        ch = " ";
    }

  // Sets the cell to be the player icon
    void SetPlayer(string c) {
        ch = c;
    }

    void SetVisited()
    {
      visited = true;
      ch = ".";
    }

    void MakeWall() {
        wall = true;
        ch = "#";
    }

    bool GetWall()
    {
      return wall;
    }
};

// This function uses the taxicab distance formula, plus
// the distance currently moved in order to select the player's 
// next move.
double Distance(Cell * c1, Player p2, int cost_so_far)
{
  double dx = abs(p2.col - c1->col);
  double dy = abs(p2.row - c1->row);

  return (dx + dy) + cost_so_far;
}

// Movement for the monster. Unlike the hunter's movement,
// this formula is interested in larger distances from 
// the target. 
double monsterDist(Cell * c1, Player p1)
{
  double dx = p1.row - c1->row;
  double dy = p1.col - c1->col;

  return abs(dy) + abs(dx);
}

class Arena{
private:
  int height;
  int width;
  Cell * playercell;
  Cell * monstercell;

public:
  Cell ***arena;
  // Arena constructor
  Arena(int rows = 20, int cols = 20)
  {
    height = rows;
    width = cols;

    arena = new Cell **[height];

    for (int i = 0; i < height; i++)
    {
      arena[i] = new Cell *[width];
    }

    for (int i = 0; i < height; i++)
    {
      for (int j = 0; j < width; j++)
      {
        arena[i][j] = new Cell(i, j);
      }
    }

    resetArena();
  }

  // Prints out the contents of the arena
  void printArena(int x = 1)
  {
    for(int i = 0; i < height; i++){
      for (int j = 0; j < width; j++){
        arena[i][j]->PrintCell(x);
      }
      cout << endl;
    }
  }

// Resets the board
void resetArena()
{
  for (int i = 0; i < height; i++)
  {
    for(int j = 0; j < width; j++)
    {

      arena[i][j]->visited = false;

      arena[i][j]->ch = " ";
    }
  }
}

// This function determines whether the current cell being 
// analyzed can be moved to or not. 
bool LegalMove(int x, int y) {

  // Make sure the cell is within bounds
  if((x < 0 || x >= height) || (y < 0 || y >= width))
  {
    return false;
  }

  // Make sure you are not moving somewhere that has already
  // been visited
  if(arena[x][y]->visited)
  {
    return false;
  }

  // Makes sure the hunter does not go through walls.
  if(arena[x][y]->wall)
  {
    return false;
  }

  // Makes sure the hunters don't occupy the same space.
  if(arena[x][y]->ch == "🤺")
  {
    return false;
  }

  return true;
}


// Find the best possible move given current circumstances
bool traverseArena(Player p1, Player p2, vector<Player> &vect, 
int &cost_so_far, bool breadcrumbs = true)
{
  Cell * current;
  Cell * neighbor;
  Player temp;
  vector<Cell *> moves;
  int best_index = 0;
  bool movable;

  // Player row and column
  int row = p1.row;
  int col = p1.col;

  current = arena[row][col];

  // Keep track of movements
  if(!current->visited && breadcrumbs == true)
  {
    current->visited = true;
    current->ch = ".";
  }

  moves = find_neighbors(current->row, current->col);

  best_index = best_choice(moves, p2, cost_so_far);

  // if there are any moves
  if (moves.size())
  {
  neighbor = moves[best_index];

  temp.row = neighbor->row;
  temp.col = neighbor->col;

  vect.push_back(temp);
  cost_so_far++;
  return true;
  }

  return false;
}

// This function is used to move the monster
void Monstermove(Player p1, Player p2, vector<Player> &vect, bool breadcrumbs = false)
{
  Cell * current;
  Cell * neighbor;
  Player temp;
  vector<Cell *> moves;
  int best_index = 0;
  bool movable;

  // Monster row and column
  int row = p2.row;
  int col = p2.col;

  current = arena[row][col];

  // Keep track of movements
  if(!current->visited && breadcrumbs == true)
  {
    current->visited = true;
    current->ch = ".";
  }

  moves = find_neighbors(current->row, current->col);

  best_index = monster_choice(moves, p1);

  // if there are any moves
  if (moves.size())
  {
  neighbor = moves[best_index];

  temp.row = neighbor->row;
  temp.col = neighbor->col;

  vect.push_back(temp);
  }

}


// This function will show all of the possible moves
// that can currently be taken.
vector<Cell *> find_neighbors(int row, int col)
{
  vector<Cell *> Neighbors;

  // Find all legal moves
  if(LegalMove(row + 1, col) == true)
  {
    Neighbors.push_back(arena[row + 1][col]);
  }

  if(LegalMove(row, col + 1) == true)
  {
    Neighbors.push_back(arena[row][col + 1]);
  }

  if(LegalMove(row - 1, col) == true)
  {
    Neighbors.push_back(arena[row - 1][col]);
  }

  if(LegalMove(row, col - 1) == true)
  {
    Neighbors.push_back(arena[row][col - 1]);
  }

  return Neighbors;  
}

// Returns the best available choice
int best_choice(vector<Cell *> moves, Player p2, int cost_so_far)
{
  int mini = 99999;
  int d = 0;
  int i = 0;

  for (int j = 0; j < moves.size(); j++)
  {
    d = Distance(moves[j], p2, cost_so_far);

    if (d < mini){
      mini = d;
      i = j;
    }
  }

  return i;
}

int monster_choice(vector<Cell *> moves, Player p1)
{
  int max = -99999;
  int d = 0;
  int i = 0;

  for(int j = 0; j < moves.size(); j++)
  {
    d = monsterDist(moves[j], p1);

    if (d > max){
      max = d;
      i = j;
    }
  }

  return i;
}

};

//***************************************************
// Function Name: Level 1
// Description: This function is used to run level
// 1 of "mazehunter". In this level, the hunter goes
// after the monster once. After reaching it, the 
// next level starts.
//***************************************************
void Level1()
{
    int rows = 20;
    int cols = 20;
    vector<Player> moves;
    bool can_move;

    // This keeps track of how far the hunter has 
    // moved.
    int cost_so_far = 1;

    Arena A1(rows, cols);
    
    // Generate random spots for players to start.
    int r1 = rand() % (rows - 1);
    int r2 = rand() % (cols - 1);
    int r3 = rand() % (rows - 1);
    int r4 = rand() % (cols - 1);

    // Create two points where player and
    // monster will start
    Player p1(r1, r2, "🤺");
    Player p2(r3, r4, "🦖");

    // Put the hunter and monser in the grid
    A1.arena[p1.row][p1.col]->SetPlayer("🤺");
    A1.arena[p2.row][p2.col]->SetPlayer("🦖");

    moves.push_back(p1);

    bool found = false;
    
  
    while(!found)
    {
      TP::clear();
      A1.arena[p1.row][p1.col]->GetPlayer();
      can_move = A1.traverseArena(p1, p2, moves, cost_so_far);

      if(can_move)
      {
        p1 = moves.back();
        A1.arena[p1.row][p1.col]->SetPlayer("🤺");
      }

      // Time to backtrack!
      else
      {
        // Go to the move before last
        moves.pop_back();
        
        p1 = moves.back();
        A1.arena[p1.row][p1.col]->SetPlayer("🤺");
      }

      if(p1.row == p2.row && p1.col == p2.col)
      {
        found = true;

        A1.arena[p1.row][p1.col]->GetPlayer();
        A1.arena[p2.row][p2.col]->GetPlayer();
      }

      A1.printArena();
      cout << "Level 1" << endl;

      // Makes sure the program does not run too quickly.
      TG::sleep(200);
    }
}


//*************************************************
// Function Name: Level2
// Description: This function is pretty much the same
// as level 1, but it plays until the hunter has caught
// the monster twice.
//**************************************************
void Level2()
{
    int rows = 20;
    int cols = 20;
    vector<Player> moves;
    bool can_move;
    int cost_so_far = 1;

    Arena A1(rows, cols);
    
    // Generate random spots for players to start.
    int r1 = rand() % (rows - 1);
    int r2 = rand() % (cols - 1);
    int r3 = rand() % (rows - 1);
    int r4 = rand() % (cols - 1);

    // Create two points where player and
    // monster will start
    Player p1(r1, r2, "🤺");
    Player p2(r3, r4, "🦖");

    // Put the hunter and monser in the grid
    A1.arena[p1.row][p1.col]->SetPlayer("🤺");
    A1.arena[p2.row][p2.col]->SetPlayer("🦖");

    moves.push_back(p1);

    // This is used to control number of times
    // hunter finds monster
    int count = 0;
    
    // Play the game until the monster has been caught
    // twice.
    while(count < 2)
    {
      TP::clear();
      A1.arena[p1.row][p1.col]->GetPlayer();
      can_move = A1.traverseArena(p1, p2, moves, cost_so_far);

      if(can_move)
      {
        p1 = moves.back();
        A1.arena[p1.row][p1.col]->SetPlayer("🤺");
      }

      // Time to backtrack!
      else
      {
        // Go to the move before last
        moves.pop_back();
        
        p1 = moves.back();
        A1.arena[p1.row][p1.col]->SetPlayer("🤺");
      }

      // What to do when the hunter has caught the monster
      if(p1.row == p2.row && p1.col == p2.col)
      {
        count++;

        A1.arena[p1.row][p1.col]->GetPlayer();
        A1.arena[p2.row][p2.col]->GetPlayer();

        // Generate the random locations again
        r1 = rand() % (rows - 1);
        r2 = rand() % (cols - 1);
        r3 = rand() % (rows - 1);
        r4 = rand() % (cols - 1);

        p1.row = r1;
        p1.col = r2;
        p2.row = r3;
        p2.col = r4;

        A1.resetArena();

        // Set the monster back in the game
        A1.arena[p2.row][p2.col]->SetPlayer("🦖");
      }

      A1.printArena(7);
      cout << "Level 2" << endl;
      TG::sleep(200);
    }
}


//*************************************************
// Function Name: Level3
// Description: In this function, the monster will 
// actively try to run from the hunter. Because this 
// function can sometimes end in a stalemate, a counter
// is included to end the level after 100 moves,
//**************************************************
void Level3()
{
    int rows = 10;
    int cols = 10;
    vector<Player> pmoves;
    vector<Player> mmoves;
    bool can_move;
    int cost_so_far = 1;
    int counter = 0;

    Arena A1(rows, cols);
    
    // Generate random spots for players to start.
    int r1 = rand() % (rows - 1);
    int r2 = rand() % (cols - 1);
    int r3 = rand() % (rows - 1);
    int r4 = rand() % (cols - 1);

    // Create two points where player and
    // monster will start
    Player p1(r1, r2, "🤺");
    Player p2(r3, r4, "🦖");

    // Put the hunter and monser in the grid
    A1.arena[p1.row][p1.col]->SetPlayer("🤺");
    A1.arena[p2.row][p2.col]->SetPlayer("🦖");

    pmoves.push_back(p1);
    mmoves.push_back(p2);

    bool found = false;
    
    // Play the game until the monster has been caught.
    while(!found && counter < 100)
    {

      TP::clear();
      A1.arena[p1.row][p1.col]->GetPlayer();
      A1.arena[p2.row][p2.col]->GetPlayer();

      can_move = A1.traverseArena(p1, p2, pmoves, cost_so_far, true);

      A1.Monstermove(p1, p2, mmoves);
      p2 = mmoves.back();
      A1.arena[p2.row][p2.col]->SetPlayer("🦖");

      if(can_move)
      {
        p1 = pmoves.back();
        A1.arena[p1.row][p1.col]->SetPlayer("🤺");
      }

      // Time to backtrack!
      else
      {
        // Go to the move before last
        pmoves.pop_back();
        
        p1 = pmoves.back();
        A1.arena[p1.row][p1.col]->SetPlayer("🤺");
      }

      if(p1.row == p2.row && p1.col == p2.col)
      {
        found = true;

        A1.arena[p1.row][p1.col]->GetPlayer();
        A1.arena[p2.row][p2.col]->GetPlayer();
      }

      counter ++;
      A1.printArena(6);
      cout << "Level 3" << endl;
      TG::sleep(200);
    }
}


//***************************************************
// Function Name: Level 4
// Description: This function is used to run level
// 4 of mazehunter. In this level, some walls are created
// to try and confuse the hunter or get him stuck. 
// Backtracking is very useful for this stage.
//***************************************************
void Level4()
{
    int rows = 20;
    int cols = 20;
    vector<Player> moves;
    bool can_move;
    int cost_so_far = 1;

    Arena A1(rows, cols);
    

    // Create two points where player and
    // monster will start. These points are not
    // randomized, because a player can randomly
    // be put inside a wall.
    Player p1(18, 2, "🤺");
    Player p2(4, 2, "🦖");

    // Put the hunter and monser in the grid
    A1.arena[p1.row][p1.col]->SetPlayer("🤺");
    A1.arena[p2.row][p2.col]->SetPlayer("🦖");

    // Create some obstacles to make life harder for
    // the hunter.
    for(int i = 0; i < 18; i++)
    {
      A1.arena[5][i]->MakeWall();
    }

    for(int i = 0; i < 17; i++)
    {
      A1.arena[16][i]->MakeWall();
    }

    for(int i = 19; i > 4; i--)
    {
      A1.arena[10][i]->MakeWall();
    }

    for(int i = 0; i < 5; i++)
    {
      A1.arena[2][i]->MakeWall();
    }

    for(int i = 4; i > 1; i--)
    {
      A1.arena[i][7]->MakeWall();
    }

    moves.push_back(p1);

    bool found = false;
    
  
    while(!found)
    {
      TP::clear();
      A1.arena[p1.row][p1.col]->GetPlayer();
      can_move = A1.traverseArena(p1, p2, moves, cost_so_far);

      if(can_move)
      {
        p1 = moves.back();
        A1.arena[p1.row][p1.col]->SetPlayer("🤺");
      }

      // Time to backtrack!
      else
      {
        // Go to the move before last
        moves.pop_back();
        
        p1 = moves.back();
        A1.arena[p1.row][p1.col]->SetPlayer("🤺");
        cost_so_far -= 1;
      }

      if(p1.row == p2.row && p1.col == p2.col)
      {
        found = true;

        A1.arena[p1.row][p1.col]->GetPlayer();
        A1.arena[p2.row][p2.col]->GetPlayer();
      }

      A1.printArena(2);
      cout << "Level 4" << endl;
      TG::sleep(200);
    }
}


//*************************************************
// Function Name: Level5
// Description: In this level, multiple hunters will 
// go after one monster.
//*************************************************
void Level5()
{
    int rows = 20;
    int cols = 20;
    vector<Player> p1moves;
    vector<Player> p3moves;
    vector<Player> mmoves;
    bool can_move;
    int cost_so_far1 = 1;
    int cost_so_far2 = 1;
    int counter = 0;

    Arena A1(rows, cols);
    
    // Generate random spots for players to start.
    int r1 = rand() % (rows - 1);
    int r2 = rand() % (cols - 1);
    int r3 = rand() % (rows - 1);
    int r4 = rand() % (cols - 1);
    int r5 = rand() % (rows - 1);
    int r6 = rand() % (cols - 1);

    // Create two points where player and
    // monster will start
    Player p1(r1, r2, "🤺");
    Player p2(r3, r4, "🦖");
    Player p3(r5, r4, "🤺");

    // Put the hunter and monser in the grid
    A1.arena[p1.row][p1.col]->SetPlayer("🤺");
    A1.arena[p2.row][p2.col]->SetPlayer("🦖");
    A1.arena[p3.row][p3.col]->SetPlayer("🤺");

    p1moves.push_back(p1);
    mmoves.push_back(p2);

    bool found = false;
    
    // Play the game until the monster has been caught.
    while(!found && counter < 100)
    {

      TP::clear();
      A1.arena[p1.row][p1.col]->GetPlayer();
      A1.arena[p2.row][p2.col]->GetPlayer();
      A1.arena[p3.row][p3.col]->GetPlayer();

      can_move = A1.traverseArena(p1, p2, p1moves, cost_so_far1, true);

      if(can_move)
      {
        p1 = p1moves.back();
        A1.arena[p1.row][p1.col]->SetPlayer("🤺");
      }

      // Time to backtrack!
      else
      {
        // Go to the move before last
        p1moves.pop_back();
        
        p1 = p1moves.back();
        A1.arena[p1.row][p1.col]->SetPlayer("🤺");
      }


      can_move = A1.traverseArena(p3, p2, p3moves, cost_so_far2, true);

      if(can_move)
      {
        p3 = p3moves.back();
        A1.arena[p3.row][p3.col]->SetPlayer("🤺");
      }

      // Time to backtrack!
      else
      {
        // Go to the move before last
        p3moves.pop_back();
        
        p3 = p3moves.back();
        A1.arena[p3.row][p3.col]->SetPlayer("🤺");
      }

      A1.Monstermove(p1, p2, mmoves);
      p2 = mmoves.back();
      A1.arena[p2.row][p2.col]->SetPlayer("🦖");

      if((p1.row == p2.row && p1.col == p2.col) || (p3.row == p2.row && p3.col == p2.col))
      {
        found = true;

        A1.arena[p1.row][p1.col]->GetPlayer();
        A1.arena[p2.row][p2.col]->GetPlayer();
        A1.arena[p3.row][p3.col]->GetPlayer();
      }

      counter ++;
      A1.printArena(6);
      cout << "Level 5" << endl;
      TG::sleep(200);
    }
}


int main()
{

  srand(time(0));

  Level1();

  Level2();

  Level3();

  Level4();

  Level5();

  return 0;
}
