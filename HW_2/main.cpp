/**
 *
 * Solution to homework task
 * Data Structures Course
 * Faculty of Mathematics and Informatics of Sofia University
 * Winter semester 2016/2017
 *
 * @author Ivan Darpatov
 * @idnumber 61810
 * @task 2
 * @compiler GCC
 *
 */

#include <iostream>
#include <fstream>
#include <cmath>
#include "CustomException.h"
#include "Query.h"
#include "HWStack.h"
#include "HWQueue.h"

using namespace std;

string* getLinesFromFile(ifstream& file, int lines_count);

void startElevator(int n_floors, HWQueue<Query> queries);
void pushQueriesBeforeCurrentInStack(HWQueue<Query>* queries, HWStack<Query>* active_queries,
                                     int time, int floor, string direction);
void printElevator(int time, int floor, string direction);

HWQueue<Query> getQueriesInQueue(string* queries_raw, int n_queries);
Query parseQuery(string query);
void parseQueries(Query* queries, string* queries_raw, int n_queries);

bool isDigit(char symbol);

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        cout << "invalid arguments count" << endl;
        return 0;
    }

    char* file_path = argv[1];
    ifstream cmd_file(file_path);

    if (!cmd_file.is_open())
    {
        cout << "failed to open commands file" << endl;
        return 0;
    }

    // get first line w/ floors and queries count
    string first_line;
    getline(cmd_file, first_line);

    int N = 0, K = 0;
    bool space_reached = false;

    // parse first line into 2 numbers
    unsigned int line_length = (first_line[first_line.size() - 1] == 13) ?
                                first_line.size() - 1 : first_line.size(); // evade Carriage Return symbol
    for (unsigned int i = 0; i < line_length; i++)
    {
        if (first_line[i] == ' ')
        {
            space_reached = true;
            continue;
        }

        if (!space_reached)
        {
            N *= 10;
            N += first_line[i] - '0';
        } else
        {
            K *= 10;
            K += first_line[i] - '0';
        }
    }

    int n_floors = N; // number of floors
    int n_queries = K; // number of queries

    if (n_queries == 0 || n_floors == 0)
    {
        cout << "floors and queries count must be more than 0" << endl;
        cout << "terminating..." << endl;
        return 0;
    }

    // get the queries
    string* queries_raw = getLinesFromFile(cmd_file, n_queries);
    if (cmd_file.is_open())
    {
        cmd_file.close();
    }

    try
    {
        // align queries into queue (considering they come ordered by time from file)
        HWQueue<Query> q_queries = getQueriesInQueue(queries_raw, n_queries);

        startElevator(n_floors, q_queries);
    }
    catch (const CustomException ce)
    {
        cout << ce.what() << endl;
    }
    return 0;
}

string* getLinesFromFile(ifstream& file, int lines_count)
{
    string* lines_container = new string[lines_count];
    string line;
    int i = 0;
    if (file.is_open())
    {
        while ( getline (file,line) )
        {
            lines_container[i++] = line;
        }
    }
    else
    {
        throw CustomException("exception: file is closed");
    }

    return lines_container;
}

void startElevator(int n_floors, HWQueue<Query> queries)
{
    HWStack<Query> active_queries(queries.getSize()); // queries being executed at the same time
    int time = 0;
    int floor = 1; // start floor
    string direction;

    while (queries.getSize() > 0)
    {
        Query bottom_query = queries.dequeue(); // query being executed now
        active_queries.push(bottom_query);
        direction = (bottom_query.getFloor() >= floor) ? "up" : "down";

        if (bottom_query.getTime() > time) // simulate elevator waiting (bottom_query.getTime() - time) seconds before being called
        {
            time = bottom_query.getTime();
        }

        while (floor != bottom_query.getFloor())
        {
            Query current = active_queries.peek(); // first query to be executed from the stack

            pushQueriesBeforeCurrentInStack(&queries, &active_queries, time, floor, direction); // insert queries to be executed between now and the completion of 'current'

            while (floor == current.getFloor() && active_queries.getSize() > 0) // execute all queries (if any) for current floor
            {
                printElevator(time, floor, direction);
                active_queries.pop();
                try
                {
                    current = active_queries.peek(); // if not empty
                }
                catch (const CustomException ce)
                {
                    break; // exit loop
                }
            }

            (direction == "up") ? floor++: floor--;
            time += 5;
        }

        printElevator(time, floor, direction); // print elevator when bottom query has reached its destination
        active_queries.pop();
    }
}

void pushQueriesBeforeCurrentInStack(HWQueue<Query>* queries, HWStack<Query>* active_queries,
                                     int time, int floor, string direction)
{
    int added_queries = 0;
    int queries_count = queries->getSize();
    Query bottom = active_queries->peek();

    //get new queries
    for (int i = 0; i < queries_count; i++)
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

                if (next.getFloor() >= floor && next.getFloor() <= bottom.getFloor())
                {
                    added_queries++;
                    active_queries->push(next);
                }
                else
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
                    added_queries++;
                    active_queries->push(next);
                }
                else
                {
                    queries->enqueue(next);
                }
            }
        }
    }

    if (added_queries == 0)
    {
        return;
    }

    Query to_be_sorted[added_queries];

    //fill array to be sorted
    for (int i = 0; i < added_queries; i++)
    {
        to_be_sorted[i] = active_queries->pop();
    }

    //bubble sort
    for (int i = 0; i < added_queries - 1; i++)
    {
        for (int j = i + 1; j < added_queries; j++)
        {
            if ((direction == "up") ?
                    (to_be_sorted[i].getFloor() < to_be_sorted[j].getFloor()) :
                    (to_be_sorted[i].getFloor() > to_be_sorted[j].getFloor()))
            {
                Query swapper = to_be_sorted[i];
                to_be_sorted[i] = to_be_sorted[j];
                to_be_sorted[j] = swapper;
            }
        }
    }

    //align sorted array in stack
    for (int i = 0; i < added_queries; i++)
    {
        if (active_queries->getSize() > 0)
        {
            if (to_be_sorted[i].getFloor() == active_queries->peek().getFloor())
            {
                //skip equal queries
                continue;
            }
        }

        active_queries->push(to_be_sorted[i]);
    }
}

void printElevator(int time, int floor, string direction)
{
    string dir = (direction == "up") ? "down" : "up";
    cout << time << " " << floor << " " << dir << endl;
}

HWQueue<Query> getQueriesInQueue(string* queries_raw, int n_queries)
{
    Query queries[n_queries];
    parseQueries(queries, queries_raw, n_queries);
    HWQueue<Query> result(n_queries);

    for (int i = 0; i < n_queries; i++)
    {
        result.enqueue(queries[i]);
    }

    return result;
}

Query parseQuery(string query)
{
    string type;
    string direction = "unknown";
    int floor = 0;
    int time = 0;
    int floor_start;
    unsigned int query_length = (query[query.size() - 1] == 13) ?
                                    query.size() - 1 : query.size();

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

    int floor_end;
    for (floor_end = floor_start; query[floor_end] != ' '; floor_end++)
    {
        if (!isDigit(query[floor_end]))
        {
            throw CustomException("exception: invalid floor");
        }
        floor *= 10;
        floor += query[floor_end] - '0';
    }

    for (unsigned int i = floor_end + 1; i < query_length; i++)
    {
        if (!isDigit(query[i]))
        {
            throw CustomException("exception: invalid time");
        }
        time *= 10;
        time += query[i] - '0';
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

bool isDigit(char symbol)
{
    return (symbol >= '0' && symbol <= '9');
}
