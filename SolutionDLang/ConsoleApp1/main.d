import std.stdio;
import std.random;
////////////////////////
import info.data.indiv;
import info.data.matdata;
import info.data.cluster;
import info.algo.clusterize;
///////////////////////////
int main(string[] argv)
{
	int nRows = 28;
	int nCols = 3;
	int nTotal = nRows * nCols;
	int[] data = [];
	data.length = nTotal;
	int nMin = 0;
	int nMax = 20;
	int n = data.length;
	for (int i = 0; i < n; ++i){
		data[i] = uniform(nMin,nMax);
	}// i
	auto m = new MatData!(int)(nRows,nCols,data);
	writeln(m);
	writeln();
	auto yy = new Clusterization!(int,int)(m,5);
	int counter = yy.clusterize();
	writeln("count\t",counter);
	writeln(yy);
	writeln();
    writeln("Hello D-World!");
    return 0;
}
