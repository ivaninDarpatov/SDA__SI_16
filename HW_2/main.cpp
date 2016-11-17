#include <iostream>
#include <cmath>
#include "CustomException.h"
#include "Query.h"
#include "HWStack.h"
#include "HWQueue.h"

#define CMD_CALL_WORDS 4
#define CMD_GO_WORDS 3

using namespace std;

void startElevator(int n_floors, HWQueue<Query> queries);
void pushQueriesBeforeCurrentInStack(HWQueue<Query>* queries, Query current, HWStack<Query>* active_queries,
                                     float time, int floor, string direction);
void printElevator(float time, int floor, string direction);

Query parseQuery(string query);
void parseQueries(Query* queries, string* queries_raw, int n_queries);
void sortQueries(Query* queries, int n_queries);

bool isValidCommand(string command, int n_floors);
bool isDigit(char symbol);

int main()
{
    int N, K;
    cin >> N;
    cin >> K;

    string dummy;
    getline(cin, dummy); // dummy input to refresh cin, won't work without this line

    int n_floors = N; // number of floors
    int n_queries = K; // number of queries

    if (n_queries == 0 || n_floors == 0)
    {
        cout << "floors and queries count must be more than 0" << endl;
        cout << "terminating..." << endl;
        return 0;
    }

    string queries_raw[n_queries];

    for (int i = 0; i < n_queries; i++)
    {
        getline(cin, queries_raw[i]);

        if (!isValidCommand(queries_raw[i], n_floors))
        {
            cout << "invalid command entered" << endl;
            cout << "terminating..." << endl;
            return 0;
        }
    }

    try
    {
        Query queries[n_queries];
        parseQueries(queries, queries_raw, n_queries);
        sortQueries(queries, n_queries);

        HWQueue<Query> q_queries(n_queries);

        for (int i = 0; i < n_queries; i++)
        {
            q_queries.enqueue(queries[i]);
        }
        startElevator(n_floors, q_queries);
    }
    catch (const CustomException ce)
    {
        cout << ce.what() << endl;
    }

    return 0;
}

void startElevator(int n_floors, HWQueue<Query> queries)
{
    HWStack<Query> active_queries(queries.getSize());
    float time = queries.seeFront().getTime();
    int floor = 1;
    string direction = "up";

    while (queries.getSize() > 0)
    {
        Query bottom_query = queries.dequeue();
        active_queries.push(bottom_query);
        direction = (bottom_query.getFloor() >= floor) ? "up" : "down";

        if (bottom_query.getTime() > time)
        {
            time = bottom_query.getTime();
        }

        while (floor != bottom_query.getFloor())
        {
            Query current = active_queries.peek();

            pushQueriesBeforeCurrentInStack(&queries, current, &active_queries, time, floor, direction);
            /*
                set the criteria (up/down, floor range, time range)
                get all responding to criteria
                order them by criteria
                push to stack in order
            */

            while (floor == current.getFloor() && active_queries.getSize() > 0)
            {
                printElevator(time, floor, direction); // function
                active_queries.pop();
                try
                {
                    current = active_queries.peek();
                }
                catch (const CustomException ce)
                {
                    break; // exit loop
                }
            }

            (direction == "up") ? floor++: floor--;
            time += 5;
            /*
                increment floor
                add 5 to time
            */
        }

        printElevator(time, floor, direction);
        active_queries.pop();
    }

    while (active_queries.getSize() > 0)
    {
        Query current = active_queries.pop();
        time += 5 * abs(current.getFloor() - floor);
        floor = current.getFloor();

        direction = (current.getFloor() >= floor) ? "up" : "down";
        printElevator(time, floor, direction);
    }
}

void pushQueriesBeforeCurrentInStack(HWQueue<Query>* queries, Query current, HWStack<Query>* active_queries,
                                     float time, int floor, string direction)
{
    int valid_queries = 0;

    //get the size for the array
    for (int i = 0; i < queries->getSize(); i++)
    {
        Query next = queries->dequeue();

        if (next.getTime() > time)
        {
            queries->enqueue(next);
        }
        else
        {
            if (direction == "up")
            {
                if (next.getType() == "call" && next.getDirection() == "down")
                {
                    queries->enqueue(next);
                    continue;
                }

                if (next.getFloor() >= floor && next.getFloor() <= active_queries->peek().getFloor())
                {
                    valid_queries++;
                    queries->enqueue(next);
                } else
                {
                    queries->enqueue(next);
                }
            }
            else
            {
                if (next.getType() == "call" && next.getDirection() == "up")
                {
                    queries->enqueue(next);
                    continue;
                }

                if (next.getFloor() <= floor && next.getFloor() >= active_queries->peek().getFloor())
                {
                    valid_queries++;
                    queries->enqueue(next);
                } else
                {
                    queries->enqueue(next);
                }
            }
        }
    }

    if (valid_queries == 0)
    {
        return;
    }

    Query unsorted[valid_queries];
    int ind = 0;

    //fill array to be sorted
    int queue_size = queries->getSize();
    for (int i = 0; i < queue_size; i++)
    {
        Query next = queries->dequeue();

        if (next.getTime() > time)
        {
            queries->enqueue(next);
        }
        else
        {
            if (direction == "up")
            {
                if (next.getType() == "call" && next.getDirection() == "down")
                {
                    queries->enqueue(next);
                    continue;
                }

                if (next.getFloor() >= floor && next.getFloor() <= active_queries->peek().getFloor())
                {
                    unsorted[ind++] = next;
                } else
                {
                    queries->enqueue(next);
                }
            }
            else
            {
                if (next.getType() == "call" && next.getDirection() == "up")
                {
                    queries->enqueue(next);
                    continue;
                }

                if (next.getFloor() <= floor && next.getFloor() >= active_queries->peek().getFloor())
                {
                    unsorted[ind++] = next;
                } else
                {
                    queries->enqueue(next);
                }
            }


        }
    }

    //bubble sort
    for (int i = 0; i < valid_queries - 1; i++)
    {
        for (int j = i + 1; j < valid_queries; j++)
        {
            if ((direction == "up") ?
                (unsorted[i].getFloor() < unsorted[j].getFloor()) :
                (unsorted[i].getFloor() > unsorted[j].getFloor()))
            {
                Query swapper = unsorted[i];
                unsorted[i] = unsorted[j];
                unsorted[j] = swapper;
            }
        }
    }

    //align sorted array in stack
    for (int i = 0; i < valid_queries; i++)
    {
        if (active_queries->getSize() > 0)
        {
            if (unsorted[i].getFloor() == active_queries->peek().getFloor())
            {
                //skip equal queries
                continue;
            }
        }

        active_queries->push(unsorted[i]);
    }
}

void printElevator(float time, int floor, string direction)
{
    string dir = (direction == "up") ? "down" : "up";
    cout << time << " " << floor << " " << dir << endl;
}

Query parseQuery(string query)
{
    string type;
    string direction = "unknown";
    int floor = 0;
    float time = 0;
    int floor_start;

    if (query.substr(0, 2) == "go")
    {
        type = "go";
        floor_start = 3;
    }
    else
    {
        if (query.substr(0, 4) == "call")
        {
            type = "call";
            if (query.substr(5, 2) == "up")
            {
                direction = "up";
                floor_start = 8;
            }
            else
            {
                if (query.substr(5, 4) == "down")
                {
                    direction = "down";
                    floor_start = 10;
                }
                else
                {
                    throw CustomException("exception: invalid direction in call query");
                }
            }
        }
        else
        {
            throw CustomException("exception: invalid query type");
        }
    }

    int temp_index;
    for (temp_index = floor_start; query[temp_index] != ' '; temp_index++)
    {
        floor *= 10;
        floor += query[temp_index] - '0';
    }

    bool hasDot = false;
    float afterDot = 1;
    for (unsigned int i = temp_index + 1; i < query.size(); i++)
    {
        if (query[i] == '.')
        {
            hasDot = true;
            continue;
        }

        if (!hasDot)
        {
            time *= 10;
            time += query[i] - '0';
        }
        else
        {
            afterDot /= 10;
            time += afterDot * (query[i] - '0');
        }
    }

    Query result(type, floor, time);
    if (direction != "unknown")
    {
        result.setDirection(direction);
    }

    return result;
}

void parseQueries(Query* queries, string* queries_raw, int n_queries)
{
    for (int i = 0; i < n_queries; i++)
    {
        queries[i] = parseQuery(queries_raw[i]);
    }
}

void sortQueries(Query* queries, int n_queries)
{
    //bubble sort
    for (int i = 0; i < n_queries - 1; i++)
    {
        for (int j = i + 1; j < n_queries; j++)
        {
            if (queries[i] > queries[j])
            {
                Query swapper = queries[i];
                queries[i] = queries[j];
                queries[j] = swapper;
            }
        }
    }
}

bool isValidCommand(string command, int n_floors)
{
    /*
        call command must have 4 words (call DIR FLOOR TIME)
        go command must have 3 words (go FLOOR TIME)
        in call:
            word 2 must be "up" or "down"
            word 3 must be of type int
            word 4 must be of type float
        in go:
            word 2 must be of type int
            word 3 must be of type float
        FLOOR must be in the floors range (1, n_floors)
    */

    if (!isDigit(command[command.size() - 1]))
    {
        cout << "command must end with a float" << endl;
        return false;
    }

    int n_words = 1;
    for (unsigned int i = 0; i < command.size(); i++)
    {
        if (command[i] == ' ')
        {
            n_words++;
        }
    }

    if (n_words > CMD_CALL_WORDS || n_words < CMD_GO_WORDS)
    {
        return false;
    }

    if (n_words == CMD_CALL_WORDS)
    {
        if (command.substr(0, 4) != "call")
        {
            return false;
        }

        if (command.substr(5, 2) != "up" && command.substr(5, 4) != "down")
        {
            return false;
        }

        int floor_start = (command.substr(5, 2) == "up") ? 8 : 10;

        if (command[floor_start - 1] == ' ')
        {
            int temp_index;
            int floor_check = 0;
            for (temp_index = floor_start; command[temp_index] != ' '; temp_index++)
            {
                if (!isDigit(command[temp_index]))
                {
                    return false;
                }

                floor_check *= 10;
                floor_check += command[temp_index] - '0';
            }

            if (floor_check < 1 || floor_check > n_floors)
            {
                return false;
            }

            bool hasDot = false;
            for (unsigned int i = temp_index + 1; i < command.size(); i++)
            {
                if (hasDot)
                {
                    if (command[i] == '.')
                    {
                        return false;
                    }
                }

                if (command[i] == '.')
                {
                    hasDot = true;
                    continue;
                }

                if (!isDigit(command[i]))
                {
                    return false;
                }
            }

        }
    }

    if (n_words == CMD_GO_WORDS)
    {
        if (command.substr(0, 2) != "go")
        {
            return false;
        }

        int temp_index;
        for (temp_index = 3; command[temp_index] != ' '; temp_index++)
        {
            if (!isDigit(command[temp_index]))
            {
                return false;
            }
        }

        bool hasDot = false;
        for (unsigned int i = temp_index + 1; i < command.size(); i++)
        {
            if (hasDot)
            {
                if (command[i] == '.')
                {
                    return false;
                }
            }

            if (command[i] == '.')
            {
                hasDot = true;
                continue;
            }

            if (!isDigit(command[i]))
            {
                return false;
            }
        }
    }

    return true;
}

bool isDigit(char symbol)
{
    return (symbol >= '0' && symbol <= '9');
}
