#include "mmysql.h"

//constructors/destructors

mmysql::mmysql(	const char* _login, 
				const char* _password, 
				const char* _database_name)
{
	on_create();
	connect_to_mysql(_login, _password, _database_name);
}
mmysql::~mmysql()
{
	//code here
	mysql_close(connection);
}

//	--- METODS ---
//	Private  *****

void mmysql::null_refrences()
{
	table.name = NULL;
	request = NULL;
	//Error
	error.message = NULL;
	//describe table
	table_describe.columns = NULL;
	//read table
	table.columns = NULL;
}

void mmysql::on_create()
{
	is_connected = false;
	call_describe_table = false;
	call_read_table = false;
	null_refrences();
}

void mmysql::Put_error_to_struct()
{ //puts error no & error message to struct
	error.code = mysql_errno(&mysql);
	//error.message = mysql_error(&mysql);
	create_char_string(mysql_error(&mysql), error.message);
}

void mmysql::Put_error_to_struct(const int _code, const char* _message)
{ //puts error no & error message to struct
	error.code = _code;
	//error.message = _message;
	create_char_string(_message, error.message);
}

void mmysql::create_char_string(const char* str, char *(&Array))
{
	std::vector<char> v;
	int i = 0;
	while (str[i] != '\0')
	{
		v.push_back(str[i]); //put character to the vector
		i++;
	}
	if(v.size() != 0){
		//set_memory_to_char_array (const int elements, char *(&Array))
		if(Array != NULL)
			delete [] Array;
		Array = new char [v.size()+1];
		//-------------------------
		for (int i = 0; i < v.size(); i++)
			Array[i] = v[i];
		Array[v.size()] = '\0';
	}
	v.clear();
	//testing ----
	//cout << "In str: \"" << str << "\"\t Out str: \"" << Array << "\"\n";
}
//  CREATE REQUEST STRING ---
const char* mmysql::create_request_string(	const char* request_type, 
											const char* request_string)
{
	if(request != NULL) 
		delete [] request;

	int _size = strlen(request_type) + strlen(request_string);
	request = new char [_size+1];

	strcpy(request, request_type);
	strcat(request, request_string);
	const char* _request = request;

	return request;
}

const char* mmysql::_create_request_string(const int _parms, const char** _lines)
{
	if(request != NULL) 
		delete [] request;
	int _size = 0;
	for(int i = 0; i < _parms; i++)
		_size+=strlen(_lines[i]);
	request = new char [_size+1];
	strcpy(request, _lines[0]);
	for (int i = 1; i < _parms; i++)
		strcat(request, _lines[i]);
	const char* _request = request;
	return _request;
}
//  END CREATE REQUEST STRING ---

void mmysql::set_table_name(const char* _tablename)
{
	if(table.name != NULL)
		delete [] table.name;
	int len = strlen(_tablename);
	table.name = new char [len+1];
	for (int i = 0; i < len; i++)
		table.name[i] = _tablename[i];
	table.name[len] = '\0';
}

void mmysql::create_memory_for_table()
{
	//clear memory from previous request
	if(table.columns != NULL)
	{
		for (int i = 0; i < table.columns_count; i++)
		{
			for (int j = 0; j < table.row_count; j++)
				delete [] table.columns[i].cells[j];
			delete [] table.columns[i].column_name;
			delete [] table.columns[i].cells;
		}
		delete [] table.columns;
	}
	// create new memory
	table.columns_count = mysql_num_fields(result);
	table.row_count = result->row_count;
	table.columns = new Table_Column [table.columns_count];
	for (int i = 0; i < table.columns_count; i++)
	{
		table.columns[i].cells = new cell[table.row_count];
	}
}

void mmysql::fill_table()
{
	create_memory_for_table();
	//----------------
	{
		int i = 0;
		while ((row = mysql_fetch_row(result)) != NULL)
		{
			for(int j = 0; j < table.columns_count; j++)
			{
				create_char_string((row[j][0] == '\0' ? " " : row[j]),
					table.columns[j].cells[i]);
				//testing
				//cout << "row [" << j << "] = " << row[j] << endl;
			}
			i++;
		}
	}
	//----------------
	{
		int i = 0;
		while((field_info = mysql_fetch_field(result)))
		{
			create_char_string(field_info->name,
				table.columns[i].column_name);
			i++;
		}
	}
	//-----------------
}
//	Public	******

bool mmysql::connect_to_mysql(	const char* _login, 
								const char* _password, 
								const char* _database_name)
{
	if (!is_connected)
	{
		is_connected = true;
		mysql_init(&mysql);
		//---------------------------------
		connection = mysql_real_connect(&mysql,
			"localhost", _login, _password, _database_name,3306,0,0);
		//---------------------------------
		if (connection == NULL) {
			Put_error_to_struct();
			return false;
		} else
			return true;
	}
	else
	{
		Put_error_to_struct(1, "STD Error, you are previously connected");
		return false;
	}
}

bool mmysql::describe_table(const char* _tablename)
{
	if(_tablename == NULL)
	{
		Put_error_to_struct(2, 
			"STD Error, mmysql::describe_table() _tablename is empty!");
		return false;
	}
	const char* request = create_request_string("DESCRIBE ", _tablename);

	query_state = mysql_query(connection, request);
	if (query_state != 0)
	{
		Put_error_to_struct();
		return false;
	}
	result = mysql_store_result(connection);
	// table columns describe set size
	table_describe.columns_count = result->row_count;
	table_describe.columns = new Column_Describe[result->row_count];
	// очистка вказівників щоб не видаляти дані з кучі
	if(!call_describe_table)
	{
		call_describe_table = true;
		for (int i = 0; i < result->row_count; i++)
		{
			table_describe.columns[i].Field = NULL;
			table_describe.columns[i].Type = NULL;
			table_describe.columns[i].Null = NULL;
			table_describe.columns[i].Key = NULL;
			table_describe.columns[i].Default = NULL;
			table_describe.columns[i].Extra = NULL;
		}
	}
	int i = 0;
	while (( row = mysql_fetch_row(result)) != NULL) 
	{
		create_char_string(row[0], table_describe.columns[i].Field);
		create_char_string(row[1], table_describe.columns[i].Type);
		create_char_string(row[2], table_describe.columns[i].Null);
		create_char_string(((row[4] == NULL) ? " " : row[4]), table_describe.columns[i].Key);
		create_char_string(((row[4] == NULL) ? "NULL" : row[4]), table_describe.columns[i].Default);
		create_char_string(((row[5][0] == '\0') ? " ": row[5]), table_describe.columns[i].Extra);
		i++;
	}
	//---testing 
	//cout << "Request string: \"" << request << '\"' << endl;
	mysql_free_result(result);
	return true;
}

bool mmysql::read_table(const char* _tablename)
{
	if(!is_connected)
	{
		Put_error_to_struct(3, "STD Error, you aren't connected");
		return false;
	}
	if(_tablename == NULL)
	{
		Put_error_to_struct(2, 
			"STD Error, mmysql::read_table() _tablename is empty!");
		return false;
	}
	set_table_name(_tablename);
	const char* _request = create_request_string("SELECT * FROM ", _tablename);
	query_state = mysql_query(connection, _request);
	if (query_state != 0)
	{
		Put_error_to_struct();
		return false;
	}
	result = mysql_store_result(connection);
	fill_table();
	mysql_free_result(result);
	return true;
}

bool mmysql::read_table_by_column_name(const char* _tablename, const char* _column_name)
{
	if(!is_connected)
	{
		Put_error_to_struct(3, "STD Error, you aren't connected");
		return false;
	}
	if(_tablename == NULL)
	{
		Put_error_to_struct(2, 
			"STD Error, read_table_by_column_name() _tablename is empty!");
		return false;
	}
	if(_column_name == NULL)
	{
		Put_error_to_struct(4, 
			"STD Error, read_table_by_column_name() _column_name is empty!");
		return false;
	}
	set_table_name(_tablename);
	const char* _req[]={"SELECT ", _column_name,
						" FROM ", _tablename};
	const char* _request = 
		_create_request_string(4, _req);

	query_state = mysql_query(connection, _request);
	if (query_state != 0)
	{
		Put_error_to_struct();
		return false;
	}
	result = mysql_store_result(connection);
	fill_table();
	mysql_free_result(result);
	return true;
}

bool mmysql::select_where(const char* _tablename, const char* _where)
{
	if(!is_connected)
	{
		Put_error_to_struct(3, "STD Error, you aren't connected");
		return false;
	}
	if(_tablename == NULL)
	{
		Put_error_to_struct(2, 
			"STD Error, read_table_by_column_name() _tablename is empty!");
		return false;
	}
	if(_where == NULL)
	{
		Put_error_to_struct(5, 
			"STD Error, read_table_by_column_name() _where is empty!");
		return false;
	}
	set_table_name(_tablename);

	const char* _req[] = {	"SELECT * FROM ", _tablename,
							" WHERE ", _where};
	const char* _request = 
		_create_request_string(4, _req);

	query_state = mysql_query(connection, _request);
	if (query_state != 0)
	{
		Put_error_to_struct();
		return false;
	}
	result = mysql_store_result(connection);
	fill_table();
	mysql_free_result(result);
	return true;
}

bool mmysql::select_by_columns_where(	const char* _tablename, 
										const char* _columns_names,
										const char* _where)
{
	if(!is_connected)
	{
		Put_error_to_struct(3, "STD Error, you aren't connected");
		return false;
	}
	if(_tablename == NULL)
	{
		Put_error_to_struct(2, 
			"STD Error, select_by_columns_where() _tablename is empty!");
		return false;
	}
	if (_columns_names == NULL)
	{
		Put_error_to_struct(6, 
			"STD Error, select_by_columns_where() _columns_names is empty!");
		return false;
	}
	if(_where == NULL)
	{
		Put_error_to_struct(5, 
			"STD Error, select_by_columns_where() _where is empty!");
		return false;
	}
	set_table_name(_tablename);

	const char* _req[] = {	"SELECT ", _columns_names, 
	 						" FROM ", _tablename,
	 						" WHERE ", _where };
	const char* _request = _create_request_string(6, _req);

	query_state = mysql_query(connection, _request);
	if (query_state != 0)
	{
		Put_error_to_struct();
		return false;
	}
	result = mysql_store_result(connection);
	fill_table();
	mysql_free_result(result);
	return true;
}

bool mmysql::insert_to_table(const char* _tablename, const char* _values)
{
	const char* _req[] = {	"INSERT INTO ", _tablename,
							" VALUES ( ", _values, " )"};
	const char* _request = _create_request_string(5, _req);

	query_state = mysql_query(connection, _request);
	if (query_state != 0)
	{
		Put_error_to_struct();
		return false;
	}
	return true;
}