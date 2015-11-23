#ifndef _main_h_
#define _main_h_
#include "mmysql.h"

#include <conio.h>
#include <iostream>
using std::cout;
using std::endl;

void Show_error(const mmysql &);

void print_describe(const mmysql &);

void display_table(const mmysql &);

int main(int , char*[]);

#endif //_main_h_