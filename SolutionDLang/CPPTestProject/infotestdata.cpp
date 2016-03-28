#include "stdafx.h"
#include "infotestdata.h"
//////////////////////////////////////
size_t InfoTestData::st_conso_cols = 7;
size_t InfoTestData::st_conso_rows = 12;
int InfoTestData::st_conso_data[] = { 
	332,428,354,1437,526,247,437,
	293,559,388,1527,567,239,258,
	372,767,562,1948,927,235,433,
	406,563,341,1507,544,324,407,
	386,608,396,1501,558,319,363,
	438,843,689,2345,1148,243,341,
	534,660,367,1620,638,414,407,
	460,699,484,1856,762,400,416,
	385,789,621,2366,1149,304,282,
	655,776,423,1848,759,495,486,
	584,995,548,2056,893,518,319,
	515,1097,887,2630,1187,561,284
};
const wchar_t *InfoTestData::st_conso_vars[] = { L"PAIN",L"LEGUMES",L"FRUITS",L"VIANDES",L"VOLAILLES",L"LAIT",L"VIN" };
const wchar_t *InfoTestData::st_conso_inds[] = { L"MA2",L"EM2",L"CA2",L"MA3",L"EM3",
L"CA3",L"MA4",L"EM4",L"CA4",L"MA5",L"EM5",L"CA5"};
///////////////////////////////////
size_t InfoTestData::st_socmortal_cols = 6;
size_t InfoTestData::st_socmortal_rows = 19;
int InfoTestData::st_socmortal_data[] = {241,16,330,43,363,325,
										156, 9, 225,10,535,328,
										85,19,349,7,281,345,
										210,12,230,21,298,269,
										156,10,260,13,367,144,
										251, 26,180,29,387,55,
										194,11,151,13,384,122,
										225, 9, 195,26,276,128,
										54,11,219,19,224,319,
										40,136,215,18,320,43,
										241,6,168,11,230,107,
										101,5,179,23,380,9,
										82,15,155,18,342,59,
										40,4,136,17,237,225,
										104,6,138,22,346,41,
										38,7,182,32,314,37,
										89,7,169,10,218,47,
										79,10,130,14,203,36,
										121,102,220,26,273,158
										};
const wchar_t *InfoTestData::st_socmortal_vars[] = { L"SUICI",L"HOMIC",L"AROUT",L"AINDU",L"AAUTR",L"CIRFO"};
const wchar_t *InfoTestData::st_socmortal_inds[] = {L"AUTRICHE",L"FRANCE",L"PORTUGAL",L"ALLFEDER",L"BELGIQUE",
									L"FINLANDE",L"SUEDE",L"SUISSE",L"ITALIE",L"IRLNORD",
									L"DANNEMARK",L"ISLANDE",L"ECOSSE",L"ESPAGNE",L"NORVEGE",
									L"IRLSUD",L"PABAS",L"ANGGALLE",L"USA"};
/////////////////////////////////////
InfoTestData::InfoTestData()
{
}


InfoTestData::~InfoTestData()
{
}
