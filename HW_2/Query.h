#ifndef QUERY_H
#define QUERY_H
using namespace std;

class Query {
private:
    string type;
    string direction;
    int floor;
    float time;
public:
    Query();
    Query(string type, int floor, float time);

    bool operator>(Query & other);

    void setDirection(string direction);

    string getType();
    string getDirection();
    int getFloor();
    float getTime();

    friend ostream& operator<< (ostream& stream, const Query& query);
private:
    void setType(string type);
    void setFloor(int floor);
    void setTime(float time);
};

Query::Query() {}

Query::Query(string type, int floor, float time)
{
    this->setType(type);
    this->setFloor(floor);
    this->setTime(time);

    this->direction = "unknown";
}

bool Query::operator>(Query & other)
{
    return this->time > other.time;
}

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

void Query::setTime(float time)
{
    if (time >= 1)
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

float Query::getTime()
{
    return this->time;
}
//


ostream& operator<< (ostream& stream, Query& query)
{
    stream << "type: " << query.getType() << endl <<
                "direction: " << query.getDirection() << endl <<
                "floor: " << query.getFloor() << endl <<
                "time: " << query.getTime() << endl;
    return stream;

}
#endif // QUERY_H
