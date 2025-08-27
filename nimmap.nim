import tables
var ret = initTable[string, string]()
for i in countup(0,5000000):
  ret["id." & $(i %% 500000)]="val." & $i
echo ret.len()
echo ret["id.10000"]
