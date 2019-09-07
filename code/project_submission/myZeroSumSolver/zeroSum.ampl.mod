param Rows;
param Columns;
param P2_Rewards{1..Rows, 1..Columns};
param P1_Rewards{a1 in 1..Rows, a2 in 1..Columns} 
  = -P2_Rewards[a1,a2];

var P2_Strategy{1..Columns} >= 0.0, <= 1.0;
var P1_reward >= -1, <= 0;

minimize P1_reward_objective: P1_reward;

subject to P1_counters{a1 in 1..Rows}:
  sum{a2 in 1..Columns}
    P1_Rewards[a1,a2]*P2_Strategy[a2]
  <= P1_reward;

subject to P2_Feasible_Strategy:
  sum{a2 in 1..Columns}
    P2_Strategy[a2]
  == 1.0;



var P1_Strategy{1..Rows} >= 0.0, <= 1.0;
var P2_reward >= 0, <= 1;

minimize P2_reward_objective: P2_reward;

subject to P2_counters{a2 in 1..Columns}:
  sum{a1 in 1..Rows}
    P2_Rewards[a1,a2]*P1_Strategy[a1]
  <= P2_reward;

subject to P1_Feasible_Strategy:
  sum{a1 in 1..Rows}
    P1_Strategy[a1]
  == 1.0;

var P1_Expected_Utility
  = sum{a1 in 1..Rows}
      sum{a2 in 1..Columns}
        P1_Strategy[a1]*P2_Strategy[a2]*P1_Rewards[a1,a2];

var P2_Expected_Utility = -P1_Expected_Utility;
