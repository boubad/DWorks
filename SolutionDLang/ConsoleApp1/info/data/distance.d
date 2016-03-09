module info.data.distance;
/////////////////////
class DistanceFunc(T) {
	this(){
	}
	double opCall(const T[] data1, const T[] data2) {
		return perform_compute(data1,data2);
	}// opCall
	protected:
		real compute_one_step(in T v1, in T v2, int pos = -1) const {
			real r = v1 - v2;
			return (r < 0) ? -r : r;
		}// compute_one_step
		double perform_compute(const T[] data1, const T[] data2) const
		in {
				assert(data1.length == data2.length);
		}
		out(result){
			assert(result >= 0);
		}
		body{
			assert(data1.length == data2.length);
			real s = 0;
			immutable int n = data1.length;
			for (int i = 0; i < n; ++i){
				s  += this.compute_one_step(data1[i],data2[i],i);
			}// i
			return cast(double)s;
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
	override real compute_one_step(in T v1, in T v2, int pos = -1) const {
		real r = v1 - v2;
		return  (r * r);
	}// compute_one_step
}// class EuclideDistanceFunc
/////////////////////////////////////////////
class WeightedDistanceFunc(T,W = float) : DistanceFunc!(T) {
	private:
		const DistanceFunc!(T) _df;
		W[] _weights;
	public:
		this(const W[] pw, const  DistanceFunc!(T) fn){
			assert(!(fn is null));
			assert(pw.length > 0);
			_df = fn;
			_weights = pw.dup;
		}
	protected:
		override real compute_one_step(in T v1, in T v2, int pos) const {
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
	immutable double epsilon = 0.00001;
	immutable int[] data1 = [0,1,2,3,4];
	immutable int[] data2 = [5,6,7,8,9];
	immutable float[] weights = [0.1, 0.2, 0.3, 0.4,0.5];
	/////////////////////////////////
	immutable double dx0 = 25;
	auto fnManHattan = new ManhattanDistanceFunc!(int);
	double d1 = fnManHattan(data1,data2);
	assert(d1 == dx0);
	///////////////////////////////////
	immutable double dr1 = 7.5;
	auto fnW1 = new WeightedDistanceFunc!(int,float)(weights,fnManHattan);
	double d2 = fnW1(data1,data2);
	auto t = (d2 > dr1) ? d2 - dr1 : dr1 - d2;
	assert(t < epsilon);
	///////////////////////////////
	immutable double dy0 = 125;
	auto fnEuclide = new EuclideDistanceFunc!(int);
	double d3 = fnEuclide(data1,data2);
	assert(d3 == dy0);
	///////////////////////////////////
}// unittest
