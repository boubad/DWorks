module info.data.indiv;
////////////////////////////
import std.string;
import std.conv;
import std.exception;
import std.array : appender;
import std.format : formattedWrite;
/////////////////////////////
import info.data.distance;
/////////////////////////
class Indiv(T=int,U=int) {
	static assert((U.stringof == "int")||(U.stringof == "short") || (U.stringof == "long") || (U.stringof == "uint")||(U.stringof == "ushort") || (U.stringof == "ulong"));
	private:
		U _index;
		T[] _data;
		static  DistanceFunc!(T) _defaultDistanceFunc;
		static this() {
			_defaultDistanceFunc = new ManhattanDistanceFunc!(T);
		}
	public:
		this(){
			_index = cast(U)-1;
		}
		this(U aIndex){
			_index = aIndex;
		}
		this(U aIndex, const T[] aData)
		in{
			assert(aIndex >= 0);
			assert(aData.length >= 0);
		}body{
			_index = aIndex;
			_data = aData.dup;
		}
	//
	public:
		bool is_valid() const @property { return ((_index >= 0) && (_data.length >= 0));}
		U index() const @property { return _index;}
		U size() const @property { return _data.length;}
		@property T[] value() const {
			return _data.dup;
		}
		@property void value(const T[] xdata){
			_data = xdata.dup;
		}// value
		T value_at(Z)(const Z icol) const 
			in {
				assert(cast(int)icol >= cast(int)0);
				assert(cast(int)icol < cast(int)_data.length);
			}
			body {
				
				return (_data[cast(int)icol]);
			}
		
		double distance(const Indiv!(T,U) other, DistanceFunc!(T) func = _defaultDistanceFunc) const
		in {
			assert(!(func is null));
			assert(other._data.length == _data.length);
		}out(result){
			assert(result >= 0);
		}body {
			real s = compute_distance(other, func);
			return cast(double)s;
		}// distance
		void distance(Z)(in Indiv!(T,U) other, out Z resp, DistanceFunc!(T) func = _defaultDistanceFunc) const
			in {
				assert(other._data.length == _data.length);
			}body {
				resp = cast(Z)compute_distance(other, func);
				assert(resp >= 0);
			}// distance
	public:
		override string toString() const {
			immutable n = _data.length;
			auto writer = appender!string();
			formattedWrite(writer,"%s\t[",_index);
			for (int i = 0; i < n; ++i){
				if (i > 0){
					formattedWrite(writer,", ");
				}
				formattedWrite(writer,"%s",_data[i]);
			}// i
			formattedWrite(writer,"]");
			return writer.data;
		}// toString
		override bool opEquals(Object o) const {
			auto rhs = cast(const Indiv!(T,U))o;
			return (rhs && (_index == rhs._index));
		}// opEquals
		override int opCmp(Object o) const
		out(result) {
			assert((result == -1) || (result == 0) || (result == 1));
		} body{
			if (typeid(this) != typeid(o)){
				return typeid(this).opCmp(typeid(o));
			}
			auto rhs = cast(const Indiv!(T,U))o;
			if (_index < rhs._index) {
				return (-1);
			} else if (_index > rhs._index){
				return (1);
			} else {
				return (0);
			}
		}// opCmp
		override size_t toHash() const {
			return (cast(size_t)_index);
		}
	private:
		real compute_distance(const Indiv!(T,U) other,DistanceFunc!(T) func) const {
			real s = func(_data, other._data);
			return (s);
		}// compute_distance
}// class Indiv(T,U)
///////////////////////
unittest {
	class Toto {
		int a;
	}
	////////////////////////////////
	auto ind0 = new Indiv!(int,int);
	assert(!ind0.is_valid);
	assert(ind0.index == -1);
	assert(ind0.size == 0);
	assert(ind0.value == []);
	////////////////////////////////
	int ii = 56;
	auto nx = new Indiv!(int)(ii);
	assert(nx.is_valid);
	assert(nx.index == ii);
	assert(nx.size == 0);
	////////////////////////////////
	immutable int[] data1 = [0,1,2,3,4];
	immutable int xIndex1 = 100;
	auto ind1 = new Indiv!(int,int)(xIndex1,data1);
	assert(ind1.is_valid);
	assert(ind1.index == xIndex1);
	assert(ind1.size == data1.length);
	assert(ind1.value == [0,1,2,3,4]);
	assert(ind1.value_at(0) == 0);
	assert(ind1.value_at(1) == 1);
	assert(ind1.value_at(2) == 2);
	assert(ind1.value_at(3) == 3);
	assert(ind1.value_at(4) == 4);
	//////////////////////////////////////
	immutable int[] data2 = [5,6,7,8,9];
	immutable int xIndex2 = 200;
	auto ind2 = new Indiv!(int,int)(xIndex2,data2);
	assert(ind2.is_valid);
	assert(ind2.index == xIndex2);
	assert(ind2.size == data2.length);
	assert(ind2.value_at(0) == 5);
	assert(ind2.value_at(1) == 6);
	assert(ind2.value_at(2) == 7);
	assert(ind2.value_at(3) == 8);
	assert(ind2.value_at(4) == 9);
	////////////////////////////////////
	double xx  = 25;
	double d1 = ind1.distance(ind2);
	assert(d1 == xx);
	double d2 = ind2.distance(ind1);
	assert(d1 == d2);
	///////////////////////////////////////
	int nd1, nd2;
	ind1.distance(ind2,nd1);
	assert(nd1 == xx);
	ind2.distance(ind1,nd2);
	assert(nd1 == nd2);
	
	//////////////////////////////////////
	immutable int xIndex3 = 10;
	auto ind11 = new Indiv!(int,int)(xIndex1,[7,8,9]);
	auto indxx = new Indiv!(int,int)(xIndex3,[7,8,9]);
	///////////////////////////////////////////////////
	assert(ind1 == ind11);
	assert(ind1 != ind2);
	assert(indxx < ind1);
	assert(ind1 > indxx);
	assert(ind1.opCmp(ind11) == 0);
	auto d = new Toto;
	assert(ind1.opCmp(d) != 0);
	////////////////////////
	assert(ind1.toHash() == ind1.index);
	assert(ind2.toHash() == ind2.index);
	assert(ind11.toHash() == ind11.index);
	/////////////////////////////////////
	string sOut = "100\t[0, 1, 2, 3, 4]";
	assert(ind1.toString() == sOut);
}// unittest
//////////////////////
//eof: indiv.d
