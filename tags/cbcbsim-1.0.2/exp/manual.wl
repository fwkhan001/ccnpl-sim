# manually tailored workload
time_unit 0.0100000 ;
sim_length 20000 ;
node 0 455 282 ,
node 1 386 324 ,
node 2 12 202 ,
node 3 357 651 ,
node 4 337 22 ;
link 0 1 8 ,
link 0 2 45 ,
link 2 3 56 ,
link 3 1 32 ,
link 3 4 62 ,
link 4 1 30 ,
link 4 2 37 ;
set_predicate 0 0 0 a < 10 ;
set_predicate 1 0 0 b < 10 ;
set_predicate 2 0 0 c < 10 ;
set_predicate 3 0 0 d < 10 ;
set_predicate 4 0 0 e < 10 ;
# send_message 3 2000 a = 10 ;
# send_message 3 2100 a = 9 ;
# send_message 2 4000 a = 9 ;
# set_predicate 3 6000 0 a > 8 ;
# set_predicate 0 6000 0 ;
# send_message 2 8000 a = 9 ;
