# ece_563_dynamic_branch_prediction
1. Type "make" to build.  (Type "make clean" first if you already compiled and want to recompile from scratch.)

2. Run trace reader:

   To run without throttling output:
   ./sim bimodal 6 gcc_trace.txt
   ./sim gshare 9 3 gcc_trace.txt
   ./sim hybrid 8 14 10 5 gcc_trace.txt

   To run with throttling (via "less"):
   ./sim bimodal 6 gcc_trace.txt | less
   ./sim gshare 9 3 gcc_trace.txt | less
   ./sim hybrid 8 14 10 5 gcc_trace.txt | less
