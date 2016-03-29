module info.data.distance;
///////////////////////
import std.math;
/////////////////////
class DistanceFunc(T) {
	this(){
	}
	real opCall(in T[] data1, in T[] data2) const
	in {
		assert(!(data1 is null));
		assert(!(data2 is null));
		assert(data1.length > 0);
		assert(data2.length == data2.length);
	}
	out(result){
		assert(result >= 0);
	}
	body{
		return perform_compute(data1,data2);
	}// opCall
	protected:
		real compute_one_step(in T v1, in T v2, in int pos = -1) const 
		out(result){
			assert(result >= 0);
		}
		body
		{
			real r = v1 - v2;
			return (r < 0) ? -r : r;
		}// compute_one_step
		real perform_compute(in T[] data1, in T[] data2) const
		in {
				assert(!(data1 is null));
				assert(data1.length > 0);
				assert(!(data2 is null));
				assert(data1.length == data2.length);
		}
		out(result){
			assert(result >= 0);
		}
		body{
			real s = 0;
			immutable int n = data1.length;
			for (int i = 0; i < n; ++i){
				s  += this.compute_one_step(data1[i],data2[i],i);
			}// i
			return (s/n);
		}// opCall
}// class DistanceFunc
/////////////////////////////////////////
class ManhattanDistanceFunc(T) : DistanceFunc!(T) {
	this(){
		super();
	}
}// class ManhattanDistanceFunc
////////////////////////////////
class EuclideDistanceFunc(T) : DistanceFunc!(T) {
	this(){
		super();
	}
protected:
	override real compute_one_step(in T v1, in T v2, in int pos = -1) const {
		real r = v1 - v2;
		return  (r * r);
	}// compute_one_step
	override real perform_compute(in T[] data1, in T[] data2) const{
		real s = super.perform_compute(data1,data2);
		return sqrt(s);
	}//perform_compute
}// class EuclideDistanceFunc
/////////////////////////////////////////////
class WeightedDistanceFunc(T,W = float) : DistanceFunc!(T) {
	private:
		const DistanceFunc!(T) _df;
		W[] _weights;
		invariant {
			assert(!(_df is null));
			assert(!(_weights is null));
		}// invaraint
	public:
		this(const W[] pw, const  DistanceFunc!(T) fn)
		in {
			assert(!(fn is null));
			assert(pw.length > 0);
		}
		body{
			_df = fn;
			_weights = pw.dup;
		}
	protected:
		override real compute_one_step(in T v1, in T v2, in int pos) const {
			assert((pos >= 0) && (pos < _weights.length));
			assert(_weights[pos] >= 0);
			real r = _df.compute_one_step(v1,v2,pos);
			assert(r >= 0);
			return (r * _weights[pos]);
		}// compute_one_step
} // class WeightedDistanceFunc
////////////////////////////////
unittest {
	/////////////////////////////
	immutable real epsilon = 0.00001;
	immutable int[] data1 = [0,1,2,3,4];
	immutable int[] data2 = [5,6,7,8,9];
	immutable float[] weights = [0.1, 0.2, 0.3, 0.4,0.5];
	/////////////////////////////////
	immutable double dx0 = 5;
	auto fnManHattan = new ManhattanDistanceFunc!(int);
	real d1 = fnManHattan(data1,data2);
	assert(d1 == dx0);
	///////////////////////////////////
	immutable real dr1 = 1.5;
	auto fnW1 = new WeightedDistanceFunc!(int,float)(weights,fnManHattan);
	real d2 = fnW1(data1,data2);
	auto t = (d2 > dr1) ? d2 - dr1 : dr1 - d2;
	assert(t < epsilon);
	///////////////////////////////
	immutable real dy0 = 5;
	auto fnEuclide = new EuclideDistanceFunc!(int);
	real d3 = fnEuclide(data1,data2);
	assert(d3 == dy0);
	///////////////////////////////////
}// unittest
