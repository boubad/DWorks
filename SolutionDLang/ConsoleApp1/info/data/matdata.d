module info.data.matdata;
////////////////////////////
import std.string;
import std.conv;
import std.exception;
import std.array : appender;
import std.format : formattedWrite;
////////////////////////////
import info.data.indiv;
/////////////////////////
class MatData(T){
	public:
		int  rows;
		int cols;
		const T[] data;
	public:
		this(in int nRows, in int nCols, in T[] aData)
		in{
			assert(nRows > 0);
			assert(nCols > 0);
			assert(!(aData is null));
			assert(aData.length >= (nRows * nCols));
		}body{
			rows = nRows;
			cols = nCols;
			data = aData;
		}
		//
	public:
		T[] row_at(int irow) const 
		in {
			assert((irow >= 0) && (irow < rows));
		} out(result) {
				assert(!(result is null));
				assert(result.length == cols);
		}body {
			enforce(cols > 0);
			return data[cols * irow..((irow + 1) * cols)].dup;
		}
		T[] col_at(int icol) const 
		in {
			assert((icol >= 0) && (icol < cols));
		} 
		out(result){
			assert(!(result is null));
			assert(result.length == rows);
		}body {
			enforce(rows > 0);
			T[] resp = [];
			for (int i = 0; i < rows; ++i){
				resp ~= data[ i * cols + icol];
			}// i
			return (resp);
		}
	public:
		override string toString() const {
			auto writer = appender!string();
			immutable nm1 = rows - 1;
			for (int i = 0; i < rows; ++i){
				for (int j = 0; j < cols; ++j){
					if (j > 0){
						formattedWrite(writer,"\t");
					}
					formattedWrite(writer,"%s",data[i * cols + j]);
				}// j
				if (i < nm1){
					formattedWrite(writer,"\n");
				}
			}// i
			return writer.data;
		}// toString
		
}// class MatData
///////////////////////////
unittest {
	immutable int nRows = 5;
	immutable int nCols = 3;
	immutable int[] data = [0,1,2,3,4,5,6,7,8,9,10,11,12,13,14];
	auto m = new MatData!(int)(nRows,nCols,data);
	assert(m.cols == nCols);
	assert(m.rows == nRows);
	///////////////////////////////
	assert(m.row_at(0) == [0,1,2]);
	assert(m.row_at(1) == [3,4,5]);
	assert(m.row_at(2) == [6,7,8]);
	assert(m.row_at(3) == [9,10,11]);
	assert(m.row_at(4) == [12,13,14]);
	/////////////////////////////////
	assert(m.col_at(0) ==[0,3,6,9,12]);
	assert(m.col_at(1) ==[1,4,7,10,13]);
	assert(m.col_at(2) ==[2,5,8,11,14]);
	/////////////////////////////////
}// unittest