module info.data.cluster;
////////////////////////////
import std.string;
import std.conv;
import std.exception;
import std.array : appender;
import std.format : formattedWrite;
////////////////////////////
import info.data.indiv;
/////////////////////////
class Cluster(T=int,U=int) : Indiv!(T,U) {
private:
	U[]	_members;
	real[] _sum;
public:
	this(){
		super();
	}
	this(U aIndex){
		super(aIndex);
	}
public:
	@property int count() const {
		return _members.length;
	}
	@property U[] members() const {
		return _members.dup;
	}
	void reset(){
		_members = [];
		_sum = [];
	}// reset
	void update_center(){
		immutable int ntotal = _members.length;
		if (ntotal > 0){
			immutable int nc = _sum.length;
			T[] result = [];
			for (int i = 0; i < nc; ++i){
				T val = cast(T)(_sum[i] / ntotal);
				result ~= val;
			}// i
			this.value(result);
		}// ntotal
	}// update_center
	bool add_indiv(in Indiv!(T,U) other, in bool bUpdate = false) 
	in {
		assert(!(other is null));
		assert(other.is_valid);
		assert(other.size > 0);
	}
	body{
		immutable int n = _members.length;
		immutable nIndex = other.index;
		immutable int nc = other.size;
		bool bFound = false;
		if (n < 1) {
			_sum = [];
			for (int i = 0; i < nc; ++i){
				_sum ~= other.value_at(i);
			}// i
		} else {
			assert(nc == this.size);
			foreach (aIndex; _members) {
				if (aIndex == nIndex){
					bFound = true;
					break;
				}
			}// foreach
			if (!bFound){
				for (int i = 0; i < nc; ++i){
					_sum[i] += other.value_at(i);
				}// i
			}// not found
		}
		if (!bFound){
			_members ~= nIndex;
			if (bUpdate) {
				this.update_center();
			}
		}// not found
		return (!bFound);
	}// add_indiv

public:
	override string toString() const {
		immutable n = _members.length;
		auto writer = appender!string();
		formattedWrite(writer,"%s\t[",this.index);
		for (int i = 0; i < n; ++i){
			if (i > 0){
				formattedWrite(writer,", ");
			}
			formattedWrite(writer,"%s",_members[i]);
		}// i
		formattedWrite(writer,"]");
		return writer.data;
	}// toString
}// class Cluster
//////////////////////////////////////
unittest {
	////////////////////////////////////////////
	auto c0 = new Cluster!(int);
	assert(c0.count == 0);
	assert(c0.members == []);
	c0.reset();
	////////////////////////////////////////////
	immutable int nIndex = 100;
	auto c1 = new Cluster!(int)(nIndex);
	assert(c1.count == 0);
	assert(c1.members == []);
	c1.reset();
	auto ind1 = new Indiv!(int,int)(1,[0,1,2]);
	c1.add_indiv(ind1,true);
	assert(c1.count == 1);
	assert(c1.members == [1]);
	auto ind2 = new Indiv!(int,int)(2,[3,4,5]);
	c1.add_indiv(ind2);
	assert(c1.count == 2);
	assert(c1.members == [1,2]);
	auto ind3 = new Indiv!(int,int)(3,[6,7,8]);
	c1.add_indiv(ind3);
	assert(c1.count == 3);
	assert(c1.members == [1,2,3]);
	auto ind4 = new Indiv!(int,int)(4,[9,10,11]);
	c1.add_indiv(ind4);
	assert(c1.count == 4);
	assert(c1.members == [1,2,3,4]);
	auto ind5 = new Indiv!(int,int)(5,[12,13,14]);
	c1.add_indiv(ind5);
	assert(c1.count == 5);
	assert(c1.members == [1,2,3,4,5]);
	c1.add_indiv(ind5);
	assert(c1.count == 5);
	assert(c1.members == [1,2,3,4,5]);
	///////////////////////////////////
	string sx0 = "100\t[1, 2, 3, 4, 5]";
	string sx = c1.toString();
	assert(sx == sx0);
}// unittest
//////////////////////////////////////
//eof: clster.d
