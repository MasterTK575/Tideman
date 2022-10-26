#include <cs50.h>
#include <stdio.h>
#include <string.h>

// Max number of candidates
#define MAX 9

// preferences[i][j] is number of voters who prefer i over j
int preferences[MAX][MAX];

// locked[i][j] means i is locked in over j
bool locked[MAX][MAX];

// Each pair has a winner, loser
typedef struct
{
    int winner;
    int loser;
}
pair;

// Array of candidates
string candidates[MAX];
pair pairs[MAX * (MAX - 1) / 2];

int pair_count;
int candidate_count;

// Function prototypes
bool vote(int rank, string name, int ranks[]);
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
void lock_pairs(void);
bool cycle_check(int start, int loser);
void print_winner(void);

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: tideman [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i] = argv[i + 1];
    }

    // Clear graph of locked in pairs
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
        }
    }

    pair_count = 0;
    int voter_count = get_int("Number of voters: ");

    // loop through all votes made
    for (int i = 0; i < voter_count; i++)
    {
        // ranks[i] is voter's ith preference
        int ranks[candidate_count];

        // loop through all ranks given
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            if (!vote(j, name, ranks))
            {
                printf("Invalid vote.\n");
                return 3;
            }
        }

        // test ranks[]
        // printf("Ranking: \n");
        // for (int k = 0; k < candidate_count; k++)
        // {
            // printf("%s or %i\n", candidates[ranks[k]], ranks[k]);
        // }

        record_preferences(ranks);

        // test preferences
        // for (int can = 0; can < candidate_count; can++)
        // {
            // printf("%s is preferred over:\n", candidates[can]);
            // for (int loser = 0; loser < candidate_count; loser++)
            // {
               // printf("%s: %i times\n", candidates[loser], preferences[can][loser]);
            // }
        // }
        printf("\n");
    }


    add_pairs();
    sort_pairs();
    lock_pairs();

    // print locked to test
    //for (int row = 0; row < candidate_count; row++)
    //{
        //for (int column = 0; column < candidate_count; column++)
        //{
            // 0 is false, 1 is true
            //printf("%i ", locked[row][column]);
        //}
        //printf("\n");
    //}

    print_winner();
    return 0;
}

// Update ranks given a new vote
bool vote(int rank, string name, int ranks[])
{
    // find the candidate
    for (int i = 0; i < candidate_count; i++)
    {
        // ignore other candidates
        if (strcmp(name, candidates[i]) != 0)
        {
            continue;
        }
        // if candidate found, commit
        else if (strcmp(name, candidates[i]) == 0)
        {
            // rank represents the rank at which the candidate was typed (i.e. Rank 1:....)
            // i represents the index of the actual candidate
            // i.e. if Alice was ranked first, then ranks[0] will become the index for Alice
            ranks[rank] = i;
            return true;
            // I don't exactly understand why updating ranks[] here in the function, also changes it in main...
            // I though we only get a copy of the variable -- UPDATE: this is the case for variables, but not for arrays;
            // if an array is changed somewhere else, changes persist
        }
    }
    return false;
}

// Update preferences given one voter's ranks
void record_preferences(int ranks[])
{
    // the actual values in ranks[i] give us the indices of the candidates
    // so ranks[0] is the index of the first place candidate

    // loop through each rank in ranks[]
    // i.e. start with number one, then go to number 2 etc.
    // i therefore represents the candidate that was preferred (i.e. was on top)
    for (int i = 0; i < candidate_count; i++)
    {
        // for each candidate go down the ranking list to see all the other candidates below
        // we start at 1 bc we always start one below i
        // or use: for (int j = i + 1; j < candidate_count; j++)
        int n = 1;
        while ((n + i) < candidate_count)
        {
            preferences[ranks[i]][ranks[n+i]]++;
            n++;
        }
    }
    return;
}


// Record pairs of candidates where one is preferred over the other
void add_pairs(void)
{
    // loop through all candidates
    for (int i = 0; i < candidate_count; i++)
    {
        // now through all the "losers" inside the array
        for (int j = 0; j < candidate_count; j++)
        {
            // skip 0s
            if (preferences[i][j] < 1)
            {
                continue;
            }
            // comparing the opposite scores with each other
            // i.e. Alice preferred over Bob vs Bob preferred over Alice
            else if (preferences[i][j] > preferences[j][i])
            {
                pairs[pair_count].winner = i;
                pairs[pair_count].loser = j;
                pair_count++;
            }
        }
    }
    return;
}

// Sort pairs in decreasing order by strength of victory
void sort_pairs(void)
{
    // using the bubble sort
    bool swapped;
    do
    {
        swapped = false;
        // look through all the pairs
        for (int i = 0; i < pair_count - 1; i ++)
        {
            // if winner score of i is less than winner score of i + 1 --> swap
            if (preferences[pairs[i].winner][pairs[i].loser] < preferences[pairs[i+1].winner][pairs[i+1].loser])
            {
                pair pos_old = pairs[i+1];
                pairs[i+1] = pairs[i];
                pairs[i] = pos_old;
                swapped = true;
            }
        }
    }
    while (swapped == true);
    return;
}

// Lock pairs into the candidate graph in order, without creating cycles
void lock_pairs(void)
{
    // go through the pairs array
    for (int i = 0; i < pair_count; i++)
    {
        // if would not create a cycle, lock in the result
        if (cycle_check(pairs[i].winner, pairs[i].loser) == false)
        {
            locked[pairs[i].winner][pairs[i].loser] = true;
        }
    }
    return;
}

bool cycle_check(int index_start, int index_loser)
{

    // if we end up back where we started, we have a cycle
    if (index_start == index_loser)
    {
        return true;
    }

    // check if the loser has a win locked in already (which might be the start to a cycle)
    // so we iterate through the locked array of the loser to find a win
    for (int i = 0; i < candidate_count; i++)
    {
        // check to see if there is an arrow coming out from the loser
        if (locked[index_loser][i] == true)
        {
            // if we found a cycle, return true
            if (cycle_check(index_start, i) == true)
            {
                return true;
            }
            // return cycle_check(index_start, i); does not work
            // reason being we don't want to return false prematurely
            // i.e. we only want to return the result of cycle_check here if it's true
            // otherwise let the algorithm run to look through all possible "path"; i.e. let the loop finish
            // since if it returns false it doesn't necessarily mean that there can't be a cycle..
            // it just means that for this particular path there wasn't a cycle; but there can be multiple paths...
            // we have to loop through all possibilities first and only if none were true can we declare it as false
            // here is an explanation
            // https://gist.github.com/nicknapoli82/6c5a1706489e70342e9a0a635ae738c9?permalink_comment_id=4177559#gistcomment-4177559
        }
    }
    // if after looking through all possibilies we don't find a cycle, there is no cycle
    return false;
}

// Print the winner of the election
void print_winner(void)
{
    bool found;
    // winner: the one that has no arrows pointing towards him
    // aka. no "true" in another candidates array
    for (int column = 0; column < candidate_count; column++)
    {
        found = true;
        for (int row = 0; row < candidate_count; row++)
        {
            // if there is a true, this means that there is an arrow pointing towards him/her
            // therefore can't be the winner
            if (locked[row][column] == true)
            {
                found = false;
                break;
            }
        }
        // if there was no true (aka. all false, therefore no arrows towards it), we found it
        if (found == true)
        {
            printf("%s\n", candidates[column]);
            return;
        }
    }
}