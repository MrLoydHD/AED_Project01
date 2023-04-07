//
// AED, August 2022 (Tomás Oliveira e Silva)
//
// First practical assignement (speed run)
//
// Compile using either
//   cc -Wall -O2 -D_use_zlib_=0 solution_speed_run.c -lm
// or
//   cc -Wall -O2 -D_use_zlib_=1 solution_speed_run.c -lm -lz
//
// Place your student numbers and names here
//   N.Mec. XXXXXX  Name: XXXXXXX
//


//
// static configuration
//
//#define SOLUTION_SPEED_RUN_A
//#define SOLUTION_SPEED_RUN_B
#define SOLUTION_SPEED_RUN_C
//#define SOLUTION_SPEED_RUN_D
//#define SOLUTION_SPEED_RUN_E

#define _max_road_size_  800  // the maximum problem size
#define _min_road_speed_   2  // must not be smaller than 1, shouldnot be smaller than 2
#define _max_road_speed_   9  // must not be larger than 9 (only because of the PDF figure)


//
// include files --- as this is a small project, we include the PDF generation code directly from make_custom_pdf.c
//

#include <math.h>
#include <stdio.h>
#include "../P02/elapsed_time.h"
#include "make_custom_pdf.c"


//
// road stuff
//

static int max_road_speed[1 + _max_road_size_]; // positions 0.._max_road_size_

static void init_road_speeds(void)
{
  double speed;
  int i;

  for(i = 0;i <= _max_road_size_;i++)
  {
    speed = (double)_max_road_speed_ * (0.55 + 0.30 * sin(0.11 * (double)i) + 0.10 * sin(0.17 * (double)i + 1.0) + 0.15 * sin(0.19 * (double)i));
    max_road_speed[i] = (int)floor(0.5 + speed) + (int)((unsigned int)random() % 3u) - 1;
    if(max_road_speed[i] < _min_road_speed_)
      max_road_speed[i] = _min_road_speed_;
    if(max_road_speed[i] > _max_road_speed_)
      max_road_speed[i] = _max_road_speed_;
  }
}


//
// description of a solution
//

typedef struct
{
  int n_moves;                         // the number of moves (the number of positions is one more than the number of moves)
  int positions[1 + _max_road_size_];  // the positions (the first one must be zero)
  int spd[1 + _max_road_size_];
}
solution_t;



//the (very inefficient) recursive solution given to the students


static solution_t solution_1,solution_1_best;
static double solution_1_elapsed_time; // time it took to solve the problem
static unsigned long solution_1_count; // effort dispended solving the problem

static void solution_1_recursion(int move_number,int position,int speed,int final_position)
{
  int i,new_speed;

  // record move
  solution_1_count++;
  solution_1.positions[move_number] = position;
  // is it a solution?
  if(position == final_position && speed == 1)
  {
    // is it a better solution?
    if(move_number < solution_1_best.n_moves)
    {
      solution_1_best = solution_1;
      solution_1_best.n_moves = move_number;
    }
    return;
  }
  // no, try all legal speeds
  for(new_speed = speed - 1;new_speed <= speed + 1;new_speed++)
    if(new_speed >= 1 && new_speed <= _max_road_speed_ && position + new_speed <= final_position)
    {
      for(i = 0;i <= new_speed && new_speed <= max_road_speed[position + i];i++)
        ;
      if(i > new_speed)
        solution_1_recursion(move_number + 1,position + new_speed,new_speed,final_position);
    }
}

static void solve_1(int final_position)
{
  if(final_position < 1 || final_position > _max_road_size_)
  {
    fprintf(stderr,"solve_1: bad final_position\n");
    exit(1);
  }
  solution_1_elapsed_time = cpu_time();
  solution_1_count = 0ul;
  solution_1_best.n_moves = final_position + 100;
  solution_1_recursion(0,0,0,final_position);
  solution_1_elapsed_time = cpu_time() - solution_1_elapsed_time;
}


//solution dos alunos

static solution_t solution_2,solution_2_best, solution_2_before;
static double solution_2_elapsed_time; // time it took to solve the problem
static unsigned long solution_2_count; // effort dispended solving the problem
static int solution_2_beforespeed;
static void solution_2_while(int move_number,int position,int speed,int final_position)
{

  int diff_speeds[3] = {1,0,-1};
  int ending = 0;
  int save=0, saved = 0;
  int possiblemove = 1;

  while (position != final_position) {
    solution_2_count++;
    solution_2.positions[move_number] = position;

    for (int i = 0; i < 3; i++) {

      possiblemove = 1;
      int new_speed = speed + diff_speeds[i];
      int next_position = position + new_speed;

      if (new_speed < 1 || new_speed > _max_road_speed_ || max_road_speed[position] < new_speed) continue;
      
      int speed_to_check = new_speed;
      int x_to_speed = speed_to_check;
      for (int j = 1; j <= (new_speed * (new_speed + 1) / 2); j++) {
        if (position + j > final_position) {
          possiblemove = 0;
          save = 1;
          break;
        }
        
        if (x_to_speed == 0) {
          speed_to_check -= 1;
          x_to_speed = speed_to_check;
        }

        if (max_road_speed[position + j] < speed_to_check) {
          
          possiblemove = 0;

          break;
        }
        x_to_speed -= 1;
      }

      if (save && !saved) {
        solution_2_before = solution_2;
        solution_2_before.n_moves = move_number;
        solution_2_beforespeed = speed;
        saved = 1;
      }
      if (possiblemove) {
        position += new_speed;
        speed = new_speed;
        move_number++;
        break;
      }
    }

  }
  solution_2.positions[move_number] = position;
  solution_2_best = solution_2;
  solution_2_best.n_moves = move_number;
  return;
}

static void solve_2(int final_position)
{
  if(final_position < 1 || final_position > _max_road_size_)
  {
    fprintf(stderr,"solve_2: bad final_position\n");
    exit(1);
  }
  solution_2_elapsed_time = cpu_time();
  solution_2_count = 0ul;
  solution_2_best.n_moves = final_position + 100;
  solution_2 = solution_2_before;
  solution_2_while(solution_2_before.n_moves,solution_2_before.positions[solution_2_before.n_moves],solution_2_beforespeed,final_position);
  //int move_number,int position,int speed,int final_position
  solution_2_elapsed_time = cpu_time() - solution_2_elapsed_time;
}

//solução 2 dos alunos (solução do professor melhorada)

static solution_t solution_3,solution_3_best;
static double solution_3_elapsed_time; // time it took to solve the problem
static unsigned long solution_3_count; // effort dispended solving the problem

static void solution_3_recursion(int move_number,int position,int speed,int final_position)
{
  int i,new_speed;

  // record move
  solution_3_count++;
  solution_3.positions[move_number] = position;
  // is it a solution?
  if(position == final_position && speed == 1)
  {
    // is it a better solution?
    if(move_number < solution_3_best.n_moves)
    {
      solution_3_best = solution_3;
      solution_3_best.n_moves = move_number;
    }
    return;
  }
  // no, try all legal speeds
  if (solution_3_best.positions[move_number] > position)
  {
    return;
  }
    for(new_speed = speed + 1;new_speed >= speed - 1;new_speed--)
    if(new_speed >= 1 && new_speed <= _max_road_speed_ && position + new_speed <= final_position)
    {
      for(i = 0;i <= new_speed && new_speed <= max_road_speed[position + i];i++)
        ;
      if(i > new_speed)
        solution_3_recursion(move_number + 1,position + new_speed,new_speed,final_position);
    }
}

static void solve_3(int final_position)
{
  if(final_position < 1 || final_position > _max_road_size_)
  {
    fprintf(stderr,"solve_3: bad final_position\n");
    exit(1);
  }
  solution_3_elapsed_time = cpu_time();
  solution_3_count = 0ul;
  solution_3_best.n_moves = final_position + 100;
  solution_3_recursion(0,0,0,final_position);
  solution_3_elapsed_time = cpu_time() - solution_3_elapsed_time;
}

static solution_t solution_4,solution_4_best;
static double solution_4_elapsed_time; // time it took to solve the problem
static unsigned long solution_4_count; // effort dispended solving the problem


static void solution_4_while(int move_number,int position,int speed,int final_position)
{
  int diff_speeds[3] = {1,0,-1};
  int possiblemove = 1;
  
  while (position != final_position) {
    solution_4.positions[move_number] = position;
    for (int i = 0; i < 3; i++) {
      solution_4_count++;
      possiblemove = 1;
      int new_speed = speed + diff_speeds[i];
      int next_position = position + new_speed;

      if (new_speed < 1 || new_speed > _max_road_speed_ || max_road_speed[position] < new_speed) continue;
      
      int speed_to_check = new_speed;
      int x_to_speed = speed_to_check;
      for (int j = 1; j <= (new_speed * (new_speed + 1) / 2); j++) {
        if (position + j > final_position) {
          possiblemove = 0;
          break;
        }
        
        if (x_to_speed == 0) {
          speed_to_check -= 1;
          x_to_speed = speed_to_check;
        }

        if (max_road_speed[position + j] < speed_to_check) {
          
          possiblemove = 0;

          break;
        }
        x_to_speed -= 1;
      }

      if (possiblemove) {
        position += new_speed;
        speed = new_speed;
        move_number++;
        break;
      }
    }

  }
  solution_4.positions[move_number] = position;
  solution_4_best = solution_4;
  solution_4_best.n_moves = move_number;
  return;
}

static void solve_4(int final_position)
{
  if(final_position < 1 || final_position > _max_road_size_)
  {
    fprintf(stderr,"solve_4: bad final_position\n");
    exit(1);
  }
  solution_4_elapsed_time = cpu_time();
  solution_4_count = 0ul;
  solution_4_best.n_moves = final_position + 100;
  solution_4_while(0,0,0,final_position);
  //int move_number,int position,int speed,int final_position
  solution_4_elapsed_time = cpu_time() - solution_4_elapsed_time;
}
//------------------------------------------------------------

// typedef struct
// {
//   int depth;                      
//   int speed;
//   int valor;
//   int pos;
//   struct node *next;     
// } node;

// static solution_t solution_3,solution_3_best;
// static double solution_3_elapsed_time; // time it took to solve the problem
// static unsigned long solution_3_count; // effort dispended solving the problem

// static void solution_3_treesearch(int move_number, int position, int speed, int final_position)
// {
//   int new_speed, new_pos;
//   solution_2_count++;

//   node root;
//   root.speed = speed;
//   root.pos = position;
//   root.valor = max_road_speed[0];
//   root.depth = 0;

//   printf("%d",root.depth);

//   new_speed = speed;
//   new_pos = position;

  
  


//   while(1) {
//     // final position
//     if (new_pos == final_position && new_speed == 1) {
//       printf("Found solution in: %d", root.depth);
//     }
//   }
// }

// static void solve_3(int final_position)
// {
//   if(final_position < 1 || final_position > _max_road_size_)
//   {
//     fprintf(stderr,"solve_3: bad final_position\n");
//     exit(1);
//   }
//   solution_3_elapsed_time = cpu_time();
//   solution_3_count = 0ul;
//   solution_3_best.n_moves = final_position + 100;
//   solution_3_treesearch(0,0,0,final_position);
//   solution_3_elapsed_time = cpu_time() - solution_3_elapsed_time;
// }

// //------------------------------------------------------------


// typedef struct
// {
//   int n_moves;                         // the number of moves (the number of positions is one more than the number of moves)
//   int positions[1 + _max_road_size_];  // the positions (the first one must be zero)
// }
// solution_t;


// typedef struct node
// {
//   int pos;
//   int speed;
//   int depth;
//   struct node *parent;
// } node;

// static node inp_arr[31];
// static int Rear = - 1;
// static int Front = - 1;

// void enqueue(node nd)
// {
//     if (Rear == 31 - 1)
//         printf("Overflow \n");
//     else
//     {
//         if (Front == - 1)
      
//         Front = 0;
//         Rear = Rear + 1;
//         inp_arr[Rear] = nd;
//     }
// } 
 
// node dequeue()
// {
//     if (Front == - 1 || Front > Rear)
//     {
//         static node nothing;
//         nothing.pos = -100;
//         return nothing;
//     }
//     else
//     {
//         node ret = inp_arr[Front];
//         Front++;
//         return ret;
//     }
// } 

// static solution_t solution_4,solution_4_best;
// static double solution_4_elapsed_time; // time it took to solve the problem
// static unsigned long solution_4_count;

// static void solution_4_deque(int move_number, int position, int speed, int final_position)
// {
//   int new_speed, new_pos;
//   solution_4_count++;

//   struct node root;
//   move_number = 1;

//   root.pos = position;
//   root.speed = speed;
//   root.depth = 0;

//   enqueue(root);

//   while (1) {

//     solution_4_count++;

//     struct node nd;
//     nd = dequeue();

//     if (nd.pos == -100) {
//       // CANNOT DEQUEUE
//       return ;
//     }

//     position = nd.pos;
//     speed = nd.speed;

//     for (int i = 3; i > 0; i--) {
//       new_speed = speed + i - 2;
//       new_pos = position + new_speed;

//       if (new_speed < 1 || new_speed > _max_road_speed_) {
//         continue;
//       }

//       if (new_pos == final_position && new_speed == 1) {
//         solution_4_best = solution_4;
//         solution_4_best.n_moves = nd.depth;
//         return;
//       }

//       for (int i = position; i < position + new_speed; i++) {
//         if (max_road_speed[i] < new_speed) {
//           continue;
//         }
//       }

//       struct node node_new;
//       node_new.speed = new_speed;
//       node_new.pos = new_pos;
//       node_new.depth = nd.depth + 1;
//       *node_new.parent = nd;

//       enqueue(node_new);
//     }
//   }
// }

// static void solve_4(int final_position)
// {
//   if(final_position < 1 || final_position > _max_road_size_)
//   {
//     fprintf(stderr,"solve_4: bad final_position\n");
//     exit(1);
//   }
//   solution_4_elapsed_time = cpu_time();
//   solution_4_count = 0ul;
//   solution_4_best.n_moves = final_position + 100;
//   solution_4_deque(0,0,0,final_position);
//   solution_4_elapsed_time = cpu_time() - solution_4_elapsed_time;
// }

//------------------------------------------------------------------------------

static solution_t solution_5,solution_5_best;
static double solution_5_elapsed_time; // time it took to solve the problem
static unsigned long solution_5_count; // effort dispended solving the problem

static void solution_5_while(int move_number,int position,int speed,int final_position)
{
  int next_pos, new_spd, mid_pos;
  
  mid_pos = _max_road_size_/2;
  // printf("mid_pos= %d\n", mid_pos);
     
  //For position 0 and 1:
  solution_5_best.positions[0] = 0;
  solution_5_best.spd[0] = 0;
  solution_5_best.positions[1] = 1;
  solution_5_best.spd[1] = 1;
  position = 1;
  speed = 1;
  move_number = 2;
 
  while (position < final_position) {
  	if(position <= mid_pos){
    	new_spd = speed + 1;	
    	next_pos = position + new_spd;    
    	if ( new_spd > max_road_speed[next_pos] ){
    		new_spd = speed;	
    		next_pos = position + new_spd;  
    		if ( new_spd > max_road_speed[next_pos] ){
    			new_spd = speed - 1;
				if(new_spd < 1){
					new_spd = 1;	
    			next_pos = position + new_spd;
				}
				else{	
    				next_pos = position + new_spd;  
    				if ( new_spd > max_road_speed[next_pos] ){
    					new_spd = solution_5_best.spd[move_number - 1] - 1;	
    					next_pos = position + new_spd;  
    					move_number = move_number - 1;
					}
				}
			}
		}
	}
	else{
    	new_spd = speed - 1;
		if(new_spd < 1){
    	new_spd = 1;
		}	
    	next_pos = position + new_spd;    
    	if ( new_spd > max_road_speed[next_pos] ){
    		new_spd = solution_5_best.spd[move_number - 1] - 1;	
    		next_pos = position + new_spd;  
    		move_number = move_number - 1;
		}
		
	}

    position = next_pos;
    speed = new_spd;
	  solution_5_best.positions[move_number] = position;
	  solution_5_best.spd[move_number] = new_spd;
	  solution_5_best.n_moves = move_number - 1;
    move_number++;
    solution_5_count++;
  }
  
  if(position == final_position){
    move_number--;
	  solution_5_best.positions[move_number] = position;
	  solution_5_best.spd[move_number] = 0;
	  solution_5_best.n_moves = move_number;
  }
	// printf("pos= %d, move= %d, sol_cont= %lu\n", position, move_number, solution_5_count);
  
}

static void solve_5(int final_position)
{
  if(final_position < 1 || final_position > _max_road_size_)
  {
    fprintf(stderr,"solve_5: bad final_position\n");
    exit(1);
  }
  solution_5_elapsed_time = cpu_time();
  solution_5_count = 0ul;
  solution_5_best.n_moves = final_position + 100;
  solution_5_while(0,0,0,final_position);
  solution_5_elapsed_time = cpu_time() - solution_5_elapsed_time;
}



//example of the slides

static void example(void)
{
  int i,final_position;

  srandom(0xAED2022);
  init_road_speeds();
  final_position = 30;
  solve_1(final_position);
  make_custom_pdf_file("example.pdf",final_position,&max_road_speed[0],solution_1_best.n_moves,&solution_1_best.positions[0],solution_1_elapsed_time,solution_1_count,"Plain recursion");
  printf("mad road speeds:");
  for(i = 0;i <= final_position;i++)
    printf(" %d",max_road_speed[i]);
  printf("\n");
  printf("positions:");
  for(i = 0;i <= solution_1_best.n_moves;i++)
    printf(" %d",solution_1_best.positions[i]);
  printf("\n");
}

static void example2(void)
{
  int i,final_position;

  srandom(0xAED2022);
  init_road_speeds();
  final_position = 30;
  solve_2(final_position);
  make_custom_pdf_file("example.pdf",final_position,&max_road_speed[0],solution_2_best.n_moves,&solution_2_best.positions[0],solution_2_elapsed_time,solution_2_count,"While loop");
  printf("mad road speeds:");
  for(i = 0;i <= final_position;i++)
    printf(" %d",max_road_speed[i]);
  printf("\n");
  printf("positions:");
  for(i = 0;i <= solution_2_best.n_moves;i++)
    printf(" %d",solution_2_best.positions[i]);
  printf("\n");
}

static void example3(void)
{
  int i,final_position;

  srandom(0xAED2022);
  init_road_speeds();
  final_position = 30;
  solve_3(final_position);
  make_custom_pdf_file("example.pdf",final_position,&max_road_speed[0],solution_3_best.n_moves,&solution_3_best.positions[0],solution_3_elapsed_time,solution_3_count,"Plain recursion but better");
  printf("mad road speeds:");
  for(i = 0;i <= final_position;i++)
    printf(" %d",max_road_speed[i]);
  printf("\n");
  printf("positions:");
  for(i = 0;i <= solution_3_best.n_moves;i++)
    printf(" %d",solution_3_best.positions[i]);
  printf("\n");
}

static void example4(void)
{
  int i,final_position;

  srandom(0xAED2022);
  init_road_speeds();
  final_position = 30;
  solve_4(final_position);
  make_custom_pdf_file("example.pdf",final_position,&max_road_speed[0],solution_4_best.n_moves,&solution_4_best.positions[0],solution_4_elapsed_time,solution_4_count,"While loop but better");
  printf("mad road speeds:");
  for(i = 0;i <= final_position;i++)
    printf(" %d",max_road_speed[i]);
  printf("\n");
  printf("positions:");
  for(i = 0;i <= solution_4_best.n_moves;i++)
    printf(" %d",solution_4_best.positions[i]);
  printf("\n");
}

static void example5(void)
{
  int i,final_position;

  srandom(0xAED2022);
  init_road_speeds();
  final_position = 30;
  solve_5(final_position);
  make_custom_pdf_file("example.pdf",final_position,&max_road_speed[0],solution_5_best.n_moves,&solution_5_best.positions[0],solution_5_elapsed_time,solution_5_count,"Plain recursion");
  printf("mad road speeds:");
  for(i = 0;i <= final_position;i++)
    printf(" %d",max_road_speed[i]);
  printf("\n");
  printf("positions:");
  for(i = 0;i <= solution_5_best.n_moves;i++)
    printf(" %d",solution_5_best.positions[i]);
  printf("\n");
}

//
// main program
//

int main(int argc,char *argv[argc + 1])
{
# define _time_limit_  3600.0
  int n_mec,final_position,print_this_one;
  char file_name[64];

  // generate the example data
  if(argc == 2 && argv[1][0] == '-' && argv[1][1] == 'e' && argv[1][2] == 'x')
  {
    example5();
    return 0;
  }
  // initialization
  n_mec = (argc < 2) ? 0xAED2022 : atoi(argv[1]);
  srandom((unsigned int)n_mec);
  init_road_speeds();
  // run all solution methods for all interesting sizes of the problem
  final_position = 1;
  solution_2_elapsed_time = 0.0;
  printf("    + --- ---------------- --------- +\n");
  printf("    |                plain recursion |\n");
  printf("--- + --- ---------------- --------- +\n");
  printf("  n | sol            count  cpu time |\n");
  printf("--- + --- ---------------- --------- +\n");
  while(final_position <= _max_road_size_/* && final_position <= 20*/)
  {
    print_this_one = ( final_position == 10 || final_position == 20 || final_position == 50 || final_position == 100 || final_position == 200 || final_position == 400 || final_position == 800) ? 1 : 0;
    printf("%3d |",final_position);
    // first solution method (very bad)
    #ifdef SOLUTION_SPEED_RUN_A
    if(solution_1_elapsed_time < _time_limit_)
    {
      solve_1(final_position);
      if(print_this_one != 0)
      {
        sprintf(file_name,"%03d_1_%s.pdf",final_position,argv[1]);
        make_custom_pdf_file(file_name,final_position,&max_road_speed[0],solution_1_best.n_moves,&solution_1_best.positions[0],solution_1_elapsed_time,solution_1_count,"Plain recursion");
      }
      printf(" %3d %16lu %9.3e |",solution_1_best.n_moves,solution_1_count,solution_1_elapsed_time);
    }
    else
    {
      solution_1_best.n_moves = -1;
      printf("                                |");
    }
    #endif
    // second solution method (less bad)
    // ...
    #ifdef SOLUTION_SPEED_RUN_B
    if(solution_2_elapsed_time < _time_limit_)
    {
      solve_2(final_position);
      if(print_this_one != 0)
      {
        sprintf(file_name,"%03d_2.pdf",final_position);
        make_custom_pdf_file(file_name,final_position,&max_road_speed[0],solution_2_best.n_moves,&solution_2_best.positions[0],solution_2_elapsed_time,solution_2_count,"Plain recursion");
      }
      printf(" %3d %16lu %9.3e |",solution_2_best.n_moves,solution_2_count,solution_2_elapsed_time);
    }
    else
    {
      solution_2_best.n_moves = -1;
      printf("                                |");
    }
    #endif
    // third solution method (less bad)
    // ...
    #ifdef SOLUTION_SPEED_RUN_C
    if(solution_3_elapsed_time < _time_limit_)
    {
      solve_3(final_position);
      if(print_this_one != 0)
      {
        sprintf(file_name,"%03d_3.pdf",final_position);
        make_custom_pdf_file(file_name,final_position,&max_road_speed[0],solution_3_best.n_moves,&solution_3_best.positions[0],solution_3_elapsed_time,solution_3_count,"Plain recursion");
      }
      printf(" %3d %16lu %9.3e |",solution_3_best.n_moves,solution_3_count,solution_3_elapsed_time);
    }
    else
    {
      solution_3_best.n_moves = -1;
      printf("                                |");
    }
    #endif
    // fourth solution method (less bad)
    // ...
    #ifdef SOLUTION_SPEED_RUN_D
    if(solution_4_elapsed_time < _time_limit_)
    {
      solve_4(final_position);
      if(print_this_one != 0)
      {
        sprintf(file_name,"%03d_4.pdf",final_position);
        make_custom_pdf_file(file_name,final_position,&max_road_speed[0],solution_4_best.n_moves,&solution_4_best.positions[0],solution_4_elapsed_time,solution_4_count,"Plain recursion");
      }
      printf(" %3d %16lu %9.3e |",solution_4_best.n_moves,solution_4_count,solution_4_elapsed_time);
    }
    else
    {
      solution_4_best.n_moves = -1;
      printf("                                |");
    }
    #endif

    #ifdef SOLUTION_SPEED_RUN_E
    if(solution_5_elapsed_time < _time_limit_)
    {
      solve_5(final_position);
      if(print_this_one != 0)
      {
        sprintf(file_name,"%03d_1.pdf",final_position);
        make_custom_pdf_file(file_name,final_position,&max_road_speed[0],solution_5_best.n_moves,&solution_5_best.positions[0],solution_5_elapsed_time,solution_5_count,"Plain recursion");
      }
      printf(" %3d %16lu %9.3e |",solution_5_best.n_moves,solution_5_count,solution_5_elapsed_time);
    }
    else
    {
      solution_5_best.n_moves = -1;
      printf("                                |");
    }
    #endif

    //done 
    printf("\n");
    fflush(stdout);
    // new final_position
    if(final_position < 50)
      final_position += 1;
    else if(final_position < 100)
      final_position += 5;
    else if(final_position < 200)
      final_position += 10;
    else
      final_position += 20;
  }
  printf("--- + --- ---------------- --------- +\n");
  return 0;
# undef _time_limit_
}




