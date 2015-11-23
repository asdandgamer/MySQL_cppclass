/*
	autor: asdandgamer
	date: 5.11.2015
	library for working with MySQL from C++
*/
#ifndef _mmysql_
#define _mmysql_

#pragma warning(disable : 4018)
#pragma warning(disable : 4996)
#pragma warning(disable : 4018)
#pragma warning(disable : 4244)

#include <WinSock2.h>
#include <vector>
#include <mysql.h>
#pragma comment(lib, "libmysql.lib")

#include <iostream>
using std::cout;
using std::endl;

// SELECT
typedef char* cell;

struct Table_Column
{
	char* column_name;
	cell* cells;
};

struct Table
{
	int row_count;
	int columns_count;
	char* name;
	Table_Column* columns;
};
//DESCRIBE [table name]
struct Column_Describe
{
	char* Field;
	char* Type;
	char* Null;
	char* Key;
	char* Default;
	char* Extra;
};

struct Table_Describe
{
	Column_Describe * columns;
	int columns_count;
};
// Errors out
struct Error
{
	int code;
	char* message;	
};
//---------------------

class mmysql
{
private: 
// varriables
	// MySQL defs :
	MYSQL *connection, mysql;
	MYSQL_RES *result;
	MYSQL_ROW row;
	MYSQL_FIELD *field_info;
	int query_state;
// My vars
	Error error;
	bool is_connected;
	char* request; //do not delete this!!!
	// regisering call functions
	bool call_describe_table;
	bool call_read_table;
// methods
	void on_create();
	void null_refrences();
	void Put_error_to_struct();
	void Put_error_to_struct(const int, const char*);
	void create_char_string(const char* str, char *(&array));
	const char* create_request_string(const char*, const char * );
	const char* _create_request_string(const int, const char*[]);
	void set_table_name(const char*);
	void create_memory_for_table();
	void fill_table();
public:
// varriables
	Table_Describe table_describe;
	Table table;
// constructors/destructors
	mmysql() { on_create(); }
	mmysql(const char*, const char*, const char*);
	~mmysql();
// methods
	bool connect_to_mysql(const char*, const char*, const char*);
	bool describe_table(const char*);
	bool read_table(const char*);
	bool read_table_by_column_name(const char*, const char*);
	bool select_where(const char*, const char*);
	bool select_by_columns_where(const char*, const char*, const char*);
	bool insert_to_table(const char*, const char*);
	// getters ----------------------
	bool Is_connected() const { return is_connected; }
	int err_code() const { return error.code; }
	char* err_msg() const { return error.message; }
};
/*
#pragma warning( 4018)
#pragma warning(enable : 4996)
#pragma warning(enable : 4018)
#pragma warning(enable : 4244)*/

#endif //_mmysql_