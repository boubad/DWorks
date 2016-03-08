module info.data.indiv;
////////////////////////////
import std.string;
import std.conv;
import std.exception;
/////////////////////////
class Indiv(T=int,U=int) {
	static assert((U.stringof == "int")||(U.stringof == "short") || (U.stringof == "long") || (U.stringof == "uint")||(U.stringof == "ushort") || (U.stringof == "ulong"));
	private:
		U _index;
		T[] _data;
	public:
		this(){
			_index = cast(U)-1;
		}
		this(U aIndex, const T[] aData)
		in{
			assert(aIndex >= 0);
			assert(aData.length > 0);
		}body{
			_index = aIndex;
			_data = aData.dup;
		}
	//
	public:
		bool is_valid() const @property { return ((_index >= 0) && (_data.length >= 0));}
		U index() const @property { return _index;}
		U size() const @property { return _data.length;}
		T value_at(Z)(const Z icol) const 
			in {
				assert(cast(int)icol >= cast(int)0);
			}
			body {
				enforce(cast(int)icol < cast(int)_data.length);
				return (_data[cast(int)icol]);
			}
		
		double distance(const Indiv!(T,U) other) const
		in {
			assert(other._data.length == _data.length);
		}out(result){
			assert(result >= cast(T)0);
		}body {
			real s = compute_distance(other);
			return cast(double)s;
		}// distance
		void distance(Z)(in Indiv!(T,U) other, out Z resp) const
			in {
				assert(other._data.length == _data.length);
			}body {
				Z s = 0;
				immutable int n = _data.length;
				for (int i = 0; i < n; ++i){
					Z t = cast(Z)(_data[i] - other._data[i]);
					s += t * t;
				}// i
				resp = s;
			}// distance
	public:
		override string toString() const {
			string f = " %s %s";
			return format(f,_index,_data);
			//return "";
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
		real compute_distance(const Indiv!(T,U) other) const {
			real s = 0;
			immutable int n = _data.length;
			for (int i = 0; i < n; ++i){
				real t = _data[i] - other._data[i];
				s += t * t;
			}// i
			return (s);
		}// compute_distance
}// class Indiv(T,U)
///////////////////////
unittest {
	////////////////////////////////
	auto ind0 = new Indiv!(int,int);
	assert(!ind0.is_valid);
	assert(ind0.index == -1);
	assert(ind0.size == 0);
	////////////////////////////////
	immutable int[] data1 = [0,1,2,3,4];
	immutable int xIndex1 = 100;
	auto ind1 = new Indiv!(int,int)(xIndex1,data1);
	assert(ind1.is_valid);
	assert(ind1.index == xIndex1);
	assert(ind1.size == data1.length);
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
	double d1 = ind1.distance(ind2);
	assert(d1 == cast(double)125);
	double d2 = ind2.distance(ind1);
	assert(d1 == d2);
	///////////////////////////////////////
	int nd1, nd2;
	ind1.distance(ind2,nd1);
	assert(nd1 == cast(int)125);
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
	////////////////////////
	assert(ind1.toHash() == ind1.index);
	assert(ind2.toHash() == ind2.index);
	assert(ind11.toHash() == ind11.index);
}// unittest
//////////////////////
//eof: indiv.d
