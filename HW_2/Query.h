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

#ifndef QUERY_H
#define QUERY_H
using namespace std;

class Query {
private:
    string type;
    string direction;
    int floor;
    int time;
public:
    Query();
    Query(string type, int floor, int time);
    ~Query();

    void setDirection(string direction);

    string getType();
    string getDirection();
    int getFloor();
    int getTime();
private:
    void setType(string type);
    void setFloor(int floor);
    void setTime(int time);
};

Query::Query() {}

Query::Query(string type, int floor, int time)
{
    this->setType(type);
    this->setFloor(floor);
    this->setTime(time);

    this->direction = "unknown";
}

Query::~Query() {}

//setters
void Query::setType(string type)
{
    if (type == "go" || type == "call")
    {
        this->type = type;
    } else
    {
        throw CustomException("exception: invalid type");
    }
}

void Query::setDirection(string direction)
{
    if (direction == "up" || direction == "down")
    {
        this->direction = direction;
    } else
    {
        throw CustomException("exception: invalid direction");
    }
}

void Query::setFloor(int floor)
{
    if (floor >= 1)
    {
        this->floor = floor;
    } else
    {
        throw CustomException("exception: invalid floor");
    }
}

void Query::setTime(int time)
{
    if (time >= 0)
    {
        this->time = time;
    } else
    {
        throw CustomException("exception: invalid time");
    }
}
//

//getters
string Query::getType()
{
    return this->type;
}

string Query::getDirection()
{
    return this->direction;
}

int Query::getFloor()
{
    return this->floor;
}

int Query::getTime()
{
    return this->time;
}
//
#endif // QUERY_H
