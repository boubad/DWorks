module info.algo.clusterize;
/////////////////////
import std.random;
///////////////////////////////
import info.data.utils;
import info.data.cluster;
import info.data.matdata;
import info.data.distance;
////////////////////////////
class Clusterization(T=int,U = int) {
	private:
		DataMode _mode;
		int _nbclusters;
		Cluster!(T,U)[] _clusters;
		MatData!(T) _data;
		DistanceFunc!(T) _func;
		static  DistanceFunc!(T) _defaultDistanceFunc;
		static immutable int NB_ITER_MAX = 50;
		static this() {
			_defaultDistanceFunc = new ManhattanDistanceFunc!(T);
		}
	public:
		this(MatData!(T) pData,in int nbClusters = 5, in DataMode aMode = DataMode.rowMode,
			DistanceFunc!(T) fDist = _defaultDistanceFunc)
		in{
			assert(!(pData is null));
			assert(nbClusters > 0);
			assert(!(fDist is null));
			if (aMode == DataMode.rowMode){
				assert(nbClusters <= pData.rows);
			} else if (aMode == DataMode.colMode){
				assert(nbClusters <= pData.cols);
			}
		}
		body{
			_mode = aMode;
			_nbclusters = nbClusters;
			_clusters = [];
            _data = pData;
			_func = fDist;
		}
		invariant {
			assert(_nbclusters > 0);
			assert((_mode == DataMode.rowMode) || (_mode == DataMode.colMode));
			assert(!(_data is null));
			assert(!(_func is null));
		}// invariant
	public:
		@property DataMode mode() const {
			return _mode;
		}
		@property mode(in DataMode m) {
			if (m != _mode){
				_mode = m;
				_clusters = [];
			}
		}// mode
		@property int clusters_count() const {
			return (_clusters is null) ? 0 : _clusters.length;
		}
		@property DistanceFunc!(T) distance_function()  {
			return _func;
		}
		@property void distance_function(DistanceFunc!(T) f)
		in{
			assert(!(f is null));
		} 
		body {
				_func = f;
				_clusters = [];
		}
	protected:
		@property int ind_size() const {
			return (_mode == DataMode.rowMode) ? _data.rows : _data.cols;
		}// size
		@property int col_size() const {
			return (_mode == DataMode.rowMode) ? _data.cols : _data.rows;
		}// size
		T[] get_element(in int index) const 
		in {
			assert(index >= 0);
			assert(index < this.ind_size);
		}
		out(result){
			assert(!(result is null));
			assert(result.length == this.col_size);
		}
		body{
			return (_mode == DataMode.rowMode) ? _data.row_at(index) : _data.col_at(index);
		}// get_element
		void initialize(in int nbClusters)
		in{
			assert(nbClusters > 0);
			assert(nbClusters <= _nbclusters);
		}
		body{
			T[] minVals = [];
			T[] maxVals = [];
			_clusters = [];
			immutable int ninds = this.ind_size;
			immutable int ncols = this.col_size;
			minVals.length = ncols;
			maxVals.length = ncols;
			for (int i = 0; i < ninds; ++i){
				T[] v = this.get_element(i);
				for (int j = 0; j < ncols; ++j){
					immutable T val = v[j];
					if (i == 0){
						minVals[j] = val;
						maxVals[j] = val;
					} else {
						if (val < minVals[j]){
							minVals[j] = val;
						}
						if (val > maxVals[j]){
							maxVals[j] = val;
						}
					}
				}// j
			}// i
			_clusters.length = nbClusters;
			for (int i = 0; i < nbClusters; ++i){
				T[] xpoints = [];
				xpoints.length = ncols;
				for (int j = 0; j < ncols; ++j){
					immutable T v1 = minVals[i];
					immutable T v2 = maxVals[j];
					assert(v1 <= v2);
					xpoints[j] = uniform(v1,v2);
				}// j
				_clusters[i] = new Cluster!(T,U)(i,xpoints);
			}// i
		}// initialize
		int get_nearest_cluster( const T[] data, DistanceFunc!(T) fdist)
		in
		{
			assert(!(data is null));
			assert(data.length == this.col_size);
			assert(!(fdist is null));
		}
		out(result){
			assert((result >= 0) && (result < _clusters.length));
		}body {
			immutable int nn = _clusters.length;
			assert(nn > 0);
			auto c0 = _clusters[0];
			assert(!(c0 is null));
			T[] vv0 = c0.value;
			assert(!(vv0 is null));
			assert(vv0.length == this.col_size);
			real mindist = fdist(data,vv0);
			int minindex = 0;
			for (int i = 1; i < nn; ++i){
				auto c = _clusters[i];
				assert(!(c is null));
				T[] vv = c.value;
				assert(!(vv is null));
				assert(vv.length == this.col_size);
				real dd = fdist(data,vv);
				if (dd < mindist){
					mindist = dd;
					minindex = i;
				}
			}// i
			return minindex;
		}
		U[U] aggreg_one_step(DistanceFunc!(T) fdist)
		in{
			assert(!(fdist is null));
		}
		out(result){
			assert(!(result is null));
		}
		body{
			immutable int nc = _clusters.length;
			for (int i = 0; i < nc; ++i){
				auto c = _clusters[i];
				assert(!(c is null));
				c.reset();
			}// i
			immutable int n = this.ind_size();
			for (int i = 0; i < n; ++i){
				auto data = this.get_element(i);
				int idest = this. get_nearest_cluster(data, fdist);
				auto c = _clusters[idest];
				assert(!(c is null));
				c.add_array(cast(U)i,data,false);
			}// i
			U[U] result = [0:0];
			result.remove(0);
			for (int i = 0; i < nc; ++i){
				auto c = _clusters[i];
				assert(!(c is null));
				c.update_center();
				immutable U aIndex = c.index;
				foreach(key; c.members){
					result[key] = aIndex;
				}
			}// i
		 return result;
		}// aggreg_one_step
	public:
		bool clusterize(int nbIter = NB_ITER_MAX)
		in {
			assert(nbIter > 0);
		}
		body{
			bool bRet = false;
			this.initialize(_nbclusters);
			immutable int nc = _clusters.length;
			immutable int n = this.ind_size();
			U[U] oldResult = this.aggreg_one_step(_func);
			for (int iter = 0; iter < nbIter; ++iter){
				bool bDone = true;
				U[U] curResult = this.aggreg_one_step(_func);
				for (int i = 0; i < n; ++i){
					U aKey = cast(U)i;
					assert(aKey in oldResult);
					U val = oldResult[aKey];
					if (aKey in curResult){
						if (curResult[aKey] != val){
							bDone = false;
							break;
						}
					} else {
						bDone = false;
						break;
					}
				}// i
				if (bDone){
					bRet = true;
					break;
				}
				oldResult = curResult;
			}// iter
			return bRet;
		}// clusterize
}// Clusterization
/////////////////////////
//eof: clusterize.d
