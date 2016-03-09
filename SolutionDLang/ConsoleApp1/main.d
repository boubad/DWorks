import std.stdio;
////////////////////////
import info.data.indiv;
import info.data.matdata;
import info.data.cluster;
///////////////////////////
int main(string[] argv)
{
	immutable int nRows = 5;
	immutable int nCols = 3;
	immutable int[] data = [0,1,2,3,4,5,6,7,8,9,10,11,12,13,14];
	auto m = new MatData!(int)(nRows,nCols,data);
	auto xx = new Cluster!(int,int)(nRows);
	writeln(m);
	writeln();
    writeln("Hello D-World!");
    return 0;
}
