#include <iostream>
#include <fstream>
#include <string.h>
#include <iomanip>
#include "CustomException.h"
#include "HWStack.h"
using namespace std;

string* getLinesFromFile(ifstream& file, int* lines_count);

string verifyExpression(string expression, char* ops);
string errorAtIndex(int index);
void printError(string base, int index);

HWStack<string> parseNotInfix(string expression);
HWStack<string> parseInfix(string expression);

string prefixToInfix(HWStack<string> prefixExpression, char* ops);
string infixToPostfix(HWStack<string> infixExpression, char* op_names, char* ops);
float calculatePostfix(string postfixExpression, char* op_names, char* ops);

bool isValidChar(char symbol, char* ops);
bool isDigit(char symbol);
bool isOperator(char symbol);
bool isOperatorName(char symbol, char* ops);
bool isOpener(char symbol);
bool isCloser(char symbol);
bool hasHigherPrecedence(char first, char second, char* op_names, char* ops);

int main (int argc, char *argv[])
{
    if (argc != 3)
    {
        cout << "invalid arguments count" << endl;
        return 0;
    }

    char* exp_path = argv[1];
    char* ops_path = argv[2];

    try
    {
        //get expression
        ifstream exp_file(exp_path);
        if (!exp_file.is_open())
        {
            cout << "failed to open expression file" << endl;
            return 0;
        }
        string expression;
        getline(exp_file, expression);
        exp_file.close();

        //get all operations as strings
        ifstream ops_file(ops_path);
        if (!ops_file.is_open())
        {
            cout << "failed to open operators file" << endl;
            return 0;
        }
        int* ops_count = new int;
        string* operations_raw = getLinesFromFile(ops_file, ops_count);
        ops_file.close();

        //store operations
        char op_names[*ops_count];
        char operators[*ops_count];
        for (int i = 0; i < *ops_count; i++)
        {
            op_names[i] = operations_raw[i][0];
            operators[i] = operations_raw[i][2];
        }

        //verify expression (throws exception saying why verifying failed)
        verifyExpression(expression, op_names);

        //parse prefix expression
        HWStack<string> prefixInStack = parseNotInfix(expression);

        //convert to infix
        string infixVer = prefixToInfix(prefixInStack, op_names);

        //parse infix expression
        HWStack<string> infixInStack = parseInfix(infixVer);

        //convert to postfix
        string postfix = infixToPostfix(infixInStack, op_names, operators);

        //calculate postfix
        float result = calculatePostfix(postfix, op_names, operators);

        //show result
        cout << postfix << endl;
        cout << setprecision(5) << result << endl;
    }
    catch (const CustomException ce)
    {
        cout << ce.what() << endl;
    }

    return 0;
}

string* getLinesFromFile(ifstream& file, int* lines_count)
{
    HWStack<string> lines_container(40);
    string line;
    if (file.is_open())
    {
        while ( getline (file,line) )
        {
            lines_container.push(line);
        }
    }
    else
    {
        throw CustomException("exception: file is closed");
    }

    *lines_count = lines_container.getSize();
    string* result = new string[*lines_count];
    int current_index = 0;

    while (lines_container.getSize())
    {
        result[current_index++] = lines_container.pop();
    }

    return result;
}

string verifyExpression(string expression, char* ops)
{
    /**
     expression must start with an operator
     expression must end with a number
     allowed characters are: '+', '-', '.', ' ', [0-9], [operator names from operators file]
     operators must always be followed by space
     '+' and '-' are always followed by a number
     '.' is always surrounded by numbers (left & right)
     '.' can only be used once in a digit
     ' ' cannot be followed by another ' '
     |operators| = |operands| - 1
    **/
    if (!isDigit(expression[expression.size() - 1]))
    {
        throw CustomException("prefix expression can only end with a number");
    }

    for (int i = 0; ops[i] != '\0'; i++)
    {
        if (ops[i] == expression[0])
        {
            break;
        }

        if (ops[i + 1] == '\0')
        {
            throw CustomException("prefix expression can only start with an operator");
        }
    }

    int operators = 0;
    int units = 1;

    for (unsigned int i = 0; i < expression.size() - 1; i++)
    {
        if (!isValidChar(expression[i], ops))
        {
            printError(expression, i);
            throw CustomException("invalid characters used");
        }

        if (isOperatorName(expression[i], ops))
        {
            operators++;

            if (expression[i + 1] != ' ')
            {
                printError(expression, i);
                throw CustomException("operators can only be followed by space");
            }
        }

        if (isDigit(expression[i]))
        {
            if (expression[i - 1] != '-' &&
                    expression[i - 1] != '+' &&
                    expression[i - 1] != ' ' &&
                    expression[i - 1] != '.' &&
                    !isDigit(expression[i - 1]))
            {
                printError(expression, i);
                throw CustomException("digits can only be preceded by '+', '-', '.', another digit or a space");
            }

            if (i != expression.size() - 1)
            {
                if (!isDigit(expression[i + 1]) &&
                    expression[i + 1] != '.' &&
                    expression[i + 1] != ' ')
                {
                    printError(expression, i);
                    throw CustomException("digits can only be followed by '.', another digit or a space");
                }
            }
        }

        if (expression[i] == ' ')
        {
            units++;

            if (!isDigit(expression[i - 1]) &&
                    !isOperatorName(expression[i - 1], ops))
            {
                printError(expression, i);
                throw CustomException("spaces can only be preceded by operator or number");
            }

            if (!isDigit(expression[i + 1]) &&
                    !isOperatorName(expression[i + 1], ops) &&
                    expression[i + 1] != '+' &&
                    expression[i + 1] != '-')
            {
                printError(expression, i);
                throw CustomException("spaces can only be followed by operator, number, '+' or '-'");
            }
        }

        if (expression[i] == '+' || expression[i] == '-')
        {
            if (!isDigit(expression[i + 1]))
            {
                printError(expression, i);
                throw CustomException("'+' and '-' can only be followed by a number");
            }
        }

        if (expression[i] == '.')
        {
            if (!isDigit(expression[i - 1]) &&
                !isDigit(expression[i + 1]))
            {
                printError(expression, i);
                throw CustomException("'.' can only be followed and preceded by a digit");
            }

            unsigned int j = i;
            do
            {
                if (j >= expression.size())
                {
                    break;
                }

                if (expression[++j] == '.')
                {
                    printError(expression, j);
                    throw CustomException("'.' can only be used once in a number");
                }
            } while (expression[j] != ' ');

            j = i;
            do
            {
                if (j <= 0)
                {
                    break;
                }

                if (expression[--j] == '.')
                {
                    printError(expression, j);
                    throw CustomException("'.' can only be used once in a number");
                }
            } while (expression[j] != ' ');
        }
    }

    if (operators != units / 2 || units % 2 == 0)
    {
        throw CustomException("invalid ratio operators-operands");
    }

    return "expression is valid";
}

string errorAtIndex(int index)
{
    string toReturn;
    for (int i = 0; i < index; i++)
    {
        toReturn.push_back(' ');
    }

    toReturn.push_back('^');

    return toReturn;
}

void printError(string base, int index)
{
    cout << endl << base << endl << errorAtIndex(index) << endl;
}

HWStack<string> parseNotInfix(string expression)
{
    int elementsCount = 1;
    for (unsigned int i = 0; i < expression.size(); i++)
    {
        if (expression[i] == ' ')
        {
            elementsCount++;
        }
    }

    HWStack<string> result(elementsCount);

    for (unsigned int i = 0; i < expression.size(); i++)
    {
        string currentUnit;
        do
        {
            currentUnit.push_back(expression[i++]);
            if (i == expression.size())
            {
                break;
            }
        }
        while (expression[i] != ' ');

        result.push(currentUnit);
    }

    return result;
}

HWStack<string> parseInfix(string expression)
{
    HWStack<string> reversed(expression.size());

    for (unsigned int i = 0; i < expression.size(); i++)
    {
        string unit;
        if (expression[i] != '+' &&
                expression[i] != '-' &&
                !isDigit(expression[i]))
        {
            unit.push_back(expression[i]);
        }
        else
        {
            do
            {
                unit.push_back(expression[i++]);
            }
            while (isDigit(expression[i]) || expression[i] == '.');

            i--;
        }

        reversed.push(unit);
    }

    HWStack<string> result(reversed.getSize());

    while (reversed.getSize() > 0)
    {
        result.push(reversed.pop());
    }

    return result;
}

string prefixToInfix(HWStack<string> prefixExpression, char* ops)
{
    string result;
    HWStack<string> resultExpression(20);

    try
    {
        while (prefixExpression.getSize() > 0)
        {
            string unit = prefixExpression.pop();

            if (unit.size() == 1)
            {
                if (isOperatorName(unit[0], ops))
                {
                    string temp;
                    temp.push_back('(');
                    temp += resultExpression.pop();
                    temp += unit;
                    temp += resultExpression.pop();
                    temp.push_back(')');
                    resultExpression.push(temp);
                    continue;
                }
            }

            resultExpression.push(unit);
        }

        result = resultExpression.pop();
    }
    catch (const CustomException ce)
    {
        cout << ce.what() << endl;
        result = "failed to convert";
    }

    return result;
}

string infixToPostfix(HWStack<string> infixExpression, char* op_names, char* ops)
{
    string result;
    HWStack<char> operators(20);

    try
    {
        while (infixExpression.getSize() > 0)
        {
            string sym = infixExpression.pop();

            if (isDigit(sym[0]) ||
                    sym[0] == '+' ||
                    sym[0] == '-')
            {
                result += sym;
                result.push_back(' ');
                continue;
            }

            if (isOpener(sym[0]))
            {
                operators.push(sym[0]);
                continue;
            }

            if (isCloser(sym[0]))
            {
                while (operators.peek() != '(')
                {
                    char op = operators.pop();
                    result.push_back(op);
                    result.push_back(' ');
                }

                operators.pop();
                continue;
            }

            if (isOperatorName(sym[0], op_names))
            {
                while (operators.getSize() > 0)
                {
                    if (isOpener(operators.peek()) || isCloser(operators.peek())) break;

                    if (!hasHigherPrecedence(sym[0], operators.peek(), op_names, ops))
                    {
                        result.push_back(operators.pop());
                        result.push_back(' ');
                    }
                    else
                    {
                        break;
                    }
                }

                operators.push(sym[0]);
            }
        }

        while (operators.getSize() > 0)
        {
            result.push_back(operators.pop());
            result.push_back(' ');
        }
    }
    catch (const CustomException ce)
    {
        cout << ce.what() << endl;
        result = "failed to convert";
    }

    return result;
}

float calculatePostfix(string postfixExpression, char* op_names, char* ops)
{
    HWStack<string> reversed = parseNotInfix(postfixExpression);
    HWStack<string> postfixInString(reversed.getSize());

    while (reversed.getSize() > 0)
    {
        postfixInString.push(reversed.pop());
    }

    HWStack<float> resultStack(postfixInString.getSize());

    while (postfixInString.getSize() > 0)
    {
        string unit = postfixInString.pop();

        if (!isOperatorName(unit[0], op_names))
        {
            unsigned int startIndex = 0;
            float value = 0;
            bool hasDot = false;
            float afterDot = 1;

            if (unit[0] == '-' || unit[0] == '+')
            {
                startIndex = 1;
            }

            for (unsigned int i = startIndex; i < unit.size(); i++)
            {
                if (unit[i] == '.')
                {
                    hasDot = true;
                    continue;
                }

                if (!hasDot)
                {
                    value *= 10;
                    value += unit[i] - '0';
                } else
                {
                    afterDot /= 10;
                    value += afterDot * (unit[i] - '0');
                }
            }

            if (unit[0] == '-')
            {
                value *= -1;
            }

            resultStack.push(value);
        } else
        {
            float left = resultStack.pop();
            float right = resultStack.pop();
            float result;
            char operation;

            for (int j = 0; op_names[j] != '\0' && ops[j] != '\0'; j++)
            {
                if (unit[0] == op_names[j])
                {
                    operation = ops[j];
                    break;
                }
            }

            switch (operation)
            {
            case '+':
                result = right + left;
                break;
            case '-':
                result = right - left;
                break;
            case '*':
                result = right * left;
                break;
            case '/':
                result = right / left;
                break;
            default:
                result = 1;
            }

            resultStack.push(result);
        }
    }

    return resultStack.pop();
}

bool isValidChar(char symbol, char* ops)
{
    for (int i = 0; ops[i] != '\0'; i++)
    {
        if (symbol == ops[i])
        {
            return true;
        }
    }

    return (symbol >= '0' && symbol <= '9') || (symbol == '-') ||
            (symbol == '+') || (symbol == ' ') || (symbol == '.');
}

bool isDigit(char symbol)
{
    return (symbol >= '0' && symbol <= '9');
}

bool isOperator(char symbol)
{
    return (symbol == '+' || symbol == '-' ||
            symbol == '*' || symbol == '/');
}

bool isOperatorName(char symbol, char* ops)
{
    for (int i = 0; ops[i] != '\0'; i++)
    {
        if (symbol == ops[i])
        {
            return true;
        }
    }

    return false;
}

bool isOpener(char symbol)
{
    return symbol == '(';
}

bool isCloser(char symbol)
{
    return symbol == ')';
}

bool hasHigherPrecedence(char first, char second, char* op_names, char* ops)
{
    for (int i = 0; op_names[i] != '\0' && ops[i] != '\0'; i++)
    {
        if (first == op_names[i])
            first = ops[i];

        if (second == op_names[i])
            second = ops[i];
    }

    return ((first == '*' && second == '-') || (first == '*' && second == '+') ||
            (first == '/' && second == '-') || (first == '/' && second == '+') ||
            (second == '('));
}
