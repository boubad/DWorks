module info.data.distance;
///////////////////////
import std.math;
/////////////////////
class DistanceFunc(T,Z=double) {
	this(){
	}
	Z opCall(in T[] data1, in T[] data2) const
	in {
		assert(!(data1 is null));
		assert(!(data2 is null));
		assert(data2.length == data2.length);
	}
	out(result){
		assert(result >= 0);
	}
	body{
		return cast(Z)perform_compute(data1,data2);
	}// opCall
	protected:
		real compute_one_step(in T v1, in T v2, in int pos) const 
		{
			real r = v1 - v2;
			return (r * r);
		}// compute_one_step
		real perform_compute(in T[] data1, in T[] data2) const
		{
			real s = 0;
			immutable int n = data1.length;
			for (int i = 0; i < n; ++i){
				s  += compute_one_step(data1[i],data2[i],i);
			}// i
			if (n > 1){
				s /= n;
			}
			return s;
		}// opCall
}// class DistanceFunc!(T,Z)
/////////////////////////////////////////
class ManhattanDistanceFunc(T,Z=double) : DistanceFunc!(T,Z)
{
    this()
    {
        super();
    }
protected:
    override real compute_one_step(in T v1, in T v2, in int pos) const
    {
        real r = v1 - v2;
        return abs(r);
    }// compute_one_step
}// class ManhattanDistanceFunc
////////////////////////////////
class EuclideDistanceFunc(T,Z=double) : DistanceFunc!(T,Z)
{
    this()
    {
        super();
    }
protected:
    override real perform_compute(in T[] data1, in T[] data2) const
    {
        real s = super.perform_compute(data1, data2);
        return sqrt(s);
    }// perform_compute
}// class EuclideDistanceFunc
/////////////////////////////////////////////
class WeightedDistanceFunc(T,Z=double,W = double) : DistanceFunc!(T,Z)
{
public:
    const DistanceFunc!(T) distance_function;
    const W[] weights;
    invariant
    {
        assert(!(distance_function is null));
        assert(!(weights is null));
        assert(weights.length > 0);
    }// invaraint
    this(in W[] pw, in DistanceFunc!(T) fn)
    in
    {
        assert(!(fn is null));
        assert(!(pw is null));
        assert(pw.length > 0);
    }
    body
    {
        distance_function = fn;
        weights = pw;
    }
protected:
    override real perform_compute(in T[] data1, in T[] data2) const
    {
        real s = 0;
        immutable int n = data1.length;
        assert(weights.length >= n);
        for (int i = 0; i < n; ++i)
        {
            real r = distance_function.compute_one_step(data1[i],data2[i],i);
            real c = weights[i];
            assert(c >= 0);
            s  +=  c * r;
        }// i
        return s;
    }// perform_compute
} // class WeightedDistanceFunc
////////////////////////////////
unittest
{
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
    immutable real dr1 = 7.5;
    auto fnW1 = new WeightedDistanceFunc!(int,double,float)(weights,fnManHattan);
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
