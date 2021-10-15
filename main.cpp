// necessary libraries are imported

#include <cmath>
#include <string>
#include <iomanip>
#include <cstring>
#include <iostream>
#include <exception>
#include <unordered_map>

// token type enumaration

enum tok_type_enum
{
	UNDIFINED,
	NUMBER,
	DELEMETER,
	FUNCTION,
	IDENTIFIER
};

// Error handling

enum Exception_Type
{
	NO_EXPR,
	UNHANDLED_BRACKET,
	DIV_BY_ZERO,
	SENTEX_ERROR,
	VAR_NOT_FOUND,
	FUNC_NOT_FOUND,
	EXPECT_A_COMMA,
	ASSIGN_VAR,
	NOT_A_VAR
};

class Parse
{
public:
	// necessary variables for token
	std::string expression;
	int index;
	int length;
	std::string token;
	enum tok_type_enum tok_type;
	int count_tok;
	int total_tok;

	// variables

	std::unordered_map<std::string, double> variables;

	// necessary operation for token management
	bool isIndexInRange();
	void get_token();
	void put_back();

	// parsing operation below
	void exec_assign_and_delete(double *result);	   // parform assignment in a variable
	void exec_add_and_sub(double *result);			   // perform add and sub operation
	void exec_mull_and_div(double *result);			   // perform mull and div operation
	void exec_pow_or_expo(double *result);			   // perform ^(pow operation)
	void exec_unary_pos_neg(double *result);		   // perform unary plus and minus opertion
	void exec_funs(double *result);					   // perform function operation
	void exec_braces(double *result);				   // perform brackets operation
	void convert_tok_and_var_to_a_num(double *result); // convert string to a number and perform prefix operation such !(factor)
													   //void exec_unary_pos_neg(double*);
protected:
	int factorial(int n)
	{
		int fact = n;
		for (int i = n - 1; i > 0; i--)
			fact *= i;
		return fact;
	}
	double radian_to_degree(double x)
	{
		return x * 3.1416 / 180;
	}

public:
	Parse()
	{
		total_tok = 0;
		variables.insert(std::make_pair("ans", 0.00));
	}

	double parse(std::string expr); // began to parse and perform equal operation
	void print_token()
	{
		std::cout << token << std::endl;
	}
};

bool Parse::isIndexInRange()
{
	return index < length;
}

void Parse::get_token()
{
	token = "";
	tok_type = UNDIFINED;

	while (isIndexInRange() && (isspace(expression[index]) || expression[index] == '\t'))
		index++;
	if (isIndexInRange() && strchr("+-*/x^(),!=", expression[index]))
	{
		tok_type = DELEMETER;
		token += expression[index];
		index++;
	}
	else if (isIndexInRange() && (isdigit(expression[index]) || expression[index] == '.'))
	{
		tok_type = NUMBER;
		while (isIndexInRange() && (isdigit(expression[index]) || expression[index] == '.'))
		{
			token += expression[index];
			index++;
		}
	}
	else if (isIndexInRange() && (isalpha(expression[index]) || strchr("_$", expression[index])))
	{
		tok_type = IDENTIFIER;
		while (isIndexInRange() && (isalnum(expression[index]) || strchr("_$", expression[index])))
		{
			token += expression[index];
			index++;
		}
	}
	if (token.length() > 0)
	{
		total_tok++;
		count_tok++;
	}
}

void Parse::put_back()
{
	index = index - token.length();
	total_tok--;
	count_tok--;
}

double Parse::parse(std::string expr)
{
	expression = expr;
	index = 0;
	length = expr.length();
	count_tok = 0;

	double result = 0;

	get_token();

	if (token == "")
		throw NO_EXPR;

	exec_assign_and_delete(&result);

	if (token != "")
	{
		print_token();
		throw SENTEX_ERROR;
	}
	std::unordered_map<std::string, double>::iterator ip = variables.find("ans");
	if (ip != variables.end())
		ip->second = result;
	else
		variables.insert(std::make_pair("ans", result));
	return result;
}

void Parse::exec_assign_and_delete(double *result)
{
	if (tok_type == IDENTIFIER)
	{

		std::string identifier = token;
		get_token();
		if (identifier == "del")
		{
			if (tok_type != IDENTIFIER)
				throw NOT_A_VAR;
			std::unordered_map<std::string, double>::iterator ip = variables.find(token);
			if (ip == variables.end())
				throw VAR_NOT_FOUND;
			variables.erase(token);
			get_token();
			if (token != "")
				throw SENTEX_ERROR;
			throw true;
		}
		else if (token == "=")
		{
			get_token();
			exec_add_and_sub(result);
			std::unordered_map<std::string, double>::iterator itr = variables.find(identifier);
			if (itr != variables.end())
			{
				itr->second = *result;
			}
			else
				variables.insert(std::make_pair(identifier, *result));
			throw ASSIGN_VAR;
		}
		else
		{
			put_back();
			token = identifier;
			tok_type = IDENTIFIER;

			exec_add_and_sub(result);
		}
	}
	else
	{
		exec_add_and_sub(result);
	}
}

void Parse::exec_add_and_sub(double *result)
{
	char ch;
	double temp;
	exec_mull_and_div(result);

	while ((ch = token[0]) == '+' || ch == '-')
	{
		get_token();
		exec_mull_and_div(&temp);

		switch (ch)
		{
		case '+':
			*result += temp;
			break;
		case '-':
			*result -= temp;
			break;
		}
	}
}

void Parse::exec_mull_and_div(double *result)
{
	char ch;
	double temp;
	exec_pow_or_expo(result);

	while ((ch = token[0]) == '*' || ch == '/')
	{
		get_token();
		exec_pow_or_expo(&temp);

		switch (ch)
		{
		case '*':
			*result *= temp;
			break;
		case '/':
		{
			if (temp == 0)
				throw DIV_BY_ZERO;
			*result /= temp;
		}
		break;
		}
	}
}

void Parse::exec_pow_or_expo(double *result)
{
	double temp = 0;
	exec_unary_pos_neg(result);
	if (token == "^")
	{
		get_token();
		exec_unary_pos_neg(&temp);
		*result = pow(*result, temp);
	}
}

void Parse::exec_unary_pos_neg(double *result)
{
	std::string oper = token;
	if (oper == "+" || oper == "-")
		get_token();
	exec_funs(result);
	if (oper == "-")
		*result = -*result;
}

void Parse::exec_funs(double *result)
{
	if (tok_type == IDENTIFIER)
	{
		std::string fun_name = token;
		get_token();
		if (token == "(")
		{
			get_token();
			if (fun_name == "exit")
			{
				if (count_tok != 3)
					throw SENTEX_ERROR;
				if (token != ")")
					throw UNHANDLED_BRACKET;
				get_token();
				if (token != "")
					throw SENTEX_ERROR;
				throw false;
			}
			else if (fun_name == "clear")
			{
				if (count_tok != 3)
					throw SENTEX_ERROR;
				if (token != ")")
					throw UNHANDLED_BRACKET;
				get_token();
				if (token != "")
					throw SENTEX_ERROR;
				system("cls");
				throw true;
			}
			else if (fun_name == "precision")
			{
				if (count_tok != 3)
					throw SENTEX_ERROR;
				double temp;
				exec_add_and_sub(&temp);
				if (token != ")")
					throw UNHANDLED_BRACKET;
				get_token();
				if (token != "")
					throw SENTEX_ERROR;
				std::cout << std::setprecision((int)temp);
				throw true;
			}
			else if (fun_name == "funs" || fun_name == "functions")
			{
				if (count_tok != 3)
					throw SENTEX_ERROR;
				if (token != ")")
					throw UNHANDLED_BRACKET;
				get_token();
				if (token != "")
					throw SENTEX_ERROR;
				std::cout << std::endl;
				std::cout << "    exit()                :       exit from the calculator (not for calculation)." << std::endl;
				std::cout << "    clear()               :       clear the console." << std::endl;
				std::cout << "    precision(n)          :       show precision of n decimal point" << std::endl;
				std::cout << "    funs()                :       show useful functions (not for calculation)." << std::endl;
				std::cout << "    vars()                :       show current stored variables." << std::endl;
				std::cout << "    sin(x)                :       sin of x." << std::endl;
				std::cout << "    cos(x)                :       cosin of x." << std::endl;
				std::cout << "    tan(x)                :       tangen of x." << std::endl;
				std::cout << "    pow(x,y)              :       y'th power of x." << std::endl;
				std::cout << "    root(x,y)             :       y'th root of x." << std::endl;
				std::cout << std::endl;
				throw true;
			}
			else if (fun_name == "vars")
			{
				if (count_tok != 3)
					throw SENTEX_ERROR;
				if (token != ")")
					throw UNHANDLED_BRACKET;
				get_token();
				if (token != "")
					throw SENTEX_ERROR;

				if (variables.empty())
					std::cout << "No variable exist" << std::endl;

				for (std::pair<std::string, double> var : variables)
				{
					std::cout << var.first << " : " << var.second << std::endl;
				}
				throw true;
			}
			else if (fun_name == "sin")
			{
				exec_add_and_sub(result);
				*result = radian_to_degree(*result);
				*result = sin(*result);
			}
			else if (fun_name == "cos")
			{
				exec_add_and_sub(result);
				*result = radian_to_degree(*result);
				*result = cos(*result);
			}
			else if (fun_name == "tan")
			{
				exec_add_and_sub(result);
				*result = radian_to_degree(*result);
				*result = tan(*result);
			}
			else if (fun_name == "pow")
			{
				double temp;
				exec_add_and_sub(result);
				if (token != ",")
					throw EXPECT_A_COMMA;
				get_token();
				exec_add_and_sub(&temp);
				*result = pow(*result, temp);
			}
			else if (fun_name == "root")
			{
				double temp;
				exec_add_and_sub(result);
				if (token != ",")
					throw EXPECT_A_COMMA;
				get_token();
				exec_add_and_sub(&temp);
				*result = pow(*result, 1 / temp);
			}
			else
				throw FUNC_NOT_FOUND;

			if (token != ")")
				throw UNHANDLED_BRACKET;
			get_token();
		}
		else
		{
			put_back();
			token = fun_name;
			tok_type = IDENTIFIER;
			exec_braces(result);
		}
	}
	else
		exec_braces(result);
}

void Parse::exec_braces(double *result)
{
	if (token[0] == '(')
	{
		get_token();
		exec_add_and_sub(result);

		if (token[0] != ')')
			throw UNHANDLED_BRACKET;
		get_token();
	}
	else
		convert_tok_and_var_to_a_num(result);
}

void Parse::convert_tok_and_var_to_a_num(double *result)
{
	char hasFactor = 0;

	switch (tok_type)
	{
	case NUMBER:
		*result = strtod(token.c_str(), NULL);
		get_token();
		break;
	case IDENTIFIER:
	{
		std::unordered_map<std::string, double>::iterator ip = variables.find(token);
		if (ip == variables.end())
			throw VAR_NOT_FOUND;
		*result = ip->second;
		get_token();
	}
	break;

	default:
		throw SENTEX_ERROR;
		break;
	}

	if (token == "!")
	{
		get_token();
		*result = (double)factorial((int)*result);
	}
}

void error(enum Exception_Type err)
{

	switch (err)
	{
	case NO_EXPR:
		break;
	case UNHANDLED_BRACKET:
		std::cout << "Unhandled bracket\n";
		break;
	case DIV_BY_ZERO:
		std::cout << "Can't div by zero\n";
		break;
	case SENTEX_ERROR:
		std::cout << "sentex error\n";
		break;
	case VAR_NOT_FOUND:
		std::cout << "variable not found" << std::endl;
		break;
	case NOT_A_VAR:
		std::cout << "not a variable" << std::endl;
		break;
	case FUNC_NOT_FOUND:
		std::cout << "function not found" << std::endl;
		break;
	case ASSIGN_VAR:
		return;
	}
}

int main()
{
	Parse p;

	std::string expr;
	bool running = true;

	std::cout.setf(std::ios::fixed, std::ios::floatfield);
	std::cout.setf(std::ios::showpoint);
	std::cout << std::setprecision(2);

	do
	{
		try
		{
			std::cout << ">>>";
			std::getline(std::cin, expr);
			std::cout << p.parse(expr) << std::endl;
		}
		catch (enum Exception_Type e)
		{
			error(e);
		}
		catch (bool run)
		{
			if (run == false)
				running = false;
		}
		catch (...)
		{
			std::cout << "Unhandled exception thrown" << std::endl;
			std::cin.clear();
			std::cin.get();
			return 0;
		}
	} while (running);
	return 0;
}