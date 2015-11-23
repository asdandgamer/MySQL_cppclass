#include "main.h"

int main(int argc, char *argv[])
{
	const char* tbl = "workers";
	mmysql mmy;
	if (!mmy.connect_to_mysql("root", "cpp123add", "cpp_try")) Show_error(mmy);

	//DESCRIBE
	if(!mmy.describe_table(tbl)) Show_error(mmy);
	print_describe(mmy);

	//SELECT * FROM table
	if(!mmy.read_table(tbl)) Show_error(mmy); 
	display_table(mmy);

	//SELECT age, id FROM table
	if(!mmy.read_table_by_column_name("people", "age, id")) Show_error(mmy);
	display_table(mmy);

	//SELECT * FROM table WHERE age=27
	/*if(!mmy.select_where("people", "age=27")) Show_error(mmy);
	display_table(mmy);*/

	//SELECT age, name FROM table WHERE id>2
	/*if(!mmy.select_by_columns_where(tbl, "name, sex", "id>2")) Show_error(mmy);
	display_table(mmy);*/

	//INSERT INTO table
	//const char* post[4] = { "7, ", "\'Petro\', ", "\'33\',", "\'male\'"};
	/*if(!mmy.insert_to_table(tbl, "\'9\', \'Angelina\', \'female\', \'1997-12-24\'")) Show_error(mmy);
	if(!mmy.read_table(tbl)) Show_error(mmy); 
	display_table(mmy);*/

	// APPLICATION END ---------------- 
	_getch(); return 0;
}

void display_table(const mmysql &mm)
{
	cout << "\n --- DISPLAY TABLE ---" << endl;
	for(int j = 0; j < mm.table.columns_count; j++)
		cout << mm.table.columns[j].column_name << "\t| ";
	cout << endl;
	for(int i = 0; i < mm.table.row_count; i++)
	{
		for(int j = 0; j < mm.table.columns_count; j++)
			cout << mm.table.columns[j].cells[i] << "\t| ";
		cout << endl;
	}
	cout << " *** end DISPLAY TABLE *** \n" << endl;
}

void print_describe(const mmysql &mm)
{
	cout << "\n --- DESCRIBE TABLE ---" << endl;
	cout << "#\t| " << "Field\t| " << "Type\t\t\t| " << "Null\t| " << "Key\t| " << "Default\t| " << "Extra" << std::endl;
	for (int i = 0; i < mm.table_describe.columns_count; i++){
		cout << (i+1) << "\t| " << mm.table_describe.columns[i].Field 
		<< "\t| " << mm.table_describe.columns[i].Type
		<< "\t\t| " << mm.table_describe.columns[i].Null
		<< "\t| " << mm.table_describe.columns[i].Key
		<< "\t| " << mm.table_describe.columns[i].Default
		<< "\t| " << mm.table_describe.columns[i].Extra << std::endl;
	}
	cout << " *** end DESCRIBE TABLE *** \n" << endl;
}

void Show_error(const mmysql &mm)
{
	cout << "Error: " << mm.err_code() << ' ' << mm.err_msg() << endl;
	_getch(); exit(1);
}