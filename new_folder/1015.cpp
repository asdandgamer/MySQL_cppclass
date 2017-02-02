#include <iostream>
#include <conio.h>
using namespace std;

int main()
{
	double m, n, p, q, i, j;
	
	m = (3.0 + (1.0 / 4)) * -1.0;
	n = -1.8;
	p = (2.0 + (3.0 / 4)) * -1.0;
	q = -2.2;
	
	i = m + n;
	j = p + q;

	if (i > j)	cout << "(m + n) > (p + q)";
	else if (i < j) cout << "(m + n) < (p + q)";
	else cout << "(m + n) = (p + q)";
	
	cout << "\nResults : (m + n) = " << i << "; (p + q) = " << j << endl;

	_getch(); return 0;
}