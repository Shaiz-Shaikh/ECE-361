# AI usage for HW6

**Tool:** Claude (chat), used step by step while I did the homework.

**What I used it for:**
- To explain what the homework was asking, and the bit tricks behind
  panel_pressed, panel_released and the read-modify-write for the LEDs.
- Claude helped in writing the C code (sensor, panel, scenario, main, the tests, the
  Makefile and regress.sh) and explained each part. I typed it into the
  terminal in small pieces, compiled it, ran it, and read the output after
  every step.
- The README text was drafted with Claude's help.

**What I did myself:**
- I ran the program on two scenarios and checked every trace line by hand
  against the rules (a held switch toggles once, a release does nothing, two
  switches pressed together toggle two LEDs) before I saved them as the
  expected traces.
- I checked that the regression test can really fail: I added a wrong line to
  an expected file, saw regress.sh print FAIL and exit with 1, then put the
  file back.
- I checked what happens before the first line of a scenario (the sensor keeps
  its starting value of 23.5 C and 75 %RH) because the README says so.

**One thing the model got wrong:** Claude gave me two terminal commands that
do not work in zsh, the shell on my Mac. One used `${PIPESTATUS[0]}`, which
printed nothing, and one used `rm tests/*.o` before any .o file existed, which
made zsh stop with "no matches found" and skip the rest of the command. The C
code and the tests were not affected, but I had to run the check again with a
simpler command.
