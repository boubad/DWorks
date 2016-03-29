module info.data.cluster;
////////////////////////////
import std.string;
import std.conv;
import std.exception;
import std.array : appender;
import std.format : formattedWrite;
////////////////////////////
import info.data.indiv;
/////////////////////////////
class ClusterDesc(U) {
private:
	U	_index;
	U[] _members;
	//
	invariant {
		assert(_index >= 0);
	}// invaraiant
public:
	this(){
		_members = [];
	}
	this(U aIndex)
	in{
		assert(aIndex >= 0);
	}
	body{
		_index = aIndex;
		_members = [];
	}
	this(U aIndex, const U[] data)
	in{
		assert(aIndex >= 0);
		assert(!(data is null));
	}
	body{
		_index = aIndex;
		_members = data.dup;
	}
public:
	override bool opEquals(Object o) const {
		auto rhs = cast(const ClusterDesc!(U))o;
		if (rhs is null){
			return false;
		}
		immutable int n = _members.length;
		if (rhs._members.length != n){
			return false;
		}
		foreach (v1; _members){
			bool bFound = false;
			foreach (v2; rhs._members){
				if (v1 == v2){
					bFound = true;
					break;
				}
			}// v2
			if (!bFound){
				return false;
			}
		}// v1
		return true;
	}// opEquals
public:
	ClusterDesc!(U) deepCopy() const {
		return new ClusterDesc!(U)(_index,_members);
	}// deepCopy
	@property U index() const {
		return _index;
	}
	@property int count() const 
		{
			return _members.length;
		}
		@property U[] members() const
			 {
				return _members.dup;
			}
		void reset(){
				_members = [];
			}
		bool contains(in U aIndex) const
			in{
				assert(aIndex >= 0);
				assert(!(_members is null));
			}
		body {
			immutable int n = _members.length;
			for (int i = 0; i < n; ++i){
				if (_members[i] == aIndex){
					return true;
				}
			}
			return false;
		}
		bool add(in U aIndex)
		in{
			assert(aIndex >= 0);
		}
		body {
			immutable int n = _members.length;
			for (int i = 0; i < n; ++i){
				if (_members[i] == aIndex){
					return false;
				}
			}
			_members ~= aIndex;
			return true;
		}
}// class ClusterDesc
/////////////////////////
class Cluster(T=int,U=int) : Indiv!(T,U) {
private:
	real[] _sum;
	ClusterDesc!(U) _desc;
public:
	this(){
		super();
		_sum = [];
		_desc = new ClusterDesc!(U);
	}
	this(U aIndex){
		super(aIndex);
		_sum = [];
		_desc = new ClusterDesc!(U)(aIndex);
	}
	this(U aIndex, const T[] data){
		super(aIndex,data);
		_sum = [];
		_desc = new ClusterDesc!(U)(aIndex);
	}
public:
	Cluster!(T,U) deepCopy() const {
		auto r = new Cluster!(T,U)(this.index,this.value);
		r._sum = _sum.dup;
		r._desc = _desc.deepCopy();
		return r;
	}
	@property ClusterDesc!(U) desc() 
	out(result){
		assert(!(result is null));
	}
	body{
		return _desc;
	}
	@property int count() const 
	out(result) {
			assert(result >= 0);
		}
		body
		{
			assert(!(this._desc is null));
			return _desc.count;
		}
		@property U[] members() const 
		out (result){
			assert(!(result is null));
		}body{
			return _desc.members;
		}
		@property real trace() const
		out(result){
			assert(result >= 0);
		}body{
			real s = 0;
			T[] vv = this.value;
			int n = vv.length;
			for (int i = 0; i < n; ++i){
				real t = vv[i];
				s += t * t;
			}
			return s;
		}
		void reset(){
			assert(!(_desc is null));
			_desc.reset();
			_sum = [];
		}// reset
		void update_center()
		{
			assert(!(_desc is null));
			immutable int ntotal = _desc.count;
			if (ntotal > 0){
				assert(!(_sum is null));
				immutable int nc = _sum.length;
				T[] result = [];
				result.length = nc;
				for (int i = 0; i < nc; ++i){
					result[i] = cast(T)(_sum[i] / ntotal);
				}// i
				this.value(result);
			}// ntotal
		}// update_center
		bool add_array(in U nIndex, in T[] xdata, in bool bUpdate = false)
		in {
			assert(nIndex >= 0);
			assert(!(xdata is null));
			assert(xdata.length > 0);
		}
		body {
			assert(!(_desc is null));
			immutable int n = _desc.count;
			immutable int nc = xdata.length;
			bool bFound = false;
			if (n < 1) {
				_sum = [];
				_sum.length = nc;
				for (int i = 0; i < nc; ++i){
					_sum[i] = xdata[i];
				}// i
			} else {
				assert(_sum.length == xdata.length);
				bFound = _desc.contains(nIndex);
				if (!bFound){
					for (int i = 0; i < nc; ++i){
						_sum[i] += xdata[i];
					}// i
				}// not found
			}
			if (!bFound){
				_desc.add(nIndex);
				if (bUpdate) {
					this.update_center();
				}
			}// not found
			return (!bFound);
		}// add_array
		bool add_indiv(in Indiv!(T,U) other, in bool bUpdate = false) 
		in {
			assert(!(other is null));
			assert(other.is_valid);
		}
		body{
			return this.add_array(other.index, other.value,bUpdate);
		}// add_indiv
	public:
		override bool opEquals(Object o) const {
			auto rhs = cast(const Cluster!(T,U))o;
			return (rhs && (this.trace == rhs.trace));
		}// opEquals
		override int opCmp(Object o) const
			out(result) {
				assert((result == -1) || (result == 0) || (result == 1));
			} body{
				if (typeid(this) != typeid(o)){
					return typeid(this).opCmp(typeid(o));
				}
				auto rhs = cast(const Cluster!(T,U))o;
				real t1 = this.trace;
				real t2 = rhs.trace;
				if (t1 < t2){
					return -1;
				} else if (t1 > t2){
					return 1;
				} else {
					return 0;
				}
			}// opCmp
		override string toString() const {
			immutable n = _desc.count;
			U[] xdata = _desc.members;
			T[] data = this.value;
			auto writer = appender!string();
			formattedWrite(writer,"{%s, %s, %s, %s}",this.index,data,xdata, this.trace);
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
}// unittest
//////////////////////////////////////
//eof: clster.d
