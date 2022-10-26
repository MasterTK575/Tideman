# Tideman
A program that runs a Tideman election, as per CS50 week 3.
https://cs50.harvard.edu/x/2022/psets/3/tideman/

In this project we had to write a program that runs a Tideman election, using ranked choice voting. The Tideman voting method (also known as "ranked pairs") is a voting method that's guaranteed to produce the Condorcet winner of the election (if one exists). Condorcet winner being the person who would have won any head-to-head matchup against another candidate (i.e. just comparing candidates in pairs - hence the name). This is usually done by constructing graphs, where an arrow pointed from candidate A to candidate B means that candidate A would win in a head-to-head competition. If there is a source of the graph (i.e. a candidate with no arrows pointing towards them), we have a winner.
