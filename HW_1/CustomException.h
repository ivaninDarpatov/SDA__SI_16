#ifndef CUSTOM_EXCEPTION_H
#define CUSTOM_EXCEPTION_H
using namespace std;

class CustomException : exception {
private:
    char* message;
public:
    CustomException(string message);
    ~CustomException() throw();
    virtual const char* what() const throw();
private:
    void setMessage(string message);
};
//meta
CustomException::CustomException(string message)
{
    this->setMessage(message);
}

CustomException::~CustomException() throw()
{
    delete [] this->message;
}
//

//override
const char* CustomException::what() const throw()
{
    return this->message;
}
//

//setters
void CustomException::setMessage(string message)
{
    if (message.size() > 0)
    {
        this->message = new char[message.size()];
        for (unsigned int i = 0; i < message.size(); i++)
        {
            this->message[i] = message[i];
        }
    } else
    {
        this->setMessage("exception: something went wrong");
    }
}
//
#endif
