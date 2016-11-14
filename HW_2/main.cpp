#include <iostream>
#include "CustomException.h"
#include "Query.h"
#include "HWStack.h"

#define CMD_CALL_WORDS 4
#define CMD_GO_WORDS 3

using namespace std;

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

    Query queries[n_queries];
    parseQueries(queries, queries_raw, n_queries);
    sortQueries(queries, n_queries);

    for (int i = 0; i < n_queries; i++)
    {
        cout << endl << "type:" << queries[i].getType() << endl;
        cout << "direction:" << queries[i].getDirection() << endl;
        cout << "floor:" << queries[i].getFloor() << endl;
        cout << "time:" << queries[i].getTime() << endl;
    }

    return 0;
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
            } else
            {
                if (query.substr(5, 4) == "down")
                {
                    direction = "down";
                    floor_start = 10;
                } else
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

    int temp_index = floor_start;
    for (temp_index; query[temp_index] != ' '; temp_index++)
    {
        floor *= 10;
        floor += query[temp_index] - '0';
    }

    bool hasDot = false;
    float afterDot = 1;
    for (int i = temp_index + 1; i < query.size(); i++)
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
        } else
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
