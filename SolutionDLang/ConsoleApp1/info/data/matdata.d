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
	private:
		int  _rows;
		int _cols;
		T[] _data;
	public:
		this(){
			_rows = 0;
			_cols = 0;
		}
		this(int nRows, int nCols, const T[] aData)
		in{
			assert(nRows > 0);
			assert(nCols > 0);
			assert(aData.length >= (nRows * nCols));
		}body{
			_rows = nRows;
			_cols = nCols;
			_data = aData[0..(nRows*nCols)].dup;
		}
		//
	public:
		bool is_valid() const @property { return ((_rows > 0)&& (_cols > 0) && (_data.length == (_cols * _rows)));}
		int cols() const @property { return (_cols);}
		int rows() const @property { return (_rows);}
		T[] row_at(int irow) const 
		in {
			assert((irow >= 0) && (irow < _rows));
		} body {
			enforce(_cols > 0);
			return _data[_cols * irow..((irow + 1) * _cols)].dup;
		}
		T[] col_at(int icol) const 
		in {
			assert((icol >= 0) && (icol < _cols));
		} body {
			enforce(_rows > 0);
			T[] resp = [];
			for (int i = 0; i < _rows; ++i){
				resp ~= _data[ i * _cols + icol];
			}// i
			return (resp);
		}
	public:
		override string toString() const {
			auto writer = appender!string();
			immutable nm1 = _rows - 1;
			for (int i = 0; i < _rows; ++i){
				for (int j = 0; j < _cols; ++j){
					if (j > 0){
						formattedWrite(writer,"\t");
					}
					formattedWrite(writer,"%s",_data[i * _cols + j]);
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
	auto m0 = new MatData!(int);
	assert(!m0.is_valid);
	assert(m0.cols == 0);
	assert(m0.rows == 0);
	//////////////////////////////////
	immutable int nRows = 5;
	immutable int nCols = 3;
	immutable int[] data = [0,1,2,3,4,5,6,7,8,9,10,11,12,13,14];
	auto m = new MatData!(int)(nRows,nCols,data);
	assert(m.is_valid);
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
	////////////////////////////////////
	string sOut = "0\t1\t2\n3\t4\t5\n6\t7\t8\n9\t10\t11\n12\t13\t14";
	assert(m.toString() == sOut);
	/////////////////////////////////
}// unittest