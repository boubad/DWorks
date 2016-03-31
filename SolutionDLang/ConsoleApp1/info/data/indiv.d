module info.data.indiv;
////////////////////////////
import std.string;
import std.conv;
import std.exception;
import std.array : appender;
import std.format : formattedWrite;
///////////////////////////
import info.data.distance;
/////////////////////////
class Indiv(T=int,U=int)
{
    static assert((U.stringof == "int")||(U.stringof == "short") || (U.stringof == "long") || (U.stringof == "uint")||(U.stringof == "ushort") || (U.stringof == "ulong"));
public:
    U index;
    string id;
    T[] data;
    //
    invariant {
        assert(index >= 0);
      
    }// invariant
public:
    this(in U aIndex, in T[] aData, in string sId="")
    in
    {
        assert(aIndex >= 0);
        assert(!(sId is null));
    } body
    {
        index = aIndex;
        id = sId;
        data = aData.dup;
    }
    //
public:
    override bool opEquals(Object o) const
    {
        auto rhs = cast(const Indiv!(T,U))o;
        return (rhs && (index == rhs.index));
    }// opEquals
    override int opCmp(Object o) const
		out(result)
		{
			assert((result == -1) || (result == 0) || (result == 1));
		}
    body
    {
        if (typeid(this) != typeid(o))
        {
            return typeid(this).opCmp(typeid(o));
        }
        auto rhs = cast(const Indiv!(T,U))o;
        if (index < rhs.index)
        {
            return (-1);
        }
        else if (index > rhs.index)
        {
            return (1);
        }
        else {
            return (0);
        }
    }// opCmp
    override size_t toHash() const
    {
        return (cast(size_t)index);
    }
}// class Indiv(T,U)
///////////////////////
unittest
{
    ////////////////////////////////
    immutable int[] data1 = [0,1,2,3,4];
    immutable int xIndex1 = 100;
    immutable string sId="testId";
    auto ind1 = new Indiv!(int,int)(xIndex1,data1);
    assert(ind1.index == xIndex1);


    //////////////////////////////////////
    immutable int[] data2 = [5,6,7,8,9];
    immutable int xIndex2 = 200;
    immutable string sId2="testId2";
    auto ind2 = new Indiv!(int,int)(xIndex2,data2);
    assert(ind2.index == xIndex2);

    ////////////////////////////////////

}// unittest
//////////////////////
//eof: indiv.d
