param Rows;
param Columns;
param P2_Rewards{1..Rows, 1..Columns};
param P1_Rewards{a1 in 1..Rows, a2 in 1..Columns}
  = -P2_Rewards[a1,a2];

# define the objective
var P2_Strategy{1..Columns} >= 0.0, <= 1.0;

# player 1's reward for a strategy
# assuming player 2 counters it
var P1_reward >= -1, <= 0;


minimize P1_reward_objective: P1_reward;

subject to Countered{a1 in 1..Rows}:
  sum{a2 in 1..Columns}
    P1_Rewards[a1, a2]*P2_Strategy[a2]
  <= P1_reward;

subject to Feasible_Strategy:	
  sum{a2 in 1..Columns}
    P2_Strategy[a2]
  == 1.0;

