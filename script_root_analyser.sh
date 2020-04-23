# Script to run decorrelate_sigmas.py file in a loop
#!/usr/bin/env zsh

# data
# for i in 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19
# do
#  for j in 0 1 2 3 4 5 6 7 8 9 10 11 12
#  do
#   root -l -b -q "RootAnalyzer.cc(\"Cut_root_files/data/Cut_2e_run166.root\",$i,$j,0.7,0.7)"
#  done
# done

# simus, only for neighbours of OM 9:6
# for i in 8 9 10
# do
#  for j in 5 6 7
#  do
#   root -l -b -q "RootAnalyzer.cc(\"Cut_root_files/simus/10e8_simus_it_centre_Cut_2e.root\",$i,$j,0.7,0.7)"
#  done
# done

# simus, only for neighbours of OM 9:6
for i in 9
do
 for j in 5
 do
  root -l -b -q "RootAnalyzer.cc(\"Cut_root_files/simus/10e9_simus_it_centre_Cut_2e.root\",\"/home/girardcarillo/Workdir/Analyses/reflectometry_tests/correctedTimes/corrected_times_MW_fr.txt\",$i,$j,0.7,0.7,0)"
 done
done

